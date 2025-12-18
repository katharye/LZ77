#include <stdio.h>
#include <string.h>

#include "../include/archivator.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s p <archive name> <files> - Pack archive\n", argv[0]);
        printf("Usage: %s u <archive name>         - Unpack ALL files from archive\n", argv[0]);
        printf("Usage: %s u <archive name> <files> - Unpack files from archive\n", argv[0]);
        printf("Usage: %s c <archive name>         - Check archive\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "p") == 0) {
        if (pack_archive(argv[2], argc, argv) == -1) {
            printf("Error with packing to archive\n");
            return 1;
        }
    } else if (strcmp(argv[1], "u") == 0) {
        if (unpack_archive(argv[2], argc, argv) == -1) {
            printf("Error with unpacking to archive\n");
            return 1;
        }
    } else if (strcmp(argv[1], "c") == 0) {
        print_archive(argv[2]);
    }

    return 0;
}