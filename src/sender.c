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

int main() {
    //allocate memory for authentication and parts of content's file
    char buffer[HALF_SIZE];
    char buffercheck[AUTH_SIZE];
    //output of xor - authentication
    strcpy(buffercheck,"0101 0001 1100 0000");

    int size = 0;
    int size2=0;
    char exitmessage[1];
    
    FILE *file_pointer;
    socklen_t length;
    int count_us=0;
    char namealgo[250];
    int flag=1;

    struct timeval start1,end1;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1) {
        fprintf(stderr, "Socket ERROR - could not create the socket : %s\n", strerror(errno));
        exit(EXIT_FAILURE); 
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
        return -1;
    }

    // Connect to the receiver
    int connection = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(connection == -1) {
        fprintf(stderr, "The connecting to receiver failed - error msg code:%s\n", strerror(errno));
        exit(EXIT_FAILURE); 
    }
    else {
        printf("[+]Sender connected to the Receiver successfully!!\n");
    }

    

    while(flag) {

        int j = 0;
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

            file_pointer = fopen(FIlE_NAME, "r");
            if(file_pointer == NULL) {
                fprintf(stderr, "Failed open the file f1.txt : %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

                //sending the part 1 of the content while FILE OPEN
                printf("[+]send the first part of the file\n");

                fread(buffer,1, sizeof(buffer),file_pointer);
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
            int x=recv(client_socket, &buffercheck, sizeof(buffercheck), 0);
            
            
            
        
            if (strcmp(XOR, buffercheck) == 0) {

                printf("Authentication success!!\n");

            }

        }
            //changing the CC Algorithem by setsockopt
            printf("[+]Change the cc algorithm to reno\n");
            strcpy(namealgo, "reno");
            length = sizeof(namealgo);
            int set_sock_opt = setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, namealgo, length);
            if (set_sock_opt != 0) {
                perror("setsockopt");
                exit(EXIT_FAILURE);
            }

            


            bzero(buffer, sizeof(buffer));
            printf("\n[+]Send the second part of the file\n\n");
            size = 0;
            fread(buffer, 1, sizeof(buffer), file_pointer);
            size = send(client_socket, buffer, sizeof(buffer), 0);
            if (size < 0) {
                printf("Send part2 failed with error!\n");
                exit(1);
            }


            size2 += size;
            printf("%d of content sent!\n", size);
            if (size2 == SIZE) {
                printf("[+]sent all the 2MB file: %d\n", size2);
                fclose(file_pointer);
                printf("\nUser Decision:\n");
                printf("Press 1 to send file again OR 0 to exit the program!\n");
                int flag2;
                scanf("%d", &flag2);
                flag = flag2;

                printf("##############################\n");


                if (flag2 == 0) {

                    exitmessage[0] = '0';
                    printf("\nSender sent the exit message for the receiver!!\n");
                }
                else{
                        exitmessage[0] = '1';
                        

                }


                    
                    int x= send(client_socket, exitmessage, sizeof(exitmessage), 0);
                    
                    //fclose(file_pointer);


            } else {
                printf("sent just %d out of %d\n", size2, SIZE);

            }



            j++;
            size = 0;
            size2 = 0;
    }
    close(client_socket);

    return 0;
}