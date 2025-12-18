#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool decoder(FILE *archive, FILE *f_out, unsigned long long filesize, long start_file_offset,
             long end_file_offset) {
    unsigned char *exit_buffer = malloc(filesize > 0 ? filesize : 1);
    if (!exit_buffer) return false;

    fseek(archive, start_file_offset, SEEK_SET);

    long compressed_size = end_file_offset - start_file_offset;
    if (compressed_size <= 0) {
        free(exit_buffer);
        return true;
    }

    uint16_t token;
    size_t end = 0;
    long bytes_read = 0;

    while (bytes_read < compressed_size) {
        if (fread(&token, sizeof(uint16_t), 1, archive) != 1) {
            free(exit_buffer);
            return false;
        }
        bytes_read += sizeof(uint16_t);

        size_t length = token & 0xF;
        uint16_t value = (token >> 4) & 0xFFF;

        if (length == 0) {
            unsigned char byte = (unsigned char)value;
            if (end >= filesize) {
                free(exit_buffer);
                return false;
            }
            exit_buffer[end++] = byte;
            fwrite(&byte, 1, 1, f_out);
        } else {
            size_t offset = value;
            if (offset == 0 || offset > end) {
                fprintf(stderr, "Ошибка смещения: offset=%zu, end=%zu\n", offset, end);
                free(exit_buffer);
                return false;
            }

            for (size_t i = 0; i < length; i++) {
                if (end >= filesize) {
                    free(exit_buffer);
                    return false;
                }
                size_t src_pos = end - offset;
                unsigned char byte = exit_buffer[src_pos];
                exit_buffer[end++] = byte;
                fwrite(&byte, 1, 1, f_out);
            }
        }
    }
    free(exit_buffer);
    return true;
}