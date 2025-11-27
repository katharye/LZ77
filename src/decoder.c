#include <stdio.h>
#include <stdbool.h>

typedef struct {
    size_t offset;
    size_t length;
    unsigned char next_char;
} Token;

#define WINDOW_SIZE 4096

unsigned char exit_buffer[WINDOW_SIZE];
size_t end = 0;

bool decoder(FILE *encoded_file, FILE *f_out) {
    Token current_token = {0, 0, 0};

    size_t bytes_read;
    while (1) {
        bytes_read = fread(&current_token.offset, 1, sizeof(size_t), encoded_file);
        if (bytes_read != sizeof(size_t)) break;

        bytes_read = fread(&current_token.length, 1, sizeof(size_t), encoded_file);
        if (bytes_read != sizeof(size_t)) return false;

        bytes_read = fread(&current_token.next_char, 1, sizeof(unsigned char), encoded_file);
        if (bytes_read != sizeof(unsigned char)) return false;

        if (current_token.length == 0 && current_token.offset == 0) {
            exit_buffer[end++] = current_token.next_char;
            printf("done");
        } else {
            unsigned char* copy_area = &exit_buffer[end - current_token.offset];
            for (size_t i = 0; i < current_token.length; i++) {
                exit_buffer[end++] = copy_area[i];
            }
            exit_buffer[end++] = current_token.next_char;
            printf("done");
        }


        printf("<%zu, %zu, %c>\n", current_token.offset, current_token.length, current_token.next_char);
        
    }

        printf("%s", exit_buffer);
    return true;
}

int main() {
    FILE *file_in = fopen("test.lz77", "rb");
    FILE *file_out = fopen("test_out.txt", "wb");
    
    decoder(file_in, file_out);
    
    fclose(file_in);
    return 0;
}