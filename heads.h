#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <zlib.h>
#include <pthread.h>
#define MAX 10

typedef struct{
    char username[100];
    char password[100];
}user;

typedef struct{
    int sockfd;
    struct sockaddr_in addr;
    char name[50];
}client;

// typedef enum{
//     text,
//     document,
//     audio
// }type;

// typedef struct{
//     //type t;
//     client from;
//     client to;
//     char msg[256];
// }message;
typedef struct{
    int type;
    char from[50];
    char to[50];
    char msg[150];
}message;

client *clients[MAX];
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
user *users[MAX];


