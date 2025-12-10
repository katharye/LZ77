#include "../include/lz77.h"
#include "../include/decode.h"

unsigned int ReadBits(unsigned int numbits)
{
    unsigned int i = bitbuf;

    while (bitsin < numbits)
    {
        int c = getc(infile);
        if (c == EOF)
        {
            printf("\nОшибка: Неожиданный конец данных");
            exit(EXIT_FAILURE);
        }
        i |= (c << bitsin);
        bitsin += 8;
    }

    bitbuf = i >> numbits;
    bitsin -= numbits;

    return i & masks[numbits];
}

void Decode(void)
{
    unsigned int i = 0;
    while (1)
    {
        if (ReadBits(1) == 0)
        {
            dict[i++] = ReadBits(CHARBITS);
            if (i == DICTSIZE)
            {
                if (fwrite(&dict, sizeof(char), DICTSIZE, outfile) != DICTSIZE)
                {
                    printf("\nОшибка записи при распаковке");
                    exit(EXIT_FAILURE);
                }
                i = 0;
            }
        }
        else
        {
            unsigned int len = (THRESHOLD + 1) + ReadBits(MATCHBITS);
            if (len == (MAXMATCH + 1)) 
            {
                if (fwrite(&dict, sizeof(char), i, outfile) != i)
                {
                    printf("\nОшибка записи при распаковке");
                    exit(EXIT_FAILURE);
                }
                return;
            }

            unsigned int pos = (i - ReadBits(DICTBITS)) & (DICTSIZE - 1);

            for (unsigned int k = 0; k < len; k++)
            {
                dict[i] = dict[pos];
                i = (i + 1) & (DICTSIZE - 1);
                pos = (pos + 1) & (DICTSIZE - 1);

                if (i == 0)
                {
                    if (fwrite(&dict, sizeof(char), DICTSIZE, outfile) != DICTSIZE)
                    {
                        printf("\nОшибка записи при распаковке");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
    }
}
