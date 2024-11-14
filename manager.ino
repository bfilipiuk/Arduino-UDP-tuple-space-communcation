#include <ZsutEthernet.h>
#include <ZsutEthernetUdp.h>
#include "tuple_space.h"
#include "udp_manager.h"

field_t tuple[2];
int results_prime[100];
int results_others[100];

int resultsPrimeCount = 0;
int resultsOthersCount = 0;

void setup() {
  Serial.begin(9600);
  udp_setup();
}

void loop() {
  // initializing tuple

  field_t result_fields[2];
  int num_fields = 2;

  initializeTuple(tuple, 1, random(1, 100));

  // sending tuple to server (tuple_space)

  int result = ts_out("managerReq", tuple, num_fields);
    if (result == 1) {
      Serial.print("Krotka wyslana pomyslnie: [");
      Serial.print(tuple[0].data.int_field);
      Serial.print(", ");
      Serial.print(tuple[1].data.int_field);
      Serial.println("]");
    } else {
      Serial.println("Blad wysylania krotki");
    }

  delay(200);

  int received = ts_inp("managerReq", result_fields, num_fields);

  if (received == TS_SUCCESS) {
    Serial.print("Otrzymano: [");
    // Wypisanie zawartości otrzymanej krotki
    for (int i = 0; i < num_fields; ++i) {
      Serial.print(result_fields[i].data.int_field);
      if (i < num_fields - 1) {
        Serial.print(", ");
      }  
    }
    Serial.println("]");

    // Wkładanie wyników do odpowiednich tablic
    if (result_fields[0].data.int_field == 1 && resultsPrimeCount < 100) {
      results_prime[resultsPrimeCount] = result_fields[1].data.int_field;
      resultsPrimeCount++;
    } else {
      results_others[resultsOthersCount] = result_fields[1].data.int_field;
      resultsOthersCount++;
    }
  }

  // Wypisywanie wyników
  Serial.print("Liczby pierwsze: [");
  for (int i = 0; i < resultsPrimeCount; i++){
    Serial.print(results_prime[i]);
    if (i < resultsPrimeCount -1 ) {
    Serial.print(",");
    }
  }
  Serial.println("]");

  Serial.print("Liczby inne: [");
  for (int i = 0; i < resultsOthersCount; i++){
    Serial.print(results_others[i]);
    if (i < resultsOthersCount -1 ) {
    Serial.print(",");
    }
  }
  Serial.println("]");

  Serial.println();
}
