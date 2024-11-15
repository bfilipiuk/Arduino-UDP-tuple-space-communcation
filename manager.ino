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
  // Initializing tuple
  field_t result_fields[2];
  int num_fields = 2;

  initializeTuple(tuple, 1, random(1, 100));

  // Sending tuple to server (tuple_space)
  int result = ts_out("managerReq", tuple, num_fields);
    if (result == 1) {
      Serial.print("Tuple sent succesfuly: [");
      Serial.print(tuple[0].data.int_field);
      Serial.print(", ");
      Serial.print(tuple[1].data.int_field);
      Serial.println("]");
    } else {
      Serial.println("Error while sending tuple");
    }

  delay(200);

  int received = ts_inp("managerReq", result_fields, num_fields);

  if (received == TS_SUCCESS) {
    // Printing received tuple
    Serial.print("Received: [");
    for (int i = 0; i < num_fields; ++i) {
      Serial.print(result_fields[i].data.int_field);
      if (i < num_fields - 1) {
        Serial.print(", ");
      }  
    }
    Serial.println("]");

    // Saving results
    if (result_fields[0].data.int_field == 1 && resultsPrimeCount < 100) {
      results_prime[resultsPrimeCount] = result_fields[1].data.int_field;
      resultsPrimeCount++;
    } else {
      results_others[resultsOthersCount] = result_fields[1].data.int_field;
      resultsOthersCount++;
    }
  }

  // Printing current results
  Serial.print("Prime numbers: [");
  for (int i = 0; i < resultsPrimeCount; i++){
    Serial.print(results_prime[i]);
    if (i < resultsPrimeCount -1 ) {
    Serial.print(",");
    }
  }
  Serial.println("]");

  Serial.print("Other numbers: [");
  for (int i = 0; i < resultsOthersCount; i++){
    Serial.print(results_others[i]);
    if (i < resultsOthersCount -1 ) {
    Serial.print(",");
    }
  }
  Serial.println("]");
  Serial.println();
}
