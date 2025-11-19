#include "../include/io.h"
#include <stdio.h>
#include <unistd.h>

void dict_print_used_bytes(const Dict *d) {
    for (size_t i = 0; i < d->size; i++) {
        printf("%u ", (unsigned)d->window[i]);
    }
    printf("| ");
    for (size_t i = 0; i < d->size; i++) {
        printf("%c ", (char)d->window[i]);
    }
    printf("\n");
}

void put_string_in_dict(Dict *dict, int ch)

int main() {
    Dict dict;
    init_dict(&dict);

    FILE *file = fopen("test.txt", "rb");
    
    uint8_t ch;
    while ((ch = fgetc(file)) != EOF) {
        size_t search_start = 
    }
    return 0;
}