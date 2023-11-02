#include "heads.h"
#define PORT 13055

void send_handler(int *);
void recv_handler(int *);
struct idpass{
    char id[NAME_SIZE];
    char pass[PWD_SIZE];
}idpass;


char usn[NAME_SIZE], pwd[PWD_SIZE];

int main(int argc, char * argv[]) {
    int sockfd;
    struct sockaddr_in server_add;
    pthread_t send_thread, recv_thread;

    char *srv_ip;

    if (argc != 2) {
      printf("usage: ./progname <ip_addr>\n");
      exit(0);
    }

    srv_ip = argv[1];


    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket not created\n");
        exit(EXIT_FAILURE);
    }

    server_add.sin_family = AF_INET;
    server_add.sin_addr.s_addr = inet_addr(srv_ip);
    server_add.sin_port = htons(PORT);
    int len = sizeof(server_add);
    int res=connect(sockfd,(struct sockaddr *)&server_add,len);
    if (res==-1) {
        perror("Socket not connected");
        exit(1);
    }

    int reg;
    printf("Login(0) or Register(1): ");
    scanf("%d",&reg);
    printf("%d\n",reg);
    write(sockfd,&reg,sizeof(int));
    if(reg==1){
        printf("Enter username and password for registration\n");
        scanf("%s",usn);
        scanf("%s",pwd);
        struct idpass ip;
        strcpy(ip.id,usn);
        strcpy(ip.pass,pwd);
        write(sockfd,&ip,sizeof(ip));
        printf("Restart and login\n");
        close(sockfd);      
        
        exit(EXIT_SUCCESS);

    }

    //Login
    printf("Enter username and password\n");
    scanf("%s",usn);
    scanf("%s",pwd);
    struct idpass ip;
    strcpy(ip.id,usn);
    strcpy(ip.pass,pwd);
    write(sockfd,&ip,sizeof(ip));
    //char buf[256];
    while(1){
    if (pthread_create(&send_thread, NULL, (void *) send_handler, &sockfd) != 0) {
        perror("no sender thread creation");
        return -1;
    }

    if (pthread_create(&recv_thread, NULL, (void *) recv_handler, &sockfd) != 0) {
        perror("no receiver thread creation");
        return -1;
    }

   
    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);
    close(sockfd);
    pthread_detach(send_thread);
    pthread_detach(recv_thread);
    break;
    }
    return 0;
}


void send_handler(int *sockfd_ptr) {;
    int sd = *sockfd_ptr;
    message* messageToSend = (message*)malloc(sizeof(message));
    char buf[MSG_SIZE],top[NAME_SIZE];
    int type;
    printf("Send message in format: To Type Message\n1:text message\n2:text file\n3:audio file\n\n");
    do{
        top[0]=0;
        buf[0]=0;
        type=0;
        scanf("%s %d %[^\n]",top,&type,buf);
        strcpy(messageToSend->from,usn);
        messageToSend->type=type;
        strcpy(messageToSend->to,top);
        switch(type){
            case 1:strcpy(messageToSend->msg,buf);
                int n=write(sd,messageToSend,sizeof(message));
                if(n<0) close(sd);
                printf("Message sent to %s\n",messageToSend->to);
                break;
            case 2:printf("hello1\n");
                filemsg *data = (filemsg *)messageToSend->msg;
                printf("hello2\n");
                strcpy(data->filename,buf);
                printf("hello3\n");
                struct stat st;
                if (stat(buf, &st) == 0) {
                    data->size= st.st_size;
                } 
                printf("hello4\n");
                if(data->size>=5000){
                    printf("Limit exceeded\n");
                    return;
                }
                printf("hello5\n");
                FILE *file = fopen(buf, "rb");
                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }
                printf("hello6\n");
                // Read the file into the buffer
                char msg[500];
                int bytesRead = fread(msg, 1, data->size, file);
                printf("hello6.1\n");
                if (bytesRead != data->size) {
                    perror("Error reading file");
                    fclose(file);
                    return;
                }
                strcpy(data->buf,msg);
                // Close the file
                fclose(file); 
                printf("Sending filename-%s from %s to %s\n",data->filename,messageToSend->from,messageToSend->to);
                write(sd,messageToSend,sizeof(message)); 
                printf("hello7\n");
                break;
        }
    }while(strcmp(buf,"Close")!=0);
    close(sd);   

    
}

void recv_handler(int *sockfd_ptr) {
    int sd = *sockfd_ptr;
    //char receivedMessage[6000];
    message* messageReceived = (message*)malloc(sizeof(message));

    while (1) {

        int n=read(sd, messageReceived, sizeof(message));
        if(n<=0){
            close(sd);
            break;
        }
        switch(messageReceived->type){
            case 1:
                if(strcmp(messageReceived->from,"Server")==0){
                    printf("%s\n",messageReceived->msg);
                }
                else{
                    printf("===Message from %s===\n%s\n",messageReceived->from,messageReceived->msg);
                }
                break;
            case 2:filemsg *data = (filemsg *)messageReceived->msg;
                char buf[50];
                strcpy(buf,data->filename);
                int size=data->size;
                 
                FILE *file = fopen(buf, "wb");
                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }

                // Read the file into the buffer
                size_t bytesRead = fwrite(data->buf, 1, data->size, file);
                if (bytesRead != data->size) {
                    perror("Error writing file");
                    fclose(file);
                    return;
                }

                // Close the file
                fclose(file); 
        } 

    }
}
// use the same code for audio