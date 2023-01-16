#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#ifdef _WIN32
//For Windows
int betriebssystem = 1;
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
//#include <ws2def.h>
#pragma comment(lib, "Ws2_32.lib")
#include <windows.h>
#include <io.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int betriebssystem = 2;
#endif


#define port "3490"
#define backlog 1
#define MAXDATASIZE 100

char* reverse(char *s){
    char buff[MAXDATASIZE];
    int i=0;
    while(s[i]!='\0')i++;
    for(int j=i-1;j>=0;j--){
        buff[j+1]=s[i];
    }
    buff[0]='\0';
    return buff;
}

int main(void){
    int sockfd,newfd;
    struct addrinfo hints;
    struct addrinfo *res,*p;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;

    int status;

    if((status=getaddrinfo(NULL,"3490",&hints,&res))!=0){
        fprintf(stderr ,"getadderinfo: %s\n", gai_strerror(status));
        return 1;
    }

    int yes=1;

    for(p=res;p!=NULL;p=p->ai_next){
        if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            perror("server : socket");
            continue;
        }

        if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int))==-1){
            perror("setsockopt");
            exit(1);
        }

        if(bind(sockfd,p->ai_addr,p->ai_addrlen)==-1){
            close(sockfd);
            perror("server :blind");
            continue;
        }

        break;
    }

    freeaddrinfo(res);

    if(p==NULL){
        fprintf(stderr,"server : failed to bind\n");
        exit(1);
    }

    printf("listening\n");

    if(listen(sockfd,backlog)==-1){
        perror("listen");
        exit(1);
    }

    printf("waiting for conneections\n");

    struct sockaddr_storage their_addr;
    socklen_t sin_size;

    newfd=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);

    if(newfd==-1){
        perror("accept");
        return 0;
    }

    char buff[MAXDATASIZE];
    int numbytes,size=0;

    memset(&buff,0,sizeof(buff));

    if((numbytes=recv(newfd,buff,MAXDATASIZE,0))==-1){
        perror("recv");
        exit(1);
    }


    printf("server :received %s\n",buff);

    buff=reverse(buff);

    if((numbytes=send(newfd,buff,MAXDATASIZE,0))==-1){
        perror("send");
        exit(1);
    }

    close(sockfd);

    return 0;
}
