#include "../include/archivator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/decoder.h"
#include "../include/encoder.h"

uint64_t get_file_size(FILE *f) {
    uint64_t pos = ftell(f);
    fseek(f, 0, SEEK_END);
    uint64_t size = ftell(f);
    fseek(f, pos, SEEK_SET);
    return size;
}

int pack_archive(char *out_filename, int argc, char *argv[]) {
    FILE *out = fopen(out_filename, "wb");
    if (!out) return -1;

    Archive main_header;
    main_header.files_cnt = (uint8_t)(argc - 3);
    fwrite(&main_header, sizeof(Archive), 1, out);

    int all_ok = 1;

    for (int i = 3; i < argc; i++) {
        FileHeader header;
        memset(&header, 0, sizeof(FileHeader));

        char *base_name = strrchr(argv[i], '/');
        if (base_name) {
            strncpy((char *)header.filename, base_name + 1, 255);
        } else {
            strncpy((char *)header.filename, argv[i], 255);
        }

        FILE *in = fopen(argv[i], "rb");
        if (!in) {
            printf("Error opening input file: %s\n", argv[i]);
            continue;
        }
        header.filesize = get_file_size(in);
        fclose(in);

        size_t size = 0;
        uint16_t *encoded_data = encoder(argv[i], &size, &all_ok);

        if (!all_ok || !encoded_data) {
            printf("Error compressing file: %s\n", argv[i]);
            if (encoded_data) free(encoded_data);
            continue;
        }

        header.encoded_filesize = (uint64_t)(size * sizeof(uint16_t));

        fwrite(&header, sizeof(FileHeader), 1, out);
        fwrite(encoded_data, sizeof(uint16_t), size, out);

        free(encoded_data);
    }
    fclose(out);
    return 0;
}

int unpack_archive(char *archive_filename, int argc, char *argv[]) {
    FILE *archive = fopen(archive_filename, "rb");
    if (!archive) return -1;

    Archive main_header;
    if (fread(&main_header, sizeof(Archive), 1, archive) != 1) {
        fclose(archive);
        return -1;
    }

    char copy[256];
    strncpy(copy, archive_filename, sizeof(copy) - 1);
    copy[sizeof(copy) - 1] = '\0';
    char *dot = strchr(copy, '.');
    if (dot && dot != copy) *dot = '\0';

    mkdir(copy, 0777);

    for (int i = 0; i < main_header.files_cnt; i++) {
        FileHeader header;
        if (fread(&header, sizeof(FileHeader), 1, archive) != 1) {
            fclose(archive);
            return -1;
        }

        if (argc > 3) {
            int needed_file = 0;
            for (int arg = 3; arg < argc; arg++)
                if (strcmp(header.filename, argv[arg]) == 0) {
                    needed_file = 1;
                    break;
                }

            if (!needed_file) {
                fseek(archive, header.encoded_filesize, SEEK_CUR);
                continue;
            }
        }

        long file_pos = ftell(archive);

        char file_path[256];
        snprintf(file_path, sizeof(file_path), "%s/%s", copy, header.filename);

        FILE *file = fopen(file_path, "wb");
        if (!decoder(archive, file, (unsigned long long)header.filesize, file_pos,
                     file_pos + header.encoded_filesize)) {
            fclose(archive);
            fclose(file);
            return -1;
        }
        fclose(file);

        fseek(archive, file_pos + header.encoded_filesize, SEEK_SET);
    }
    fclose(archive);
    return 0;
}

void print_archive(char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("Incorrect file!");
        return;
    }

    Archive main_header;
    size_t result = fread(&main_header, sizeof(Archive), 1, file);
    if (result != 1) {
        printf("Incorrect file!");
        return;
    }

    printf("\nArchive content:\n");
    printf("%-35s | %12s | %12s | %s\n", "File Name", "Original", "Compressed", "Ratio");
    printf("------------------------------------|--------------|--------------|-------\n");

    for (int i = 0; i < main_header.files_cnt; i++) {
        FileHeader header;
        size_t result = fread(&header, sizeof(FileHeader), 1, file);
        if (result != 1) {
            printf("Incorrect file!");
            return;
        }
        int ratio = header.encoded_filesize * 100 / header.filesize;
        printf("%-35s | %12llu | %12llu | %d%%\n", header.filename, (unsigned long long)header.filesize,
               (unsigned long long)header.encoded_filesize, ratio);

        fseek(file, header.encoded_filesize, SEEK_CUR);
    }
}
