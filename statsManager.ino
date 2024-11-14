#include <ZsutEthernet.h>
#include <ZsutEthernetUdp.h>
#include "tuple_space.h"
#include "udp_manager.h"

FILE f_out;
int sput(char c, __attribute__((unused)) FILE* f) {return !Serial.write(c);}

field_t result_fields[2];
int num_fields = 2;
int changes_zero_count = 0;
int changes_one_count = 0;

void setup() {
  Serial.begin(9600);

  fdev_setup_stream(&f_out, sput, nullptr, _FDEV_SETUP_WRITE); // cf https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaf41f158c022cbb6203ccd87d27301226
  stdout = &f_out;

  udp_setup();
}

void loop() {
  int received = ts_inp("managerReq", result_fields, num_fields);
  if (received == TS_SUCCESS) {
    printf("[");
    // Wypisanie zawartości otrzymanej krotki
    for (int i = 0; i < num_fields; ++i) {
      printf("%d", result_fields[i].data.int_field);
      if (i < num_fields - 1) {
        printf(", ");
      }  
    }
    printf("]\n");

    if (result_fields[1].data.int_field == 0) {
      changes_zero_count++;
    } else if (result_fields[1].data.int_field == 1) {
      changes_one_count++;
    }
  }

  printf("Ilosc zmian stanu na 0: %d\n", changes_zero_count);
  printf("Ilosc zmian stanu na 1: %d\n", changes_one_count);

  printf("\n");
  
  delay(200);
}
