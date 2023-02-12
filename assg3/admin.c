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
#define MAX_IP_ADDR 10

int count=0;

struct addr{
    char name[100];
    char ip_addr[100];
}x[MAX_IP_ADDR];

void add(char *name,char *ip_addr){

    if(count>MAX_IP_ADDR){
        printf("error : admin address reach is exceeded");
        return;
    }

    strcpy(x[count].name,name);
    strcpy(x[count].ip_addr,ip_addr);
    count++;
}


int main(void){

    add("www.google.com","216.58.194.174");
    add("www.cloudflare.com","104.16.248.247");
    add("www.facebook.com","104.24.103.15");
    add("www.github.com","185.199.108.153");
    add("www.amazon.com","13.250.177.223");
    add("www.rohit.com","69.10.208.246");
    add("www.kohli.com","50.56.70.162");
    add("www.dhoni.com","107.180.50.234");
    add("www.levi.com","198.11.176.250");
    add("www.doflamingo.com","45.79.41.19");

    
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

    if((status=getaddrinfo("127.0.0.4","3493",&hints,&res))!=0){
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

	    printf("received request from tld server for ip = %s : ",buff);


	    char buff1[MAXDATASIZE];
	    memset(&buff1,0,sizeof(buff1));
	    
	    strcat(buff1,buff);

	    char ip_address[100];
	    int flag=0;
	    
	    for(int i=0;i<MAX_IP_ADDR;i++){
	    
		if(strcmp(x[i].name,buff1)==0){
		
		    
		    strcpy(ip_address,x[i].ip_addr);
		    flag=1;
		    break;
		    
		}
		
	    }
	    
	    printf("ip address is %s\n",ip_address);

	    if(flag==1){
		if((numbytes = sendto(sockfd, ip_address, strlen(ip_address), 0, (struct sockaddr*)&their_addr, addr_len))==-1){
		    perror("error in sending to tld server\n");
		    exit(1);
		}
	    }
	    else{
		memset(buff1,0,sizeof(buff1));
		strcat(buff1,"address could not be located");
		if((numbytes = sendto(sockfd, buff1, strlen(buff1), 0, (struct sockaddr*)&their_addr, addr_len))==-1){
		    perror("error in sending to tld server\n");
		    exit(1);
		}
	    }
    
    }

    close(sockfd);

    return 0;
}
