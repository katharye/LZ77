#ifndef IO_H
#define IO_H

unsigned char* put_file_in_data(const char* filename, size_t* file_size, int* all_okay);
int put_data_in_file(FILE* file, uint16_t length, uint16_t value);

#endif