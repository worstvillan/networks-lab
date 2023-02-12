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

#define MAXDATASIZE 100

int main(int argc,char *argv[]){
    int sockfd;
    struct addrinfo hints;
    struct addrinfo *res,*p;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;

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
        
        break;
    }

    if(p==NULL){
        fprintf(stderr, "client:failed to connect\n");
        return 2;
    }

    freeaddrinfo(res);
    
    while(1)
{
    char buff[MAXDATASIZE];
    int numbytes,size=0;

    struct sockaddr_storage their_addr;
    socklen_t sin_size;

    memset(&buff,0,sizeof(buff));

    while((buff[size++]=getchar())!='\n');
    buff[size-1]='\0';


    if((numbytes=sendto(sockfd,buff,size,0,p->ai_addr,p->ai_addrlen))==-1){
        perror("sendto");
        exit(1);
    }
    
    memset(&buff,0,sizeof(buff));

    if((numbytes=recvfrom(sockfd,buff,MAXDATASIZE,0,p->ai_addr,&p->ai_addrlen))==-1){
        perror("recv");
        exit(1);
    }

    buff[numbytes] = '\0';

    printf("%s\n",buff);
    
    memset(&buff,0,sizeof(buff));
}

    close(sockfd);

    return 0;
}
