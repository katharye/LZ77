#include "cli.h"
#include "encoder.h"
#include "decoder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Глобальные переменные для файлов */
FILE *infile, *outfile;

int process_command_line(int argc, char *argv[])
{
    char *s;

    if (argc != 4)
    {
        printf("\n'prog2 e file1 file2' encodes file1 into file2.\n"
               "'prog2 d file2 file1' decodes file2 into file1.\n");
        return EXIT_FAILURE;
    }
    
    if ((s = argv[1], s[1] || strpbrk(s, "DEde") == NULL)
     || (s = argv[2], (infile  = fopen(s, "rb")) == NULL)
     || (s = argv[3], (outfile = fopen(s, "wb")) == NULL)) {
        printf("??? %s\n", s);
        return EXIT_FAILURE;
    }

    /* allocate 4k I/O buffers */
    setvbuf(infile, NULL, _IOFBF, 4096);
    setvbuf(outfile, NULL, _IOFBF, 4096);

    if (toupper(*argv[1]) == 'E')
    {
        printf("Compressing %s to %s\n", argv[2], argv[3]);
        Encode();
    }
    else
    {
        printf("Decompressing %s to %s\n", argv[2], argv[3]);
        Decode();
    }

    fclose(infile);
    fclose(outfile);

    return EXIT_SUCCESS;
}