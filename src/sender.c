#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 9999

void ReaderFile()
{

    FILE *ptr;
    char ch;
    ptr=fopen("file.txt","r");

    if(ptr==NULL)
        printf("file can not be opened");

    while (ch!=EOF)
    {
        ch=fgetc(ptr);
        printf("%c\n",ch);
    }
    fclose(ptr);
}
void main()
{
    int senderSocket;
    struct sockaddr_in receivaddr,cli;
    ReaderFile();

    senderSocket=socket(AF_INET,SOCK_STREAM,0);
    if(senderSocket ==-1)
    {
        printf("socket creation failed !!!\n");
        exit(0);
    }
    else{
        printf("socket succesfuly created!! \n ");
    }
    bzero(&receivaddr,sizeof(receivaddr));

    receivaddr.sin_family=AF_INET;
    receivaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    receivaddr.sin_port=htons(PORT);
    
    if(connect (senderSocket,(struct sockaddr*)&receivaddr,sizeof(receivaddr))!=0)
    {
        printf("connection with the receiver failed ...!!!\n");

    }
    else
    {
        printf("connected to the receiver .. \n");
    }

    close(senderSocket);
}
