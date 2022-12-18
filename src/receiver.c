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


#define BUFF_SIZE 64000
#define MILLION  100000000.0
#define SERVER_PORT 5566
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SIZE 1048576
#define HALF_SIZE 524288
#define AUTH_SIZE 21
#define FIlE_NAME "f2.txt"
// 313357196, 207101734

#define XOR "0001 1010 1101 1010"
#define SA struct sockaddr
#define SI struct sockaddr_in
int setSock();

void printTimes(double *timePart1, double *timePart2, int len,double sum_Cubic,double sum_Reno);

int main() {


    int size2=0;
    // on linux to prevent crash on closing socket.
    signal(SIGPIPE, SIG_IGN);
    char buffer[HALF_SIZE];
    char buffreExit[1];
    char buffer_auth[AUTH_SIZE];
    struct timeval start, end;
    int counter = 0;

    int counter_avg = 0;

    //Accept and incoming connection
    printf("[+]TCP protocol socket created -> Waiting for connection with sender\n");

    struct sockaddr_in client_addr;
    socklen_t client_addr_length = sizeof(client_addr);

    double sum_for_average_cubic = 0.0;
    double sum_for_average_reno = 0.0;
    double *part1 = NULL;
    double *part2 = NULL;
    part1 = (double *) malloc(1);
    part2 = (double *) malloc(1);
    char namealgo[250];
    socklen_t length;

    //Check allocate

    char messageexit[1] ;
    messageexit[0]= '0';
    int sockfd = -1;
    int countbytes = 0;
    int j=1;
    sockfd = setSock(); //Create the socket
    int checkPart = 0;
    int rcv=0;
    int Bole=1;

    do {
        

        countbytes = 0;

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr_length = sizeof(client_addr);

        //The receiver accept the connection
        int client_socket = accept(sockfd, (SA *) &client_addr, &client_addr_length);
        if (client_socket == -1) {
            printf("listen failed with error code : \n");
            //close(sockfd);
            return -1;
        } else {
            printf("[+]client number connection accepted\n");
        }
        

        while (1) {
            printf("incheck6\n");

            gettimeofday(&start, 0);
            rcv = recv(client_socket, &buffer, HALF_SIZE, 0);
            printf("1\t%d\n",rcv);
            countbytes += rcv;
            printf("2\t%d\n",countbytes);

            if (countbytes == HALF_SIZE) {


                int k = send(client_socket,XOR, sizeof(buffer_auth),0);
                printf("[+]Sent Ack. Authentication!\n");
                gettimeofday(&end, 0);
                //printf("%d\n",countbytes);
                if (checkPart == 0) { //To handle with  the first recv part

                    printf("Checkpart1\n");

                    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / MILLION;


                    sum_for_average_cubic += time_spent;
                    *(part1 + counter++) = time_spent;
                    printf("Time in microseconds: %f microseconds\n", time_spent);
                    checkPart = 1;
                    size2+=countbytes;
                    countbytes=0;



                } else { // To handle with  the second recv part

                    printf("Checkpart2\n");

                    bzero(namealgo, sizeof(namealgo));
                    printf("\nSwitching to Reno algorithm\n");
                    strcpy(namealgo, "reno");
                    length = strlen(namealgo);
                    if ((setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, namealgo, length)) != 0) {

                        printf("setsockopt_receiver\n");

                        return -1;

                    }
                    double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / MILLION;

                    sum_for_average_cubic += time_spent;
                    *(part2 + counter++) = time_spent;
                    printf("Time in microseconds: %f microseconds\n", time_spent);
                    checkPart = 0;
                    size2+=countbytes;
                    countbytes=0;
                }
                part1 = (double *) realloc(part1, sizeof(double) * (counter + 2));
                part2 = (double *) realloc(part2, sizeof(double) * (counter + 2));


                printf("\nsize2==%d\n\n",size2);
                if (size2 == SIZE) {

                    printf("All the file is recv done\n");
                    printf("\n****\n");
                    // To get the decision of the user
                    bzero(buffreExit, sizeof(buffreExit));
                    printf("\n\n");
                    int x = recv(client_socket, &buffreExit, sizeof(buffreExit), 0);
                    printf("%c\n",buffreExit[0]);
                    printf("%d\n", x);
                    if (buffreExit[0] == messageexit[0]) {

//                  printTimes(part1, part2, counter, sum_for_average_cubic, sum_for_average_reno);
                        //close(client_socket);
                    Bole =0;
                        printf("inCheck1!!\n");

                    break;
                    }else{
                        Bole =1;
                        printf("inCheck2!!\n");
                        size2=0;
                    }




                }

            }

//            if (size2==SIZE){
//                printf("inCheck3!!\n");
//
//                break;
//            }
            //printf("*\n");

        }
        printf("inCheck4!!\n");
        close(client_socket);
        //size2=0;

        counter_avg++;

    }while(Bole);

    //printTimes(part1, part2, counter, sum_for_average_cubic, sum_for_average_reno);

    close(sockfd);
    return 0;
}






void printTimes(double *timePart1, double *timePart2, int len,double sum_Cubic,double sum_Reno) {



    printf("Times Summary:\n");
    printf("Part1:-\n");
    for (int j = 0; j < len; j++) {
        printf("%f\n", *(timePart1 + j));
    }

    printf("Part2:-\n");
    for (int j = 0; j < len; j++) {
        printf("%f\n", *(timePart2 + j));
    }

    double avg_cubic = sum_Cubic / len;
    double avg_reno = sum_Reno / len;

    printf("\n******\n");

    printf("Averages:\n");
    printf("Part1:\t%f\n", avg_cubic);

    printf("Averages:\n");
    printf("Part2:\t%f\n", avg_reno);

    printf("\nEND\n");

    free(timePart1);
    free(timePart2);


}

int setSock(){


    SI server_addr;
    char namealgo[250];
    socklen_t length;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT); // short, network byte order

    // create a socket lisener.
    int sockfd = -1;
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("Couldn't create a socket listener : %d\n",errno);
        perror("socket\n");
        return -1;
    }

    strcpy(namealgo,"cubic");
    length = sizeof(namealgo);
    if (getsockopt(sockfd,IPPROTO_TCP,TCP_CONGESTION,namealgo,&length)!=0){

        perror("getsockopt\n");
        return -1;
    }

    // connect the server to a port which can read and write on.
    if(bind(sockfd, (SA *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Bind failed with error code : %d" , errno);
        return -1;
    }

    printf("Receiver connected to the Sender successfully!\n\n");


    if(listen(sockfd, 500) == -1) {
        printf("listen() failed with error code : %d",errno);
        return -1;
    }

    return sockfd;
}