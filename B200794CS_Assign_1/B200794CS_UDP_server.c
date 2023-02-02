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

void  reverse(char *s){
    char buff[MAXDATASIZE];
    int j=strlen(s)-1;
    int k=0;
    for(j;j>=0;j--){
        buff[j]=s[k];
        k++;
    }
    buff[strlen(s)]='\0';
    strcpy(s,buff);
}

int main(void){
    int sockfd,newfd;
    struct addrinfo hints;
    struct addrinfo *res,*p;
     struct sockaddr_storage their_addr;
    socklen_t addr_len;

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_DGRAM;
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

    char buff[MAXDATASIZE];
    int numbytes,size=0;

    memset(&buff,0,sizeof(buff));

    addr_len=sizeof (their_addr);

    if((numbytes=recvfrom(sockfd,buff,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr,&addr_len))==-1){
        perror("recvfrom");
        exit(1);
    }

    reverse(buff);

    if((numbytes=sendto(sockfd,buff,strlen(buff),0,(struct sockaddr *)&their_addr,addr_len))==-1){
        perror("send");
        exit(1);
    }

    close(sockfd);

    return 0;
}
