#ifndef UDP_MANAGER_H
#define UDP_MANAGER_H

#include <Ethernet.h>
#include <EthernetUdp.h>

extern EthernetUDP udp;

// Sets up UDP connection
void udp_setup();

// Sends UDP message
int udp_send_packet(unsigned char *buffer, int length);

// Receives UDP message
int udp_receive_packet(char *buffer, int length);

#endif