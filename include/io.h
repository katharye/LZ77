#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef IO_H
#define IO_H

unsigned char* put_file_in_data(const char* filename, size_t* file_size, int* all_okay);

#endif