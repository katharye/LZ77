#ifndef IO_H
#define IO_H

#include <stdlib.h>
#include <stdint.h>

#define WINDOW_SIZE 20//1<<15
#define LOOKAHEAD_SIZE 6//1<<12
#define SEARCH_SIZE (WINDOW_SIZE - LOOKAHEAD_SIZE)

#define MIN_MATCH = 1//3
#define MAX_MATCH = 5//64


typedef struct {
    char window[W];
    char pos;
    char size;
} Dict;

typedef struct {
    int offset;
    int length;
    char next_char;
} Token;


char get_byte_from_dict(Dict *d, int pos);
void put_byte_in_dict(Dict *d, char c);
char dict_get_offset(Dict *d, int offset);
void init_dict(Dict *d);

#endif