#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/socket.h>




//Defining the important values
#define SERVER_PORT 5566
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SIZE 1001704
#define HALF_SIZE 500852
#define FIlE_NAME "f2.txt"
#define XOR "0001 1010 1101 1010"
#define SA struct sockaddr//
#define SI struct sockaddr_in//

int main() {
    //allocate memory for authentication and parts of content's file
    char buffer[HALF_SIZE];
    char xorcheck[20];
    //output of xor - authentication
    strcpy(xorcheck,"0101 0001 1100 0000");

    int size =0;
    int size2=0;
    char msgexit[1];
    FILE *f;
    socklen_t length;
    int count_us=0;
    char namealgo[250];
    int sign=1;

    

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1) {
        fprintf(stderr, "Socket ERROR - could not create the socket : %s\n", strerror(errno));
        exit(1); 
    }
    else printf("[+]TCP protocol socket created.\n");

    //Sets the first count bytes of destination
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    int rval = inet_pton(AF_INET, (const char*)SERVER_IP_ADDRESS, &server_address.sin_addr);
    if(rval <= 0) {
        printf("Failed to convert an IP to Binary format!");
        exit(1); 
       
    }

    // Connect to the receiver
    int connection = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(connection == -1) {
        fprintf(stderr, "The connecting to receiver failed - error msg code:%s\n", strerror(errno));
        exit(1); 
    }
    else {
        printf("[+]Sender connected to the Receiver successfully!!\n");
    }

    

    while(sign) {

        
        count_us++;
        
        if (count_us>1)
        {
            printf("Changing the CC algorithm to the default ->cubic\n");
        }
        

            // Create clinet socket

            // By default cc algorithm is cubic
            strcpy(namealgo,"cubic");
            int get_sock_opt = getsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, namealgo, &length);
            if( get_sock_opt != 0) {
                perror("getsockopt");
                exit(EXIT_FAILURE);
            }
            //opening the file for read content
            f = fopen(FIlE_NAME, "r");
            if(f == NULL) {
                fprintf(stderr, "Failed open the file f1.txt : %s\n", strerror(errno));
                exit(1);
            }

                //sending the part 1 of the content while FILE OPEN
                printf("[+]send the first part of the file\n");

                fread(buffer,1, sizeof(buffer),f);
                size = send(client_socket, buffer, sizeof(buffer), 0);
                if(size<0){
                    printf("ERROR - Send the half 1 of f1.txt-> failed with error!\n");
                    exit(1);
                }
                
            //the size of half file!
            printf("%d of content sent!\n",size);
            size2+=size;

        if (size==HALF_SIZE) {
            printf("\n");
            recv(client_socket, &xorcheck, sizeof(xorcheck), 0);
            
            
            
            //Give back an Auth sign for sender
            if (strcmp(XOR, xorcheck) == 0) {

                printf("Authentication success!!\n");

            }

        }
            //changing the CC Algorithem by setsockopt
            printf("[+]Change the cc algorithm to reno\n");
            strcpy(namealgo, "reno");
            length = sizeof(namealgo);
            int set_sock_opt = setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, namealgo, length);
            if (set_sock_opt != 0) {
                perror("setsockopt - changing the CC algo failed");
                exit(1);
            }

            

            //sending the next half file to receiver
            bzero(buffer, sizeof(buffer));
            printf("\n[+]Send the second part of the file\n\n");
            size = 0;
            fread(buffer, 1, sizeof(buffer), f);
            size = send(client_socket, buffer, sizeof(buffer), 0);
            if (size < 0) {
                printf("Send part2 failed with error!\n");
                exit(1);
            }

            //if size2 is full receiver got everything
            size2 += size;
            printf("%d of content sent!\n", size);
            if (size2 == SIZE) {
                printf("[+]sent all the 1MB file: %d\n", size2);
                fclose(f);
                printf("\nUser Decision:\n");
                printf("Press 1 to send file again OR 0 to exit the program!\n");
                int indication_us;
                scanf("%d", &indication_us);
                sign = indication_us;

                printf("###################################\n");

                //indication of uder from the sender/ if 1 ->send again else->print times and exit
                if (indication_us == 0) {

                    msgexit[0] = '0';
                    printf("\nSender sent the exit message for the receiver!!\n");
                }
                else{
                        msgexit[0] = '1';
                        

                }

                    send(client_socket, msgexit, sizeof(msgexit), 0);
                    
            } else {
                printf("sent just %d out of %d\n", size2, SIZE);

            }

            size = 0;
            size2 = 0;
    }
    close(client_socket);

    return 0;
}