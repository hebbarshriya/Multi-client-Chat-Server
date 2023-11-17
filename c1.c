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
    int sockfd, reg;
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

    printf("Login(0) or Register(1): ");
    scanf("%d",&reg);
    write(sockfd,&reg,sizeof(int));
    if(reg==1){
        printf("Enter registration credentials\nUsername: ");
        scanf("%s",usn);
        printf("Enter password: ");
        scanf("%s",pwd);
        printf("\n");
        struct idpass ip;
        strcpy(ip.id,usn);
        strcpy(ip.pass,pwd);
        write(sockfd,&ip,sizeof(ip));
        printf("---------REGISTRATION SUCCESSFUL!!! LOGIN AGAIN---------\n");
        close(sockfd);      
        
        exit(EXIT_SUCCESS);

    }

    //Login
    printf("Enter username: ");
    scanf("%s",usn);
    printf("Enter password: ");
    scanf("%s",pwd);
    printf("\n");
    struct idpass ip;
    strcpy(ip.id,usn);
    strcpy(ip.pass,pwd);
    write(sockfd,&ip,sizeof(ip));
    while(1){
    if (pthread_create(&send_thread, NULL, (void *) send_handler, &sockfd) != 0) {
        perror("No sender thread creation");
        return -1;
    }

    if (pthread_create(&recv_thread, NULL, (void *) recv_handler, &sockfd) != 0) {
        perror("No receiver thread creation");
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
    filemsg *data = (filemsg *)messageToSend->msg;
    char buf[MSG_SIZE],top[NAME_SIZE];
    int type;
    struct stat st;
    FILE *file;
    int bytesRead;
    printf("Send message in format: Reciever_Name (Type) Message\n\n\n1:TEXT MESSAGE\n2:TEXT FILE\n3:AUDIO/VIDEO FILE\n\n");
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
            case 2:  
                strcpy(data->filename,buf);
                
                if (stat(buf, &st) == 0) {
                    data->size= st.st_size;
                } 
                if(data->size>=MSG_SIZE){
                    printf("Limit exceeded\n");
                    return;
                }
                file = fopen(buf, "rb");
                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }
                bytesRead = fread(&(data->buf), 1, data->size, file);
                if (bytesRead != data->size) {
                    perror("Error reading file");
                    fclose(file);
                    return;
                }
                // Close the file
                fclose(file); 
                printf("Sending filename-%s from %s to %s\n",data->filename,messageToSend->from,messageToSend->to);
                write(sd,messageToSend,sizeof(message)); 
                //if(n<0) printf("Error while transfer\n");
                break;
            case 3:  
                strcpy(data->filename,buf);
               
                if (stat(buf, &st) == 0) {
                    data->size= st.st_size;
                } 
                if(data->size>=MSG_SIZE){
                    printf("Limit exceeded\n");
                    return;
                }
                file = fopen(buf, "rb");
                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }
                bytesRead = fread(&(data->buf), 1, data->size, file);
                if (bytesRead != data->size) {
                    perror("Error reading file");
                    fclose(file);
                    return;
                }
                // Close the file
                fclose(file); 
                printf("Sending audio filename-%s from %s to %s\n",data->filename,messageToSend->from,messageToSend->to);
                write(sd,messageToSend,sizeof(message)); 
                break;
            
        }
    }while(strcmp(buf,"Close")!=0);
    close(sd);   

    
}

void recv_handler(int *sockfd_ptr) {
    int sd = *sockfd_ptr;
    message* messageReceived = (message*)malloc(sizeof(message));
    filemsg *data = (filemsg *)messageReceived->msg;
    struct stat st;
    char *dirname = "shared_folder";
    char path[300];
    FILE *file;
    size_t bytesRead;
    int size,result;
    char command[310];

    while (1) {

        int n=recv(sd, messageReceived, sizeof(message),MSG_WAITALL);
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
                    printf("%s : %s\n",messageReceived->from,messageReceived->msg);
                }
                break;
            case 2:
                size=data->size;
                
                // Create the directory
                
                if (stat(dirname, &st) != 0) {
                // Directory does not exist, create it
                    if (mkdir(dirname, 0777) == -1) {
                        perror("Error creating directory");
                        return;
                    }
                }

                // Open or create the file within the directory
                
                snprintf(path, sizeof(path), "%s/%s_%s", dirname,messageReceived->from, data->filename);

                file = fopen(path, "wb");
                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }

                // Read the file into the buffer
                bytesRead = fwrite(&(data->buf), 1, data->size, file);
                if (bytesRead != data->size) {
                    perror("Error writing file");
                    fclose(file);
                    return;
                }
                // Close the file
                fclose(file); 
                printf("%s : %s (file saved in %s directory)\n",messageReceived->from,data->filename,dirname);
                break;

            case 3:
                size=data->size;
                // Create the directory
                
                if (stat(dirname, &st) != 0) {
                // Directory does not exist, create it
                    if (mkdir(dirname, 0777) == -1) {
                        perror("Error creating directory");
                        return;
                    }
                }

                // Open or create the file within the directory
                snprintf(path, sizeof(path), "%s/%s_%s", dirname,messageReceived->from, data->filename);

                file = fopen(path, "wb");
                if (file == NULL) {
                    perror("Error opening file");
                    return;
                }

                // Read the file into the buffer
                bytesRead = fwrite(&(data->buf), 1, data->size, file);
                if (bytesRead != data->size) {
                    perror("Error writing file");
                    fclose(file);
                    return;
                }
                // Close the file
                fclose(file); 
                
                snprintf(command, sizeof(command), "vlc %s", path);

                // Run the command using system
                result = system(command);

                // Check the result of the command
                if (result == -1) {
                    perror("Error running command. Audio/Video file saved\n");
                    return;
                } 
                else {
                    printf("Audio/Video played successfully\n");
                }
                break;
                
        } 

    }
}

