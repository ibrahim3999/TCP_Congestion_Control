#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#define PORT 9999
#define SIZE 1024

void sendFile(FILE *fp,int senderSocket)
{
  int n;
  char data[SIZE] = {0};
 
  while(fgets(data, SIZE, fp) != NULL) {
    if (send( senderSocket,data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, SIZE);
  }
}

char *ReaderFile(int i)
{

    FILE *ptr;
    char ch;
    char *res;
    int N=1,j=0;
    int to;
    char *fileName="file.txt";
    char *head;
    int len;

    res=(char *) malloc(1);
    ptr=fopen(fileName,"r");
    fseek(ptr,0,SEEK_END);
    len=ftell(ptr);
    fclose(ptr);
    ptr=fopen(fileName,"r");
    if(ptr==NULL)
        perror("file can not be opened");
    
    if(i==0)len/=2;
    
    while ((ch=fgetc(ptr))!=EOF && i<len)
    {
        
        if(i+1>N && i>0)
        {
            res=(char*) realloc(res,strlen(res)*2);
            N=strlen(res)*2;
        }
        *(res+i)=ch;
        i++;
    }
    fclose(ptr);
    return res;

}
void main()
{   char ch;
    int senderSocket,a;
    char *data;
    struct sockaddr_in receivaddr,cli;
    FILE *fp;
    char *filename="file.txt";
    //data=ReaderFile(0);//first part 0 is first part
    
    senderSocket=socket(AF_INET,SOCK_STREAM,0);
    if(senderSocket ==-1)
    {
        perror("socket creation failed !!!\n");
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
            perror("connection with the receiver failed ...!!!\n");
            exit(1);
       }
    printf("connected to the receiver .. \n");

    fp= fopen(filename,"r");
    if(fp == NULL)
    {
        perror(" Ettor in reading file.");
        exit(1);
    }

    sendFile(fp,senderSocket);
    printf(" File data sent succesfully!!!\n");

    printf(" Closing the connection \n");    
    close(senderSocket);
    
}
