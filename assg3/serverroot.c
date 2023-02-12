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
#define MAX_IP_ADDR 20

int count=0;

struct addr{
    char tld[100];
    char ip_addr[100];
}x[MAX_IP_ADDR];

void add(char *tld,char *ip_addr){

    if(count>=MAX_IP_ADDR){
        printf("error : tld address reach is exceeded");
        return;
    }

    strcpy(x[count].tld,tld);
    strcpy(x[count].ip_addr,ip_addr);
    count++;
}

char* lookup(char *name){
    for(int i=0;i<MAX_IP_ADDR;i++){
        if(strcmp(x[i].tld,name)==0){
            return x[i].ip_addr;
        }
    }

    return NULL;
}

int main(void){

    add(".com","127.0.0.3");

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

    if((status=getaddrinfo("127.0.0.2","3491",&hints,&res))!=0){
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

    memset(&hints2,0,sizeof hints2);
    hints2.ai_family=AF_UNSPEC;
    hints2.ai_socktype=SOCK_DGRAM;
    hints2.ai_flags=AI_PASSIVE;

    int status2;

    if((status2=getaddrinfo("127.0.0.3","3492",&hints2,&res2))!=0){
        fprintf(stderr ,"getadderinfo: %s\n", gai_strerror(status2));
        return 1;
    }

    int yes2=1;

    for(p2=res2;p2!=NULL;p2=p2->ai_next){

        if((sockfd2=socket(p2->ai_family,p2->ai_socktype,p2->ai_protocol))==-1){
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
    
    while(1){

    char buff[MAXDATASIZE];
    int numbytes,size=0;

    memset(&buff,0,sizeof(buff));

    addr_len=sizeof (their_addr);

    if((numbytes=recvfrom(sockfd,buff,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr,&addr_len))==-1){
        perror("recvfrom");
        exit(1);
    }

    buff[numbytes]='\0';
    
    

    char tld1[5];
    tld1[0]='.';
    tld1[1]=buff[numbytes-3];
    tld1[2]=buff[numbytes-2];
    tld1[3]=buff[numbytes-1];
    tld1[4]='\0';

    char* ip_addr=lookup(tld1);

    if(ip_addr!=NULL){

        printf("tld server ip address for .com : %s\n",ip_addr);
        
        printf("ip address for %s :",buff);

        if((numbytes=sendto(sockfd2,buff,strlen(buff),0,p2->ai_addr,p2->ai_addrlen))==-1){
            perror("send");
            exit(1);
        }

        memset(buff,0,sizeof(buff));

        if((numbytes=recvfrom(sockfd2,buff,MAXDATASIZE-1,0,p2->ai_addr,&p2->ai_addrlen))==-1){
            perror("recvfrom");
            exit(1);
        }

	
        buff[numbytes]='\0';
	printf(" %s\n",buff);

        if((numbytes=sendto(sockfd,buff,strlen(buff),0,(struct sockaddr *)&their_addr,addr_len))==-1){
        	perror("recvfrom");
        	exit(1);
    	}
        
        
        
    }
    else{
        printf("error in finding tld\n");
    }
    
    
    }
    
    close(sockfd2);
    close(sockfd);
    return 0;
}
