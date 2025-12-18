#include "../include/encoder.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/io.h"

void write_to_stream(uint16_t* f_out, uint16_t length, uint16_t value, size_t size) {
    uint16_t token;

    token = (length & 0xF) | ((value & 0xFFF) << 4);

    f_out[size] = token;
}

Token find_longest_token(unsigned char* window, size_t coding_pos, size_t search_start,
                         size_t lookahead_end) {
    Token default_token = {0, 0, window[coding_pos]};

    for (size_t match_pos = search_start; match_pos + 1 < coding_pos; match_pos++) {
        if (window[match_pos] == window[coding_pos]) {
            size_t length = 1;

            while (coding_pos + length < lookahead_end && match_pos + length < coding_pos &&
                   length < MAX_MATCH && window[(match_pos + length)] == window[(coding_pos + length)]) {
                length++;
            }

            unsigned char next = (coding_pos + length <= lookahead_end) ? window[(coding_pos + length)] : 0;

            Token new_token = {coding_pos - match_pos, length, next};
            if (new_token.length > default_token.length) {
                default_token = new_token;
            }
        }
    }
    return default_token;
}

uint16_t* encoder(char* filename_in, size_t* size, int* all_okay) {
    size_t file_size;
    unsigned char* file_in_data = put_file_in_data(filename_in, &file_size, all_okay);
    if (*all_okay == 0) return NULL;

    size_t capacity = 100;
    uint16_t* out = malloc(sizeof(uint16_t) * capacity);
    *size = 0;

    size_t coding_pos = 0;
    size_t search_start = 0;
    size_t lookahead_end = (LOOKAHEAD_SIZE < file_size) ? LOOKAHEAD_SIZE : file_size;

    while (coding_pos < file_size) {
        search_start = (coding_pos < SEARCH_SIZE) ? 0 : coding_pos - SEARCH_SIZE;
        lookahead_end = (coding_pos + LOOKAHEAD_SIZE > file_size) ? file_size : coding_pos + LOOKAHEAD_SIZE;

        Token token = find_longest_token(file_in_data, coding_pos, search_start, lookahead_end);

        if (token.length == 0) {
            write_to_stream(out, (uint16_t)token.length, (uint16_t)token.next_char, *size);
        } else {
            write_to_stream(out, (uint16_t)token.length, (uint16_t)token.offset, *size);
        }

        (*size)++;
        if (*size == capacity) {
            capacity *= 2;
            out = realloc(out, sizeof(uint16_t) * capacity);
        }

        size_t shift = (token.length > 0) ? token.length : 1;
        coding_pos += shift;
    }

    out = realloc(out, sizeof(uint16_t) * *size);

    return out;
}