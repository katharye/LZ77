#include <stddef.h>
#include <stdint.h>

#ifndef ENCODER_H
#define ENCODER_H

#define LOOKAHEAD_SIZE 4096
#define SEARCH_SIZE 4095

#define MAX_MATCH 15

typedef struct {
    size_t offset;
    size_t length;
    unsigned char next_char;
} Token;

uint16_t* encoder(char* filename_in, size_t* size, int* all_okay);

#endif