#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WINDOW_SIZE 16384
#define LOOKAHEAD_SIZE 4096
#define SEARCH_SIZE (WINDOW_SIZE - LOOKAHEAD_SIZE)

#define MIN_MATCH 2
#define MAX_MATCH 16

typedef struct {
    size_t offset;
    size_t length;
    unsigned char next_char;
} Token;

unsigned char window[WINDOW_SIZE];
size_t real_size;
size_t absolute_pos;

unsigned char get_byte(size_t absolute_pos) { return window[absolute_pos % WINDOW_SIZE]; }

void print_buf(size_t coding_pos, size_t absolute_pos) {
    printf("|");
    for (size_t i = 0; i < WINDOW_SIZE; i++) {
        if (i == coding_pos % WINDOW_SIZE) printf("}{");
        printf("%c", get_byte(i));
        if (i == absolute_pos % WINDOW_SIZE) printf("][");
    }
    printf("|\n");
    printf("W_size = %d | S_size = %d | L_size = %d\n", WINDOW_SIZE, SEARCH_SIZE, LOOKAHEAD_SIZE);
}

Token find_longest_token(size_t coding_pos, size_t absolute_pos) {
    if (coding_pos == 0) {
        return (Token){0, 0, window[0]};
    }
    
    size_t search_end = coding_pos - 1;
    size_t search_start = (coding_pos > SEARCH_SIZE) ? coding_pos - SEARCH_SIZE : 0;
    
    Token default_token = {0, 0, window[coding_pos % WINDOW_SIZE]};

    int matches_checked = 0;
    #define MAX_MATCHES 32

    for (size_t match_pos = search_start; match_pos <= search_end; match_pos++) {
        if (window[match_pos % WINDOW_SIZE] == window[coding_pos % WINDOW_SIZE]) {
            matches_checked++;
            if (matches_checked > MAX_MATCHES) break;
            size_t length = 1;


            while (
                coding_pos + length <= absolute_pos &&
                match_pos - length < coding_pos &&
                length < MAX_MATCH &&
                window[(match_pos + length) % WINDOW_SIZE] == window[(coding_pos + length) % WINDOW_SIZE]
            ) {
                    length++;
                }
            

            if (length >= MIN_MATCH) {
                unsigned char next = (coding_pos + length <= absolute_pos)
                    ? window[(coding_pos + length) % WINDOW_SIZE] 
                    : 0;

                Token new_token = {coding_pos - match_pos, length, next};
                if (new_token.length > default_token.length) {
                    default_token = new_token;

                    if (default_token.length >= 128) {
                        return default_token;
                    }
                }
            }
        }
    }
    return default_token;
}

int encoder(char* filename_in, char* filename_out) {
    FILE* f_in = fopen(filename_in, "rb");
    FILE* f_out = fopen(filename_out, "wb");

    if (!f_in || !f_out) {
        if (f_in) fclose(f_in);
        if (f_out) fclose(f_out);
        return 1;
    }

    real_size = fread(window, 1, LOOKAHEAD_SIZE, f_in);
    if (real_size == 0) {
        fclose(f_in);
        fclose(f_out);
        return 1;
    }

    absolute_pos = real_size - 1; 
    size_t coding_pos = 0;


    while (coding_pos <= absolute_pos) {
        Token token = find_longest_token(coding_pos, absolute_pos);

        printf("<%zu, %zu, %c>\n", token.offset, token.length, token.next_char);

        fwrite(&token.offset, sizeof(size_t), 1, f_out);
        fwrite(&token.length, sizeof(size_t), 1, f_out);
        fwrite(&token.next_char, sizeof(unsigned char), 1, f_out);

        size_t shift = (token.length > 0) ? token.length + 1 : 1;
        coding_pos += shift;

        if(real_size >= LOOKAHEAD_SIZE) {
            size_t bytes_read = 0;
            size_t write_pos = absolute_pos + 1;
            for (size_t i = 0; i < shift; i++) {
                int ch = fgetc(f_in);
                if (ch == EOF) break;
                window[(write_pos + i) % WINDOW_SIZE] = (unsigned char)ch;
                bytes_read++;
            }
            absolute_pos += bytes_read;
        }    
    }
    fclose(f_in);
    fclose(f_out);
}

int main() {
    encoder("/home/katharye/Projects/C/LZ77/src/input.txt", "test.lz77");
    return 0;
}