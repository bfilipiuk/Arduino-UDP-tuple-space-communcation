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

    field_t fields[2]; // Przyjmujemy, że każda krotka ma 2 pola

    // Utworzenie gniazda UDP
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Błąd przy tworzeniu gniazda");
        exit(EXIT_FAILURE);
    }

    // Konfiguracja adresu serwera
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Przypisanie adresu do gniazda
    if (bind(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0) {
        perror("Błąd przy przypisywaniu adresu do gniazda");
        exit(EXIT_FAILURE);
    }

    printf("Serwer UDP uruchomiony na porcie %d.\n", SERVER_PORT);

    while (1) {
        struct sockaddr_in clientAddr;
        socklen_t addr_size = sizeof(clientAddr);
        char buffer[BUFFER_SIZE];

        int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &clientAddr, &addr_size);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';

            int command, num_fields;
            char tuple_name[32];
            deserializePacket(buffer, &command, tuple_name, fields, &num_fields);
            printf("Otrzymano polecenie: %d, nazwa: %s\n", command, tuple_name);

            if (strcmp(tuple_name, "managerReq") == 0) {
                if (command == TS_OUT) {
                    // Dodanie krotki do tablicy taskow
                    if (tupleCount < MAX_TUPLES) {
                        tuples[tupleCount][0] = fields[0]; // Pierwsze pole
                        tuples[tupleCount][1] = fields[1]; // Drugie pole
                        tupleCount++;
                    } else {
                        printf("Brak miejsca dla nowych krotek taskow.\n");
                    }
                }
                
                if (command == TS_INP) {
                    memcpy(&managerAddr, &clientAddr, sizeof(clientAddr));
                    managerAddrSet = 1;

                    usleep(200);

                    if (managerAddrSet && resTupleCount > 0) {
                        char send_buffer[BUFFER_SIZE];
                        int total_packet_size = serializePacket(send_buffer, TS_OUT, tuple_name, res_tuples[0], 2);

                        sendto(sockfd, send_buffer, total_packet_size, 0, (struct sockaddr *) &managerAddr, managerAddrSize);
                        printf("Wysyłanie pakietu do managera. Dane: [%d, %d]\n", res_tuples[0][0].data.int_field, res_tuples[0][1].data.int_field);

                        // Usunięcie wysłanej krotki i przesunięcie pozostałych krotek
                        for (int i = 0; i < resTupleCount - 1; i++) {
                            res_tuples[i][0] = res_tuples[i + 1][0];
                            res_tuples[i][1] = res_tuples[i + 1][1];
                        }
                        resTupleCount--;
                    }
                }
            }                

            if (strcmp(tuple_name, "workerReq") == 0) {
                if (command == TS_OUT) {
                    // Dodanie krotki do tablicy wynikow
                    if (resTupleCount < MAX_TUPLES) {
                        res_tuples[resTupleCount][0] = fields[0]; // Pierwsze pole
                        res_tuples[resTupleCount][1] = fields[1]; // Drugie pole
                        resTupleCount++;
                    } else {
                        printf("Brak miejsca dla nowych krotek wynikow.\n");
                    }
                }
            

                if (command == TS_INP) {
                    memcpy(&sensingAddr, &clientAddr, sizeof(clientAddr));
                    sensingAddrSet = 1;

                    usleep(200);

                    if (sensingAddrSet && tupleCount > 0) {
                        char send_buffer[BUFFER_SIZE];
                        int total_packet_size = serializePacket(send_buffer, TS_OUT, tuple_name, tuples[0], 2);
                        // printf("%d\n", total_packet_size);
                        sendto(sockfd, send_buffer, total_packet_size, 0, (struct sockaddr *) &sensingAddr, sensingAddrSize);
                        printf("Wysyłanie pakietu do workera. Dane: [%d, %d]\n", tuples[0][0].data.int_field, tuples[0][1].data.int_field);

                        // Usunięcie wysłanej krotki i przesunięcie pozostałych krotek
                        for (int i = 0; i < tupleCount - 1; i++) {
                            tuples[i][0] = tuples[i + 1][0];
                            tuples[i][1] = tuples[i + 1][1];
                        }
                        tupleCount--;
                    }
                }
            }

            // Wypisanie aktualnego stanu przestrzeni krotek
            printf("Przestrzeń krotek taskow: \n");
            printf("[");
            for (int i = 0; i < tupleCount; i++) {
                printf("[%d, %d]", tuples[i][0].data.int_field, tuples[i][1].data.int_field);
                if (i < tupleCount - 1) {
                    printf(", ");
                }
            }
            printf("]\n");

            printf("Przestrzeń krotek wynikow: \n");
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
