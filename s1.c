#include "heads.h"
#define PORT 13055

struct idpass{
    char id[NAME_SIZE];
    char pass[PWD_SIZE];
}idpass;
int numUsers=0;

void addQueue(client *new){
    pthread_mutex_lock(&clients_mutex);
    for(int i=0;i<numUsers;i++){
        if(clients[i]==NULL){
            clients[i]=new;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void removeQueue(int sid){
    pthread_mutex_lock(&clients_mutex);
    for(int i=0;i<numUsers;i++){
        if(clients[i] != NULL && clients[i]->sockfd==sid){
            clients[i]=NULL;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void newUser(int sockid, struct idpass ip){
    for(int i=0;i<numUsers;i++){
        if(strcmp(users[i]->username,ip.id)==0) {
            return;
        }
    }
    
    if(numUsers<MAX){
        users[numUsers] = malloc(sizeof(user));
        strcpy(users[numUsers]->username,ip.id);
        strcpy(users[numUsers]->password,ip.pass);
        numUsers++;
        return;
    }
    else
        printf("Registration unsuccessful\n");
    
}

void broadcast_join(char nam[50]){

    pthread_mutex_lock(&clients_mutex);
    char buf[MSG_SIZE];
    for(int i=0;i<numUsers;i++){
        if(clients[i]!=NULL && strcmp(clients[i]->name,nam)!=0){
            sprintf(buf, "------- %s JOINED THE CHAT -------\n", nam);
            message* msg = (message*)malloc(sizeof(message));
            strcpy(msg->from,"Server");
            strcpy(msg->to,clients[i]->name);
            msg->type=1;
            strcpy(msg->msg,buf);
            write(clients[i]->sockfd, msg, sizeof(message));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void broadcast_leave(char name[NAME_SIZE]){
    pthread_mutex_lock(&clients_mutex);
    char buf[MSG_SIZE];
    printf("%s logged out\n",name);
    for(int i=0;i<numUsers;i++){
        if(clients[i]!=NULL && strcmp(clients[i]->name,name)!=0){
            sprintf(buf, "------- %s has left the chat -------\n", name);
            message* msg = (message*)malloc(sizeof(message));
            strcpy(msg->from,"Server");
            strcpy(msg->to,clients[i]->name);
            msg->type=1;
            strcpy(msg->msg,buf);
            write(clients[i]->sockfd, msg, sizeof(message));
    
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}


void printCurrUsers(client *c){

    pthread_mutex_lock(&clients_mutex);
    char buf[MSG_SIZE];
   
    for(int i=0;i<numUsers;i++){
        if(clients[i]!=NULL && clients[i]->name!=c->name){
            sprintf(buf, "------- %s is online -------\n", clients[i]->name);
            message* msg = (message*)malloc(sizeof(message));
            strcpy(msg->from,"Server");
            strcpy(msg->to,c->name);
            msg->type=1;
            strcpy(msg->msg,buf);
            write(c->sockfd, msg, sizeof(message));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}
void forward(message *msg){

    pthread_mutex_lock(&clients_mutex);
    printf("Forwarded from %s to %s\n",msg->from,msg->to);
    for(int i=0;i<numUsers;i++){
        if(clients[i]!=NULL){
        if(strcmp(clients[i]->name, msg->to)==0){
            write(clients[i]->sockfd, msg, sizeof(message));
            break;
    
        }}      
    }
    pthread_mutex_unlock(&clients_mutex);
}
void broadcast(message *msg){

    pthread_mutex_lock(&clients_mutex);
    for(int i=0;i<numUsers;i++){
        if(clients[i]!=NULL){
        if(strcmp(clients[i]->name, msg->from)!=0 ){
            write(clients[i]->sockfd, msg, sizeof(message));    
        }   }    
    }
    pthread_mutex_unlock(&clients_mutex);
}

void handleClient(void* arg){

    client *thisClient = (client *) arg;
    message *receivedMessage = (message*)malloc(sizeof(message));


    while (1) {

        int n=recv(thisClient->sockfd, receivedMessage, sizeof(message),MSG_WAITALL);
        if(n<=0) break;
        if (strcmp(receivedMessage->msg,"Close")==0) {
            broadcast_leave(thisClient->name);
            break;
        }
        if(strcmp(receivedMessage->to,"all")==0){
            broadcast(receivedMessage);
        }
        else {
            forward(receivedMessage);
        }
    }

    close(thisClient->sockfd);
    removeQueue(thisClient->sockfd);
    free(thisClient);
    pthread_detach(pthread_self());
    return;
}

char *user_db_file = "user_db.txt";

void read_user_db()
{
  FILE *fp;
  char *user, *passwd;
  struct idpass ip;
  char buf[120];

  printf("Reading user_db\n");

  fp = fopen(user_db_file, "r");
  if (fp == NULL) {
    printf("user_db_file is empty\n");
    return;
  }
  while (fgets(buf, sizeof(buf), fp) != NULL) {
    if ((buf[0] == '#') || (buf[0] == ' ') || (buf[0] == '\t') || (buf[0] == '\n')) {
         continue;
    }
    user   = strtok(buf, "=\n\r");
    passwd = strtok(NULL, "=\n\r");
    if (user == NULL || passwd == NULL) {
        continue;
    }
    if ((strlen(user) > 0) && (strlen(passwd) > 0)) {
      strncpy(ip.id, user, sizeof(ip.id));
      strncpy(ip.pass, passwd, sizeof(ip.pass));
      printf("user=%s\n", user);
      newUser(0, ip);
    }
  }
  printf("user_db: num_users=%d\n", numUsers);
  fclose(fp);
}

void save_user_db(void)
{
  FILE *fp;
  int ix;

  fp = fopen(user_db_file, "w");
  if (fp == NULL) {
    printf("unable to open user_db_file\n");
    return;
  }
  for (ix=0; ix < numUsers; ix++) {
    fprintf(fp, "%s=%s\n", users[ix]->username, users[ix]->password);
  }
  fclose(fp);
}

int main(){
    int mainsockfd, clisockfd;
    struct sockaddr_in servadd, cliadd;
    char user[NAME_SIZE],pswd[PWD_SIZE];
    pthread_t tid;
     
    //creating the main socket that listens for connections.
    mainsockfd=socket(AF_INET, SOCK_STREAM,0);
    if(mainsockfd<0){
        perror("Socket not created\n");
        exit(EXIT_FAILURE);
    }
    servadd.sin_family= AF_INET;
    servadd.sin_addr.s_addr = INADDR_ANY;
    servadd.sin_port=htons(PORT);
    int opt = 1;
    setsockopt(mainsockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, sizeof(opt));

    int res= bind(mainsockfd,(struct sockaddr*)&servadd, sizeof(servadd));
    if(res==-1){
        perror("Bind error\n");
        exit(EXIT_FAILURE);
    }
   
    printf("Server listening on port %d\n",PORT);

    listen(mainsockfd,MAX);

    memset(clients,0,sizeof(clients));
    memset(users,0,sizeof(users));
    read_user_db();

    while(1)
    {
        int clilen=sizeof(cliadd);
        clisockfd = accept(mainsockfd,(struct sockaddr*)&cliadd, &clilen);
        if(clisockfd<0){
        perror("Accept error\n");
        }
        int reg;
        read(clisockfd,&reg,sizeof(int));
        struct idpass ip;
        read(clisockfd,&ip,sizeof(ip));
        if(reg==1){
            // register new user
            newUser(clisockfd, ip);
            save_user_db();
            continue;
        }

        // Login(must be done even after new registration)

        for(int j=0;j<numUsers;j++){
            if(strcmp(ip.id,users[j]->username)==0 && strcmp(ip.pass,users[j]->password)==0){
                printf("%s logged in\n",ip.id);
                client *cli = (client*)malloc(sizeof(client));
                if(cli ==NULL){
                    printf("Mem allocation failed\n");
                }
                cli->addr = cliadd;
                cli->sockfd = clisockfd;
                strcpy(cli->name, ip.id);
                addQueue(cli);
                broadcast_join(cli->name);
                printCurrUsers(cli);
                pthread_create(&tid, NULL, (void *) handleClient, (void*)cli);
            }            
        }
    }
}
