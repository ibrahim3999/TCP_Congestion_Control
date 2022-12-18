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




#define AUTH_SIZE 21
#define FIlE_NAME "f2.txt"
#define SERVER_PORT 5566
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SIZE 1048576  
#define HALF_SIZE 524288

// ID AUTHORS: 313357196, 207101734

#define XOR "0001 1010 1101 1010"
#define SA struct sockaddr
#define SI struct sockaddr_in

int main() {
    //allocate memory for authentication and parts of content's file
    char buffer[HALF_SIZE];
    char buffercheck[AUTH_SIZE];
    //authentication
    strcpy(buffercheck,"0101 0001 1100 0000");

    int size = 0;
    int send_size=0;
    char exitmessage[1];
    
    FILE *file_pointer;
    socklen_t length;
    int count_us=0;
    char namealgo[250];
    
    struct timeval start1,end1;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket == -1) {
        perror("[-]Error created socket");
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
        return -1;
    }

    // Connect to the receiver
    int connection = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(connection == -1) {
        perror("[-]The connecting to receiver failed\n");
        exit(1); 
    }
    
    printf("[+]Sender connected to the Receiver successfully!!\n");

    char flag='m';

    while(flag!='e') {

        
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
            //open my file
            file_pointer = fopen(FIlE_NAME, "r");
            if(file_pointer == NULL) {
                fprintf(stderr, "Failed open the file f1.txt : %s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }

                //part one sending
                printf("[+]send the first part of the file\n");

                fread(buffer,1, sizeof(buffer),file_pointer);
                size = send(client_socket, buffer, sizeof(buffer), 0);
                if(size<0){
                    perror("[+]ERROR - Send the half 1 of f1.txt-> failed with error!\n");
                    exit(1);
                }
            // Half size 

            send_size+=size;

        if (size==HALF_SIZE) {
            printf("\n");
            int x=recv(client_socket, &buffercheck, sizeof(buffercheck), 0);

            if (strcmp(XOR, buffercheck) == 0) {printf("[+]Authentication success!!\n");}

        }
            //Change algorithm 
            printf("[+]Change the cc algorithm to reno\n");
            strcpy(namealgo, "reno");
            length = sizeof(namealgo);
            int set_sock_opt = setsockopt(client_socket, IPPROTO_TCP, TCP_CONGESTION, namealgo, length);
            if (set_sock_opt != 0) {
                perror("setsockopt");
                exit(1);
            }

            

            //sending the next half file to receiver
            bzero(buffer, sizeof(buffer));
            printf("\n[+]Send the second part of the file\n\n");
            size = 0;
            fread(buffer, 1, sizeof(buffer), file_pointer);
            size = send(client_socket, buffer, sizeof(buffer), 0);
            if (size < 0) {
                printf("[-]Falied sender second PART !!\n");
                exit(1);
            }


            send_size += size;
            if (send_size == SIZE) {
                printf("[+]sent all the 2MB file: %d\n", send_size);
                fclose(file_pointer);
                printf("\nUser Decision:\n");
                printf("Press Any char to send file again OR 'e' to exit the program!\n");
                char flag2;
                scanf("%c", &flag2);
                flag = flag2;

                printf("\n\n");

                if (flag2 == 'e') {

                    exitmessage[0] = 'e';
                    printf("\n The client EXIT !!\n");
                }
                int x= send(client_socket, exitmessage, sizeof(exitmessage), 0);
                    
                


            } else {
                printf("sent just %d out of %d\n", send_size, SIZE);

            }

            size = 0;
            send_size = 0;
    }
    close(client_socket);
    return 1;

}