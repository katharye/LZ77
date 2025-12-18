#include <stdio.h>
#include <stdbool.h>

#ifndef DECODER_H
#define DECODER_H

#define WINDOW_SIZE 200 * (1<<20) 

bool decoder(FILE *archive, FILE *f_out, unsigned long long filesize, long start_file_offset, long end_file_offset);

#endif