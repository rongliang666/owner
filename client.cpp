#include <iostream>
#include <strings.h>
#include <stdio.h>
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h>
#include <arpa/inet.h>  


#define  PORT 20000
 #define  MAXDATASIZE 100
int main(void)
{
    int  sockfd, num;  
    char  buf[MAXDATASIZE];  
    struct hostent *he;  
    struct sockaddr_in server;  
    if((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1){  
    printf("socket()error\n");  
    return 0;
    }

    bzero(&server,sizeof(server));  
    server.sin_family= AF_INET;  
    server.sin_port = htons(PORT);  
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))==-1){ 
    printf("connect()error\n");  
    return 0;
    }

    char str[100] = "hello world\n";
    if (send(sockfd, str, sizeof(str), 0) < 0)
    {
        std::cout <<"send failed\n";
    }
    close(sockfd);
}
