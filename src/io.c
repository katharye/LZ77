#include "../include/io.h"
#include <string.h>

unsigned char get_byte_from_dict(Dict *d, size_t pos) {
    return d->window[pos % WINDOW_SIZE];
}

void put_byte_in_dict(Dict *d, unsigned char c) {
    d->window[d->pos % WINDOW_SIZE] = c;
    d->pos++;
    if (d->size < WINDOW_SIZE) d->size++;
}

unsigned char dict_get_offset(Dict *d, size_t offset) {
    size_t index = (d->pos - offset + WINDOW_SIZE) % WINDOW_SIZE;

    return d->window[index];
}

void init_dict(Dict *d) {
    d->pos = 0;
    d->size = 0;
    memset(d->window, 0, WINDOW_SIZE);
}

