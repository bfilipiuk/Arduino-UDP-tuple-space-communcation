#include "tuple_space.h"
#include "udp_manager.h"
#include <stdio.h>
#include <string.h>

int bytesToInt(unsigned char byte1, unsigned char byte2, unsigned char byte3, unsigned char byte4) {
    int result = 0;

    result |= (byte4 << 24);
    result |= (byte3 << 16);
    result |= (byte2 << 8);
    result |= byte1;

    return result;
}

int serializePacket(char* packet, int command, char* tuple_name, field_t* fields, int num_fields){
    int total_packet_size = 0;
    unsigned char flags_combined = 0x00;

    flags_combined |= ((command >> 1) & 1) << 7;
    flags_combined |= (command & 1) << 6;
    flags_combined |= ((num_fields - 1) & 1) << 5;
    flags_combined |= (fields[0].is_actual & 1) << 4;
    flags_combined |= (fields[0].type & 1) << 3;

    if (num_fields > 1) {
        flags_combined |= (fields[1].is_actual & 1) << 2;
        flags_combined |= (fields[1].type & 1) << 1;
    }

    packet[total_packet_size++] = flags_combined;
    packet[total_packet_size++] = strlen(tuple_name);
    strncpy(&packet[total_packet_size], tuple_name, strlen(tuple_name));
    total_packet_size += strlen(tuple_name);

    for (int i = 0; i < num_fields; i++) {
        if (fields[i].is_actual == TS_YES) {
            if (fields[i].type == TS_INT) {
                uint32_t int_val = fields[i].data.int_field;
                for (int j = 0; j < sizeof(uint32_t); j++) {
                    packet[total_packet_size++] = (int_val >> (8 * (3 - j))) & 0xFF;
                }
            }
        }
    }

    return total_packet_size;
}

int deserializePacket(char* packet, int* command, char* tuple_name, field_t* fields, int* num_fields) {
    int total_packet_size = 0;

    unsigned char flags_combined = packet[total_packet_size++];

    int tuple_name_length = packet[total_packet_size++];

    for (int i = 0; i < tuple_name_length; ++i) {
        tuple_name[i] = packet[total_packet_size++];
    }
    tuple_name[tuple_name_length] = '\0';

    *command = packet[0] >> COMMAND_TYPE_POS & COMMAND_TYPE_MASK;
    *num_fields = (packet[0] >> NUM_FIELDS_POS) & 1;

    int bit_pos = 4;
    for (int i = 0; i <= *num_fields; i++)
    {
        fields[i].is_actual = packet[0] >> (sizeof(char) * 8 - bit_pos++) & 1;
        fields[i].type = packet[0] >> (sizeof(char) * 8 - bit_pos++) & 1;

        if (fields[i].is_actual == TS_YES) {
            if (fields[i].type == TS_INT) {
                fields[i].data.int_field = bytesToInt(
                    packet[total_packet_size++],
                    packet[total_packet_size++],
                    packet[total_packet_size++],
                    packet[total_packet_size++]
                );
            } 
        }
    }

    return total_packet_size;
}

void initializeTuple(field_t *fields, int task, int number) {
    fields[0].is_actual = TS_YES;
    fields[0].type = TS_INT;
    fields[0].data.int_field = task;

    fields[1].is_actual = TS_YES;
    fields[1].type = TS_INT;
    fields[1].data.int_field = number;
}

int ts_out(char* tuple_name, field_t* fields, int num_fields) {
    unsigned char packet[1024];
    int packetSize = serializePacket((char*)packet, TS_OUT, tuple_name, fields, num_fields);

      if (packetSize > 0) {
        return udp_send_packet(packet, packetSize);
    }

    return TS_FAILURE;
}

int ts_inp(char* tuple_name, field_t* fields, int num_fields) {
    unsigned char packet[1024];
    memset(packet, 0, sizeof(packet));

    int total_packet_size = serializePacket((char*)packet, TS_INP, tuple_name, fields, num_fields);
    if (total_packet_size <= 0) {
        return TS_FAILURE;
    }

    if (udp_send_packet(packet, total_packet_size) != 1) {
        return TS_FAILURE;
    }

    memset(packet, 0, sizeof(packet));
    delay(700);
    int total_packet_size_rec = udp_receive_packet((char*)packet, 1024);
    if (total_packet_size_rec <= 0) {
        return TS_FAILURE;
    }

    int command;
    unsigned char tuple_name_rec[32];
    int num_fields_rec;
    deserializePacket((char*)packet, &command, (char*)tuple_name_rec, fields, &num_fields_rec);

    return TS_SUCCESS;
}