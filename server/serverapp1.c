#include "tuple_space_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_PORT 12345
#define BUFFER_SIZE 1024
#define MAX_TUPLES 100

field_t tuples[MAX_TUPLES][2];
field_t res_tuples[MAX_TUPLES][2];
int tupleCount = 0;
int resTupleCount = 0;

struct sockaddr_in sensingAddr;
socklen_t sensingAddrSize = sizeof(sensingAddr);
int sensingAddrSet = 0;

struct sockaddr_in managerAddr;
socklen_t managerAddrSize = sizeof(managerAddr);
int managerAddrSet = 0;

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;

    field_t fields[2];

    // Initializing UDP socket
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error while creating socket");
        exit(EXIT_FAILURE);
    }

    // Server address configuration
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Binding address to socket
    if (bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error while binding address to socket");
        exit(EXIT_FAILURE);
    }

    printf("UDP server running on port %d.\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t addr_size = sizeof(clientAddr);
        char buffer[BUFFER_SIZE];

        // Checking for clients requests
        int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &clientAddr, &addr_size);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';

            int command, num_fields;
            char tuple_name[32];
            deserializePacket(buffer, &command, tuple_name, fields, &num_fields);
            printf("Received command: %d, name: %s\n", command, tuple_name);

            // Checking if request is from manager
            if (strcmp(tuple_name, "managerReq") == 0) {
                if (command == TS_OUT) {
                    // Adding task to tasks tuple space
                    if (tupleCount < MAX_TUPLES) {
                        tuples[tupleCount][0] = fields[0];
                        tuples[tupleCount][1] = fields[1];
                        tupleCount++;
                    } else {
                        printf("Maximum tasks have been reached.\n");
                    }
                }
                
                if (command == TS_INP) {
                    // Sending solution to manager
                    memcpy(&managerAddr, &clientAddr, sizeof(clientAddr));
                    managerAddrSet = 1;

                    usleep(200);

                    if (managerAddrSet && resTupleCount > 0) {
                        char send_buffer[BUFFER_SIZE];
                        int total_packet_size = serializePacket(send_buffer, TS_OUT, tuple_name, res_tuples[0], 2);

                        sendto(sockfd, send_buffer, total_packet_size, 0, (struct sockaddr *) &managerAddr, managerAddrSize);
                        printf("Sending solution to manager. Data: [%d, %d]\n", res_tuples[0][0].data.int_field, res_tuples[0][1].data.int_field);

                        for (int i = 0; i < resTupleCount - 1; i++) {
                            res_tuples[i][0] = res_tuples[i + 1][0];
                            res_tuples[i][1] = res_tuples[i + 1][1];
                        }
                        resTupleCount--;
                    }
                }
            }                

            // Checking if request is from worker
            if (strcmp(tuple_name, "workerReq") == 0) {
                if (command == TS_OUT) {
                    // Adding tuple to solutions tuple space
                    if (resTupleCount < MAX_TUPLES) {
                        res_tuples[resTupleCount][0] = fields[0];
                        res_tuples[resTupleCount][1] = fields[1];
                        resTupleCount++;
                    } else {
                        printf("Maximum solutions have been reached.\n");
                    }
                }
            

                if (command == TS_INP) {
                    // Sending task to worker
                    memcpy(&sensingAddr, &clientAddr, sizeof(clientAddr));
                    sensingAddrSet = 1;

                    usleep(200);

                    if (sensingAddrSet && tupleCount > 0) {
                        char send_buffer[BUFFER_SIZE];
                        int total_packet_size = serializePacket(send_buffer, TS_OUT, tuple_name, tuples[0], 2);
                        sendto(sockfd, send_buffer, total_packet_size, 0, (struct sockaddr *) &sensingAddr, sensingAddrSize);
                        printf("Sending task to worker. Data: [%d, %d]\n", tuples[0][0].data.int_field, tuples[0][1].data.int_field);

                        for (int i = 0; i < tupleCount - 1; i++) {
                            tuples[i][0] = tuples[i + 1][0];
                            tuples[i][1] = tuples[i + 1][1];
                        }
                        tupleCount--;
                    }
                }
            }

            // Printing current tasks tuple space status
            printf("Tasks tuple space: \n");
            printf("[");
            for (int i = 0; i < tupleCount; i++) {
                printf("[%d, %d]", tuples[i][0].data.int_field, tuples[i][1].data.int_field);
                if (i < tupleCount - 1) {
                    printf(", ");
                }
            }
            printf("]\n");

            // Printing current solutions tuple space status
            printf("Solutions tuple space: \n");
            printf("[");
            for (int i = 0; i < resTupleCount; i++) {
                printf("[%d, %d]", res_tuples[i][0].data.int_field, res_tuples[i][1].data.int_field);
                if (i < resTupleCount - 1) {
                    printf(", ");
                }
            }
            printf("]\n");

            printf("\n");
        }
    }
    return 0;
}
