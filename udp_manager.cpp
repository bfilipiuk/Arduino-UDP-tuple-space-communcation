#include "udp_manager.h"
#include <stdlib.h>
#include <time.h>
#include <Ethernet.h>
#include <EthernetUdp.h>

#define SERVER_PORT 12345

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

unsigned int localPort = random(5000,6000);
unsigned int serverPort = SERVER_PORT;
IPAddress serverIP(192, 168, 0, 47);
EthernetUDP udp;

// Initialize UDP server
void udp_setup() {
    ZsutEthernet.begin(mac);
    delay(1500);

    srand(ZsutMillis());
    Serial.println("Polaczono z siecia Ethernet");

    localPort = random(5000,6000);
    udp.begin(localPort);
}

// UDP send packet function
int udp_send_packet(unsigned char *buffer, int length) {
    udp.beginPacket(serverIP, serverPort);
    udp.write(buffer, length);
    udp.endPacket();

    return 1;
}

// UDP receive packet function
int udp_receive_packet(char *buffer, int length)
{
  int packetSize = udp.parsePacket();
  if (packetSize > 0)
  {
    udp.read(buffer, length);
    return packetSize;
  }
  return 0;
}