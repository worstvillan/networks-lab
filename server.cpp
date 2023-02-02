#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>
#include <gmp.h>


#define PORT 2035

int main(){

    int server_socket,addrlen,new_socket,client_socket[30],max_clients=30,valread,sd,val;
    int max_sd;

    struct sockaddr_in addr;

    char buff[1024];

    fd_set readfd;

    for(int i=0;i<30;i++){
        client_socket[i]=0;
    }

    if((master_socket = socket(AF_NET,SOCK_STREAM,0))==0){
        perror("socket_failed");
        exit(EXIT_FAILURE);
    }

    if(setsockopt(master_socket,SOL_SOCKET,SO_REUSEADDR,(char*)&opt,sizeof(opt))<0){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    addr.sin_family=AF_NET;
    addr.sin_addr.s_addr=INADDR_ANY;
    addr.sin_port=htons(PORT);

    if(bind(master_socket,(struct sockaddr*)&addr,sizeof(addr))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(master_socket,10)<0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    addrlen=sizeof(addr);

    while(1){

        FD_ZERO(&readfd);

        FD_SET(master_socket,&readfd);
        max_sd=master_socket+1;

        for(int i=0;i<30;i++){
            sd=client_socket[i];

            if(sd>0){
                FD_SET(sd,&readfd);
            }

            if(sd>max_sd){
                max_sd=sd;
            }

        }

        val=select(max_sd,&readfs,NULL,NULL,NULL);

        if((val<0) && (errno!=EINTR)){
            printf("select error");
        }

        if(FD_ISSET(master_socket,&readfd)){

            if((new_socket=accept(master_socket,(struct sockaddr*)&addr,(socklen_t*)&addrlen))<0){
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //i got the new connection here

            for(int i=0;i<max_clients;i++){

                if(client_socket[i]==0){
                    
                    client_socket[i]=new_socket;
                    
                    break;

                }

            }

        }

        for(int i=0;i<30;i++){

            sd=client_socket[i];

            if(FD_ISSET(sd,&readfd)){

                valread = recv(sd,buff,1024);
                buffer[valread]='/0';

                if(strcmp(buffer,"/exit")==0) || (strcmp(buffer,"/quit")==0) || (strcmp(buffer,"/part")==0){

                    close(sd);
                    client_socket[i]=0;

                }
                else{

                    for(int j=0;j<30;j++){

                        if(client_socket[j]!=0){

                            send(client_socket[j],buffer,strlen(buffer),0);

                        }

                    }

                }
            }
        }

    }

    return 0;
}