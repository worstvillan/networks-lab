#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>


#define backlog 1
#define MAXDATASIZE 100

int main(void){

    int sockfd;
    struct addrinfo hints;
    struct addrinfo *res,*p;
     struct sockaddr_storage their_addr;
    socklen_t addr_len;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
    hints.ai_flags=AI_PASSIVE;

    int status;

    if((status=getaddrinfo("127.0.0.3","3492",&hints,&res))!=0){
        fprintf(stderr ,"getadderinfo: %s\n", gai_strerror(status));
        return 1;
    }

    int yes=1;

    for(p=res;p!=NULL;p=p->ai_next){
        if((sockfd=socket(p->ai_family,p->ai_socktype,p->ai_protocol))==-1){
            perror("server : socket");
            continue;
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

    int sockfd2;
    struct addrinfo hints2;
    struct addrinfo *res2,*p2;
     struct sockaddr_storage their_addr2;
    socklen_t addr_len2;

    memset(&hints2,0,sizeof hints2);
    hints2.ai_family=AF_UNSPEC;
    hints2.ai_socktype=SOCK_DGRAM;
    hints2.ai_flags=AI_PASSIVE;

    int status2;

    if((status2=getaddrinfo("127.0.0.4","3493",&hints2,&res2))!=0){
        fprintf(stderr ,"getadderinfo: %s\n", gai_strerror(status));
        return 1;
    }

    int yes2=1;

    for(p2=res2;p2!=NULL;p2=p2->ai_next){

        if((sockfd=socket(p2->ai_family,p2->ai_socktype,p2->ai_protocol))==-1){
            perror("server : socket");
            continue;
        }

        break;
    }

    freeaddrinfo(res2);

    if(p2==NULL){
        fprintf(stderr,"server : failed to bind\n");
        exit(1);
    }

    char buff[MAXDATASIZE];
    int numbytes,size=0;

    memset(&buff,0,sizeof(buff));

    addr_len=sizeof (their_addr);

    if((numbytes=recvfrom(sockfd,buff,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr,&addr_len))==-1){
        perror("recvfrom");
        exit(1);
    }

    buff[numbytes]='\0';

    if((numbytes=sendto(sockfd2,buff,strlen(buff),0,p2->ai_addr,p2->addr_len))==-1){
        perror("send");
        exit(1);
    }

    memset(buff,0,sizeof(buff));

    if((numbytes=recvfrom(sockfd2,buff,strlen(buff),0,p2->ai_addr,p2->addr_len))==-1){
        perror("send");
        exit(1);
    }

    buff[numbytes]='\0';
    
    close(sockfd2);

    if((numbytes=sendto(sockfd,buff,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr,&addr_len))==-1){
        perror("recvfrom");
        exit(1);
    }

    close(sockfd);

    return 0;
}