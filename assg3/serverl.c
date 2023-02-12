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

int csize=0;
#define MAXDATASIZE 100

struct ip{
    	char name[100];
    	char ip_addr[100];
}cache[2];
    	

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

    if((status=getaddrinfo("127.0.0.1","3490",&hints,&res))!=0){
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

    if((status2=getaddrinfo("127.0.0.2","3491",&hints2,&res2))!=0){
        fprintf(stderr ,"getadderinfo: %s\n", gai_strerror(status));
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
    
    char chac[MAXDATASIZE];
   
    
    while(1){
    

	    memset(&chac,0,sizeof(chac));
	    char buff[MAXDATASIZE];
	    int numbytes,size=0;

	    memset(&buff,0,sizeof(buff));

	    addr_len=sizeof (their_addr);

	    if((numbytes=recvfrom(sockfd,buff,MAXDATASIZE-1,0,(struct sockaddr *)&their_addr,&addr_len))==-1){
		perror("recvfrom");
		exit(1);
	    }

	    buff[numbytes]='\0';
	    
	    
	    
	    int g=0;
	    for(int i=0;i<2;i++){
	    	if(strcmp(cache[i].name,buff)==0){
	    		printf("record found in cache\n");
	    		printf("ip address of the %s : %s\n",buff,cache[i].ip_addr);
	    		strcpy(buff,cache[i].ip_addr);
	    		if((numbytes=sendto(sockfd,buff,strlen(buff),0,(struct sockaddr *)&their_addr,addr_len))==-1){
				perror("recvfrom");
				exit(1);
			}
			g=1;
	 	}
	    }
	    if(g==1){
	    	continue;
	    }
	    
	    printf("record not found in cache\n");
	    
	    printf("ip address of the %s:",buff);
	    
	    strcpy(chac,buff);

	    if((numbytes=sendto(sockfd2,buff,strlen(buff),0,p2->ai_addr,p2->ai_addrlen))==-1){
		perror("send");
		exit(1);
	    }

	    memset(buff,0,sizeof(buff));

	    if((numbytes=recvfrom(sockfd2,buff,MAXDATASIZE-1,0,p2->ai_addr,&p2->ai_addrlen))==-1){
		perror("recvfrom");
		exit(1);
	    }
	    
	    printf(" %s\n",buff);
	    buff[numbytes]='\0';
	    
	    if(strcmp(buff,"address could not be located")!=0){
	    	
	    	strcpy(cache[csize].name,chac);
	    	strcpy(cache[csize].ip_addr,buff);
	    	csize++;
	    	
	    }
	    
	    if((numbytes=sendto(sockfd,buff,strlen(buff),0,(struct sockaddr *)&their_addr,addr_len))==-1){
		perror("sendto");
		exit(1);
	    }
	    
	    memset(&buff,0,sizeof(buff));
	    
	    if(csize==2)csize=0;
    
    
    }

    close(sockfd2);

 
    close(sockfd);

    return 0;
}
