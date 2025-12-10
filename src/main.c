#include "../include/lz77.h"
#include "../include/archive.h"

// Определение глобальных переменных
unsigned char dict[DICTSIZE + MAXMATCH];
unsigned int hash[HASHSIZE], nextlink[DICTSIZE + 1];
unsigned int matchlength, matchpos;
unsigned int bitbuf = 0, bitsin = 0;
unsigned int masks[17] = {
    0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535
};
FILE *infile, *outfile;

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Использование:\n");
        printf("  prog1 pack archive file1 ... (Создать архив)\n");
        printf("  prog1 list archive           (Показать содержимое)\n");
        printf("  prog1 unpack archive [file]  (Извлечь всё или файл)\n");
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "pack") == 0) {
        ArchiveFiles(argv[2], argc - 3, &argv[3]);
    }
    else if (strcmp(argv[1], "list") == 0) {
        ListFiles(argv[2]);
    }
    else if (strcmp(argv[1], "unpack") == 0) {
        char *target = (argc >= 4) ? argv[3] : NULL;
        ExtractFiles(argv[2], target);
    }
    else {
        printf("Неизвестная команда: %s\n", argv[1]);
        printf("Доступные команды: pack, list, unpack\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
