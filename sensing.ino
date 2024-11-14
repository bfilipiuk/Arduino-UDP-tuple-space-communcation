#include <ZsutDhcp.h>
#include <ZsutFeatures.h>
#include <ZsutEthernet.h>
#include <ZsutEthernetUdp.h>
#include "tuple_space.h"
#include "udp_manager.h"

field_t tuple[2];

int sensorValue = 0;
int newSensorValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
  udp_setup();
}

void loop() {
  newSensorValue = ZsutAnalog5Read();
  Serial.print(newSensorValue);
  Serial.print("\n");

  if (newSensorValue != sensorValue && newSensorValue == 1){

    Serial.print("Wykryto zmiane wartosci z 0 na 1!\n");

    sensorValue = newSensorValue;

    initializeTuple(tuple, 5, 1);

    int result = ts_out("sensingReq", tuple, 2);
      if (result == 1) {
        Serial.print("Krotka wyslana pomyslnie: [");
        Serial.print(tuple[0].data.int_field);
        Serial.print(", ");
        Serial.print(tuple[1].data.int_field);
        Serial.println("]\n");
    } else {
        Serial.println("Blad wysylania krotki\n");
      }
    } else if (newSensorValue != sensorValue && newSensorValue == 0){
        Serial.print("Wykryto zmiane wartosci z 1 na 0!\n");

    sensorValue = newSensorValue;

    initializeTuple(tuple, 5, 0);

    int result = ts_out("sensingReq", tuple, 2);
      if (result == 1) {
        Serial.print("Krotka wyslana pomyslnie: [");
        Serial.print(tuple[0].data.int_field);
        Serial.print(", ");
        Serial.print(tuple[1].data.int_field);
        Serial.println("]\n");
    } else {
        Serial.println("Blad wysylania krotki\n");
      }
    } else {
    Serial.print("Wartosc nie zmienila sie.\n");
  }

  delay(300);
}