#include "../include/io.h"
#include <stdio.h>
#include <stdint.h>

unsigned char* put_file_in_data(const char* filename, size_t* file_size, int* all_okay) {
    FILE *file = fopen(filename, "rb");

    *all_okay = 0;
    if (!file) return NULL;

    if(fseek(file, 0, SEEK_END)) {
        fclose(file);
        return NULL;
    }
    
    long pos = ftell(file);
    if (pos <= 0) {
        fclose(file);
        return NULL;
    }
    
    *file_size = (size_t)pos;

    if(fseek(file, 0, SEEK_SET)) {
        fclose(file);
        return NULL;
    }

    unsigned char* data = malloc(sizeof(char) * (*file_size));
    if (data == NULL) {
        fclose(file);
        return NULL;
    }

    size_t read_count = fread(data, 1, *file_size, file);
    if (read_count != *file_size) {
        free(data);
        return NULL;
    }

    fclose(file);

    *all_okay = 1;
    return data;
}

int put_data_in_file(FILE* file, uint16_t length, uint16_t value) {
    uint16_t token;

    token = (length & 0xF) | ((value & 0xFFF) << 4);
    
    size_t result = fwrite(&token, sizeof(uint16_t), 1, file);
    if (result != 1) {
        return 1;
    }

    return 0;
}