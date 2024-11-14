#include <ZsutEthernet.h>
#include <ZsutEthernetUdp.h>
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
    field_t fields[2]; // Zakładamy, że mamy dwa pola w krotce
    field_t result_fields[2];
    int num_fields = 2; // Liczba pól w krotce
    bool result;

    int received = ts_inp("workerReq", fields, num_fields);
    if (received == TS_SUCCESS) {
        Serial.print("Otrzymano: [");
        // Wypisanie zawartości krotki
        for (int i = 0; i < num_fields; ++i) {
            Serial.print(fields[i].data.int_field);
            if (i < num_fields - 1) {
                Serial.print(", ");
            }  
        }
        Serial.println("]");

        result = isPrime(fields[1].data.int_field);
        if (result == true) {
            Serial.print(fields[1].data.int_field);
            Serial.println("-liczba pierwsza-1");

            initializeTuple(result_fields, 1, fields[1].data.int_field);

            Serial.print("Krotka do wysłania: ");
            Serial.print(result_fields[0].data.int_field);
            Serial.print(", ");
            Serial.print(result_fields[1].data.int_field);
            Serial.println("]");

            ts_out("workerReq", result_fields, 2);

        } else {
            Serial.print(fields[1].data.int_field);
            Serial.println("-nie liczba pierwsza-0");

            initializeTuple(result_fields, 0, fields[1]. data.int_field);

            Serial.print("Krotka do wyslania: [");
            Serial.print(result_fields[0].data.int_field);
            Serial.print(", ");
            Serial.print(result_fields[1].data.int_field);
            Serial.println("]");

            ts_out("workerReq", result_fields, 2);
        }
    } else {
        Serial.println("Blad przy odbieraniu krotki");
    }

    Serial.println();

    delay(200);
}