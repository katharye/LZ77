#include "../include/lz77.h"
#include "../include/archive.h"
#include "../include/encode.h"
#include "../include/decode.h"

void ResetCoder(void) {
    bitbuf = 0;
    bitsin = 0;
    matchlength = 0; 
    matchpos = 0;
}

void ArchiveFiles(char *archive_name, int file_count, char *filenames[]) {
    FILE *arch = fopen(archive_name, "wb");
    if (!arch) {
        printf("Не удалось создать архив %s\n", archive_name);
        exit(EXIT_FAILURE);
    }
    outfile = arch;

    printf("Создание архива %s (%d файлов)\n", archive_name, file_count);

    for (int i = 0; i < file_count; i++) {
        ResetCoder();
        
        infile = fopen(filenames[i], "rb");
        if (!infile) {
            printf("Ошибка чтения %s, пропущен.\n", filenames[i]);
            continue;
        }

        fseek(infile, 0, SEEK_END);
        uint32_t fsize = ftell(infile);
        fseek(infile, 0, SEEK_SET);

        FileHeader header;
        header.magic = ARCHIVE_MAGIC;
        header.name_len = strlen(filenames[i]);
        header.original_size = fsize;
        header.compressed_size = 0;

        long header_pos = ftell(arch); 
        fwrite(&header, sizeof(FileHeader), 1, arch);
        fwrite(filenames[i], 1, header.name_len, arch);

        long data_start = ftell(arch);
        printf("Упаковка: %s ... ", filenames[i]);
        
        Encode();
        
        long data_end = ftell(arch);
        header.compressed_size = data_end - data_start;

        fseek(arch, header_pos, SEEK_SET);
        fwrite(&header, sizeof(FileHeader), 1, arch);
        fseek(arch, data_end, SEEK_SET); 

        double ratio = fsize > 0 ? (1.0 - (double)header.compressed_size / fsize) * 100.0 : 0;
        printf("OK (-%.1f%%)\n", ratio);
        
        fclose(infile);
    }

    fclose(arch);
}

void ListFiles(char *archive_name) {
    FILE *arch = fopen(archive_name, "rb");
    if (!arch) {
        printf("Не удалось открыть архив %s\n", archive_name);
        exit(EXIT_FAILURE);
    }

    printf("\nСодержимое архива %s:\n", archive_name);
    printf("-------------------------------------------------------------\n");
    printf(" %-20s | %10s | %10s | %s\n", "Имя файла", "Размер", "Сжато", "Сжатие");
    printf("-------------------------------------------------------------\n");

    FileHeader header;
    uint32_t total_orig = 0, total_comp = 0;
    int count = 0;

    while (fread(&header, sizeof(FileHeader), 1, arch) == 1) {
        if (header.magic != ARCHIVE_MAGIC) {
            printf("Ошибка: Неверный формат архива.\n");
            break;
        }

        char *filename = (char*)malloc(header.name_len + 1);
        if (!filename) {
            printf("Ошибка памяти.\n");
            break;
        }
        
        // ИСПРАВЛЕНИЕ: Проверка результата fread
        if (fread(filename, 1, header.name_len, arch) != header.name_len) {
            printf("Ошибка чтения имени файла.\n");
            free(filename);
            break;
        }
        filename[header.name_len] = '\0';

        double ratio = header.original_size > 0 
            ? (1.0 - (double)header.compressed_size / header.original_size) * 100.0 
            : 0.0;

        printf(" %-20s | %10u | %10u | %5.1f%%\n", 
               filename, header.original_size, header.compressed_size, ratio);

        total_orig += header.original_size;
        total_comp += header.compressed_size;
        count++;

        free(filename);
        fseek(arch, header.compressed_size, SEEK_CUR);
    }

    printf("-------------------------------------------------------------\n");
    double total_ratio = total_orig > 0 
        ? (1.0 - (double)total_comp / total_orig) * 100.0 
        : 0.0;
    printf(" Всего файлов: %d      | %10u | %10u | %5.1f%%\n", count, total_orig, total_comp, total_ratio);

    fclose(arch);
}

void ExtractFiles(char *archive_name, char *target_file) {
    FILE *arch = fopen(archive_name, "rb");
    if (!arch) {
        printf("Не удалось открыть архив %s\n", archive_name);
        exit(EXIT_FAILURE);
    }
    infile = arch;

    FileHeader header;
    int files_extracted = 0;

    while (fread(&header, sizeof(FileHeader), 1, arch) == 1) {
        if (header.magic != ARCHIVE_MAGIC) {
            printf("Ошибка: Неверный формат.\n");
            break;
        }

        char *filename = (char*)malloc(header.name_len + 1);
        // ИСПРАВЛЕНИЕ: Проверка результата fread
        if (fread(filename, 1, header.name_len, arch) != header.name_len) {
            printf("Ошибка чтения имени файла из архива.\n");
            free(filename);
            break;
        }
        filename[header.name_len] = '\0';

        int do_extract = (target_file == NULL) || (strcmp(filename, target_file) == 0);

        if (do_extract) {
            printf("Извлечение: %s ... ", filename);
            ResetCoder();
            
            outfile = fopen(filename, "wb");
            if (!outfile) {
                printf("Ошибка создания %s\n", filename);
                free(filename);
                fseek(arch, header.compressed_size, SEEK_CUR);
                continue;
            }

            long start_pos = ftell(arch);
            Decode();
            long end_pos = ftell(arch);
            
            if ((end_pos - start_pos) != header.compressed_size) {
                 fseek(arch, start_pos + header.compressed_size, SEEK_SET);
            }

            fclose(outfile);
            printf("OK\n");
            files_extracted++;
        } else {
            fseek(arch, header.compressed_size, SEEK_CUR);
        }

        free(filename);
    }

    if (target_file != NULL && files_extracted == 0) {
        printf("Файл '%s' не найден.\n", target_file);
    }

    fclose(arch);
}
