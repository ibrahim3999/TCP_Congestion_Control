#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 9999
void main()
{
    int sockrec ,connrec,len;
    struct sockaddr_in recaddr,cli;
    
    sockrec=socket(AF_INET,SOCK_STREAM,0);
    if(sockrec==-1)
    {
        printf("socket creation failed ..\n");
    }
    else{
        printf("Socket Successfully created ..\n");
    }

    bzero(&recaddr,sizeof(recaddr));

    recaddr.sin_family=AF_INET;
    recaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    recaddr.sin_port=htons(PORT);
    if((bind(sockrec,(struct sockaddr*)&recaddr,sizeof(recaddr)))!=0)
    {
        printf("socket bind failed ... !!!\n");
        exit(0);
    }
    else{
        printf("Sokect successfully binded ...!!!\n");
    }
    if((listen (sockrec,5))!=0)
    {
        printf("Listen failed ...\n");
        exit(0);
    }
    else
    printf("Receiver listening ... \n");

    len=sizeof(cli);

    connrec=accept(sockrec,(struct sockaddr*)&cli,&len);
    if(connrec<0)
    {
        printf("Receiver accept failed ...\n");
        exit(0);
    }
    printf("Receiver accept the Sender ... \n ");   
}