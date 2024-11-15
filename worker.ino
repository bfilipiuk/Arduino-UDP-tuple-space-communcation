#include <Ethernet.h>
#include <EthernetUdp.h>
#include "tuple_space.h"
#include "udp_manager.h"

FILE f_out;

int sput(char c, __attribute__((unused)) FILE* f) {return !Serial.write(c);}

bool isPrime(int number) {
    if (number <= 1) {
        return false;
    }

    for (int i = 2; i * i <= number; i++) {
        if (number % i == 0) {
            return false;
        }
    }
    
    return true;
}

void setup() {
    Serial.begin(9600);

    fdev_setup_stream(&f_out, sput, nullptr, _FDEV_SETUP_WRITE); // cf https://www.nongnu.org/avr-libc/user-manual/group__avr__stdio.html#gaf41f158c022cbb6203ccd87d27301226
    stdout = &f_out;

    udp_setup();
}

void loop() {
    field_t fields[2];
    field_t result_fields[2];
    int num_fields = 2;
    bool result;

    // Taking tuple from tuple space
    int received = ts_inp("workerReq", fields, num_fields);
    if (received == TS_SUCCESS) {
        Serial.print("Received: [");
        for (int i = 0; i < num_fields; ++i) {
            Serial.print(fields[i].data.int_field);
            if (i < num_fields - 1) {
                Serial.print(", ");
            }  
        }
        Serial.println("]");

        // Checking if number is prime
        result = isPrime(fields[1].data.int_field);
        if (result == true) {
            Serial.print(fields[1].data.int_field);
            Serial.println("-is prime-1");

            initializeTuple(result_fields, 1, fields[1].data.int_field);

            Serial.print("Tuple to send: ");
            Serial.print(result_fields[0].data.int_field);
            Serial.print(", ");
            Serial.print(result_fields[1].data.int_field);
            Serial.println("]");
            
            // Sending tuple to tuple space
            ts_out("workerReq", result_fields, 2);

        } else {
            Serial.print(fields[1].data.int_field);
            Serial.println("-is not prime-0");

            initializeTuple(result_fields, 0, fields[1]. data.int_field);

            Serial.print("Tuple to send: [");
            Serial.print(result_fields[0].data.int_field);
            Serial.print(", ");
            Serial.print(result_fields[1].data.int_field);
            Serial.println("]");

            // Sending tuple to tuple space
            ts_out("workerReq", result_fields, 2);
        }
    } else {
        Serial.println("Error while receiving tuple");
    }

    Serial.println();

    delay(200);
}