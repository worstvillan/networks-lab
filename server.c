#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define PORT "2035"

void* get_in_addr(struct sockaddr *sa){
    if(sa->sa_family==AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(){

    fd_set master;
    fd_set read_fds;
    int fd_max;

    int listener;
    int new_fd;
    struct sockaddr_storage remoteaddr;
    socklen_t addr_len;

    char buf[256];
    int nbytes;

    char remote[INET6_ADDRSTRLEN];

    int yes=1;
    int i,j,rv;


    struct addrinfo hints,*ai,*p;

    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    memset(&hints,0,sizeof(hints));
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;

    if((rv=getaddrinfo(NULL,PORT,&hints,&ai))!=0){

        fprintf(stderr,"selectserver: %s\n",gai_strerror(rv));
        exit(1);

    }

    for(p=ai;p!=NULL;p=p->ai_next){
    
        listener=socket(p->ai_family,p->ai_socktype,p->ai_protocol);
        if(listener<0)continue;

        setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));

        if(bind(listener,p->ai_addr,p->ai_addrlen)<0){
            close(listener);
            continue;
        }

        break;
    }

    if(p==NULL){

        fprintf(stderr,"selectserver :failed to bind\n");
        exit(3);

    }

    freeaddrinfo(ai);

    if(listen(listener,10)==-1){
        perror("listen");
        exit(3);
    }

    FD_SET(listener,&master);

    fd_max=listener;

    while(1){
        read_fds=master;

        if(select(fd_max,&read_fds,NULL,NULL,NULL)==-1){
            perror("select");
            exit(4);
        }

        for(i=0;i<=fd_max;i++){
            if(FD_ISSET(i,&read_fds)){
                if(i==listener){
                    addr_len=sizeof(remoteaddr);
                    new_fd=accept(listener,(struct sockaddr*)&remoteaddr,&addr_len);

                    if(new_fd==-1){
                        perror("accept");
                    }
                    else{
                        FD_SET(new_fd,&master);

                        if(new_fd>fd_max){
                            fd_max=new_fd;
                        }

                        printf("newconnection from %s on socket %d\n",inet_ntop(remoteaddr.ss_family,
                            get_in_addr((struct sockaddr*)&remoteaddr),remote,INET6_ADDRSTRLEN),new_fd);

                        
                    }
                }

                else{

                    if((nbytes =recv(i,buf,sizeof(buf),0))<=0){

                        if(nbytes==0){

                            printf("select server :socket %d hung up\n",i);

                        }
                        else{

                            perror("recv");

                        }

                        close(i);
                        FD_CLR(i,&master);

                    }
                    else{

                        for(j=0;j<=fd_max;j++){

                            if(FD_ISSET(j,&master)){

                                if(j!=listener && j!=i){

                                    if(send(j,buf,nbytes,0)==-1){
                                        perror("send");
                                    }

                                }

                            }

                        }
                    }
                }
            }
        }
    }
    return 0;
}
