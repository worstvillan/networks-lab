#pragma comment(lib, "Ws2_32.lib")
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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
int betriebssystem = 2;
#endif


#define MAXDATASIZE 100

int main(int argc,char *argv[]){
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *res,*p;



    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;

    int status;

    if((status=getaddrinfo("127.0.0.1","3490",&hints,&res))!=0){
        fprintf(stderr,"getaddrinfo:%s\n",gai_strerror(status));
        return 1;
    }

    for(p=res;p!=NULL;p=p->ai_next){
        if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            perror("client : socket");
            continue;
        }

        if(connect(sockfd,p->ai_addr,p->ai_addrlen)==-1){
            close(sockfd);
            perror("client : connect");
            continue;
        }
        
        break;
    }

    if(p==NULL){
        fprintf(stderr, "client:failed to connect\n");
        return 2;
    }

    freeaddrinfo(res);

    char buff[MAXDATASIZE];
    int numbytes,size=0;

    memset(&buff,0,sizeof(buff));

    printf("input the string from client");

    while((buff[size++]=getchar())!='\n');
    buff[size-1]='\0';


    if((numbytes=send(sockfd,buff,MAXDATASIZE,0))==-1){
        perror("send");
        exit(1);
    }
    
    memset(&buff,0,sizeof(buff));

    if((numbytes=recv(sockfd,buff,MAXDATASIZE,0))==-1){
        perror("recv");
        exit(1);
    }

    buff[numbytes] = '\0';

    printf("client : received %s\n",buff);

    close(sockfd);

    return 0;
}
