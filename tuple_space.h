#ifdef __cplusplus
extern "C" {
#endif

#ifndef TUPLE_SPACE_H
#define TUPLE_SPACE_H
#include <stdint.h>

#define TS_YES      1
#define TS_NO       0
#define TS_INT      0
#define TS_SUCCESS  1
#define TS_FAILURE  0

#define TS_OUT  0
#define TS_INP  1

typedef struct {
    int is_actual;
    int type;
    union {
        int int_field;
        float float_field;
        char* string_field;
    } data;
} field_t;

#define COMMAND_TYPE_MASK   0x03
#define COMMAND_TYPE_POS    (sizeof(char) * 8 - 2)
#define NUM_FIELDS_POS      (sizeof(char) * 8 - 3)

// Creates tuple with two fields
void initializeTuple(field_t *fields, int task, int number);

// Sends tuple to tuple space
int ts_out (char*, field_t *, int);

// Takes tuple from tuple space and then deletes it from there
int ts_inp (char*, field_t *, int);

// Serializes tuple so it can be send via UDP
int serializePacket(char* packet, int command, char* tuple_name, field_t* fields, int num_fields);

// Deserializes tuple so it can be received with UDP
int deserializePacket(char* packet, int* command, char* tuple_name, field_t* fields, int* num_fields);

// Changes bytes to int
int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4);

#endif

#ifdef __cplusplus
}
#endif