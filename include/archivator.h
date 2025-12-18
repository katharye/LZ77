#ifndef ARCHIVATOR_H
#define ARCHIVATOR_H

#include <stdint.h>

#pragma pack(push, 1)

typedef struct {
   uint8_t files_cnt;
} Archive;

typedef struct {
    char filename[256];
    
    uint64_t filesize;
    uint64_t encoded_filesize;

    
} FileHeader;
#pragma pack(pop)

    int pack_archive(char* out_filename, int argc, char *argv[ ]);
    int unpack_archive(char* archive_filename, int argc, char *argv[ ]);
    void print_archive(char* filename);

#endif