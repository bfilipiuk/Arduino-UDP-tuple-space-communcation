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
    int is_actual;  /* does the data member contains data */
    int type;       /* what is the type of the data member */
    union {
        int int_field;
        float float_field;
        char* string_field;
    } data;
} field_t;          /* a new type corresponding to one field of a tuple*/

/* API */

#define COMMAND_TYPE_MASK   0x03
#define COMMAND_TYPE_POS    (sizeof(char) * 8 - 2)
#define NUM_FIELDS_POS      (sizeof(char) * 8 - 3)

// this function creates tuple with two fields
void initializeTuple(field_t *fields, int task, int number);

// this function sends tuple to tuple space
int ts_out (char*, field_t *, int);

// this function takes tuple from tuple space and then deletes it from there
int ts_inp (char*, field_t *, int);

// this function serializes tuple so it can be send via UDP
int serializePacket(char* packet, int command, char* tuple_name, field_t* fields, int num_fields);

// this function deserializes tuple so it can be received with UDP
int deserializePacket(char* packet, int* command, char* tuple_name, field_t* fields, int* num_fields);

// changes bytes to int
int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4);

#endif

#ifdef __cplusplus
}
#endif