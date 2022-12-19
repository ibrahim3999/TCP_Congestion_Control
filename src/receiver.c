#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

//Defining the important values


#define SERVER_PORT 5566
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SIZE 1001704
#define HALF_SIZE 500852
#define FIlE_NAME "f2.txt"
#define XOR "0001 1010 1101 1010"
#define MICROSEC  100000000.0
int setSock();



int main() {


    
    char buffer[HALF_SIZE];
    char buffreExit[1];
    char buffer_auth[20];
    struct timeval start, end;
    int counter = 0;
    int counter1=0;
    int counter2=0;
    int sum=1;
    int count_us = 0;
    int checkPart = 0;
    int got_bytes=0;
    int ra=1;//repeat again
    int size2=0;
    double cubic_sum=0;
    double reno_sum=0;
    char namealgo[250];
    socklen_t length;
    struct sockaddr_in client_addr;
    struct sockaddr_in server_addr;
    socklen_t client_addr_length = sizeof(client_addr);
    signal(SIGPIPE, SIG_IGN);
    
    
    //for allocate memory UD and calc times
    double sum_average_cubic = 0.0;
    double sum_average_reno = 0.0;
    double *part1 = NULL;
    double *part2 = NULL;
    part1 = (double *) malloc(1);
    part2 = (double *) malloc(1);
    part1 = (double *) realloc(part1, sizeof(double) * (counter + 2));
    part2 = (double *) realloc(part2, sizeof(double) * (counter + 2));
    char msgexit[1] ;
    msgexit[0]= '0';
    int receiver_socket = -1;

    int setSock(){

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT); 

    // create a socket lisener for sender.
    int receiver_socket = -1;
    if((receiver_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Couldn't create a socket listener : %d\n",errno);
        exit(1);
        
    }
    //default CC
    strcpy(namealgo,"cubic");
    length = sizeof(namealgo);
    //getting the information about CC algo changes
    if (getsockopt(receiver_socket,IPPROTO_TCP,TCP_CONGESTION,namealgo,&length)!=0){
        exit(1);
        
    }

    // connect the client
    
    if(bind(receiver_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Bind failed with error code : %d" , errno);
        exit(1);
    }

    printf("[+]Receiver connected to the Sender successfully!\n\n");


    if(listen(receiver_socket, 500) == -1) {
        printf("listen failed with error code : %d",errno);
        exit(1);
    }
    return receiver_socket;
}

receiver_socket = setSock(); //Creating the socket
    
    printf("[+]TCP protocol socket created -> Waiting for connection with sender\n");
    

    while(ra){
        

        int countbytes = 0;

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr_length = sizeof(client_addr);

        //The receiver accept the connection
        
        int client_socket = accept(receiver_socket, (struct sockaddr *) &client_addr, &client_addr_length);
        if (client_socket == -1) {
            printf("listen failed with error code :%d \n",errno);
            exit(1);
        } else {
            printf("[+]Sender number connection accepted\n");
        }
        

        while (1) {
            
            //starting the timer for chanks
            gettimeofday(&start, 0);
            printf("Chank- %d\n",sum++);
            got_bytes = recv(client_socket, &buffer, HALF_SIZE, 0);
            printf("1\t%d\n",got_bytes);
            countbytes += got_bytes;
            printf("2\t%d\n",countbytes);

            if (countbytes == HALF_SIZE) {
                
                
                send(client_socket,XOR, sizeof(buffer_auth),0);
                
                gettimeofday(&end, 0);
                
                if (checkPart == 0) { //To handle with  the first recv part
                    printf("\n[+]Receiver got the part 1 of file!\n");
                    printf("[+]Sending Ack. Authentication!\n\n");
                    printf("[+]Change the cc algorithm to reno\n");
                    

                    double timer = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / MICROSEC;


                    sum_average_cubic += timer;
                    *(part1 + counter1++) = timer;
                    
                    checkPart = 1;
                    size2+=countbytes;
                    countbytes=0;


                //if half size sent fully to receiver
                } else { 
                    
                    count_us++;
                    printf("[+]Receiver got the part 2 of file!\n");

                    bzero(namealgo, sizeof(namealgo));
                    
                    strcpy(namealgo, "reno");
                    length = strlen(namealgo);
                    if ((setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, namealgo, length)) != 0) {

                        printf("setsockopt_receiver\n");

                        return -1;

                    }
                    double timer = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / MICROSEC;

                    sum_average_reno += timer;
                    *(part2 + counter2++) = timer;
                    
                    checkPart = 0;
                    size2+=countbytes;
                    countbytes=0;
                }
                

             
                if (size2 == SIZE) {

                    printf("All content file is in Receiver!\n");
                    printf("\n##########################################\n");
                    
                    // To get the decision of the user
                    bzero(buffreExit, sizeof(buffreExit));
                    printf("\n\n");
                    recv(client_socket, &buffreExit, sizeof(buffreExit), 0);
                    printf("%c\n",buffreExit[0]);
                    


                    if (buffreExit[0] == msgexit[0]) {
                        
                        printf("Times By Parts:\n");
                        printf("Part1:\n");
                        for (int j = 0; j < count_us; j++) {
                            printf("%f\n", *(part1 + j));
                            cubic_sum+=*(part1+j);
                        }

                        printf("\nPart2:\n");
                        for (int j = 0; j < count_us; j++) {
                            printf("%f\n", *(part2 + j));
                            reno_sum+=*(part2+j);

                        }

                        printf("\nAverage time for Part 1 is:%f\n",cubic_sum/count_us);
                        printf("Average time for Part 2 is:%f\n",reno_sum/count_us);
                        printf("\nAverage file is:%f\n",(cubic_sum+reno_sum)/count_us);



//                  
                    ra=0;
                        

                    break;
                    }
                    else{
                        ra=1;
                        size2=0;
                    }




                }

            }


        }
        
        close(client_socket);
        

        

    }

    
    close(receiver_socket);
    return 0;
}