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

    printf("listener waiting to recvfrom...\n");

    char buff[MAXDATASIZE];
    int numbytes,size=0;

    memset(&buff,0,sizeof(buff));

    struct sockaddr_storage their_addr;
    socklen_t sin_size;

    addr_len=sizeof their_addr;

    if((numbytes=recvform(sockfd,buff,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr,&addr_len))==-1){
        perror("recvfrom");
        exit(1);
    }

    printf("listener : got packet from %s\n",buff);

    buff=reverse(buff);

    if((numbytes=sendto(sockfd,buff,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr,&addr_len))==-1){
        perror("send");
        exit(1);
    }

    close(sockfd);

    return 0;
}
