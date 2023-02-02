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
#define MAXDATASIZE 100

void reverse(char *s){
    int i=0,j=0;
    while(s[i]!='\0')i++;
    i--;
    while(j<i){
    	char c=s[i];
    	s[i]=s[j];
    	s[j]=c;
    	j++,i--;
    }
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
   
   while(1){
   
   if(listen(sockfd,10)==-1){
	perror("listen");
	exit(1);
   }
   
   
    
    
	    
	    
	    struct sockaddr_storage their_addr;
		    socklen_t sin_size;

		    newfd=accept(sockfd,(struct sockaddr *)&their_addr,&sin_size);

		    if(newfd==-1){
			perror("accept");
			return 0;
		    }

		    
		    
		while(1){    
	    
	    
	    
	    		char buff[MAXDATASIZE];
			    int numbytes,size=0;

		    
			memset(&buff,0,sizeof(buff));

		    if((numbytes=recv(newfd,buff,MAXDATASIZE,0))==-1){
			perror("recv");
			exit(1);
		    }

			if(strcmp(buff,"exit")==0)break;
		    reverse(buff);
		    
			int len=strlen(buff);

		    if((numbytes=send(newfd,buff,len,0))==-1){
			perror("send");
			exit(1);
		    }

		    
		    
	}
		
		close(newfd);
	
}
    return 0;
}
