#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <zlib.h>
#include <pthread.h>
#define NAME_SIZE 50
#define PWD_SIZE 50
#define MSG_SIZE 5000000
#define MAX 10

typedef struct{
    char username[NAME_SIZE];
    char password[PWD_SIZE];
}user;

typedef struct{
    int sockfd;
    struct sockaddr_in addr;
    char name[NAME_SIZE];
}client;

typedef struct{
    int type;
    char from[NAME_SIZE];
    char to[NAME_SIZE];
    char msg[MSG_SIZE];
}message;

typedef struct{
    char filename[NAME_SIZE];
    int size;
    char buf;
}filemsg;

client *clients[MAX];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
user *users[MAX];


