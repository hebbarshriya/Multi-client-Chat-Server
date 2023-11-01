#include "heads.h"
#define PORT 13055

void send_handler(int *);
void recv_handler(int *);
struct idpass{
    char id[50];
    char pass[50];
}idpass;


char usn[50], pwd[50];

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
        //int a1;
        //read(sockfd,&a1,sizeof(int));
        /*if(a1==0){
            printf("Username already exists\n");
            close(sockfd);
        }*/
        //else{
            printf("Restart and login\n");
            close(sockfd);
        //}
        
        
        exit(EXIT_SUCCESS);

    }

    //Login
    printf("Enter username and password\n");
    scanf("%s",usn);
    scanf("%s",pwd);
    struct idpass ip;
    strcpy(ip.id,usn);
    strcpy(ip.pass,pwd);
    //int a;
    write(sockfd,&ip,sizeof(ip));
    //read(sockfd,&a,sizeof(int));
    // if(a==0){
    //     printf("Not authorised user\n");
    //     close(sockfd);
    // }
    //else{
    char buf[256];
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
    char buf[256],top[50];
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
        strcpy(messageToSend->msg,buf);
        printf("%s : %s : %s\n",messageToSend->from,messageToSend->to,messageToSend->msg);
        write(sd,messageToSend,sizeof(message));
    }while(strcmp(buf,"Close")!=0);
    printf("ballo\n");
    close(sd);
    pthread_exit(NULL);      

    
}

void recv_handler(int *sockfd_ptr) {
    int sd = *sockfd_ptr;
    char receivedMessage[256];

    while (1) {
        receivedMessage[0]=0;
        int n=read(sd, receivedMessage, sizeof(receivedMessage));
        if(n<0){
            close(sd);
            break;
        }
        if(receivedMessage[0]!=0)
        printf("received msg: %s\n",receivedMessage);      

    }
}
// use the same code for audio
