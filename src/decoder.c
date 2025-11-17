#include "decoder.h"
#include "encoder.h"
#include "io.h"
#include <stdlib.h>
#include <stdio.h>

/* main decoder */
void Decode(void)
{
    register unsigned int i, j, k;
    unsigned long bytesdecompressed;

    i = 0;
    bytesdecompressed = 0;

    for ( ; ; )
    {
        if (ReadBits(1) == 0)   /* character or match? */
        {
            dict[i++] = ReadBits(CHARBITS);
            if (i == DICTSIZE)
            {
                if (fwrite(&dict, sizeof (char), DICTSIZE, outfile) == EOF)
                {
                    printf("\nerror writing to output file");
                    exit(EXIT_FAILURE);
                }
                i = 0;
                bytesdecompressed += DICTSIZE;
                printf("\r%ld", bytesdecompressed);
            }
        }
        else
        {
            /* get match length from input stream */
            k = (THRESHOLD + 1) + ReadBits(MATCHBITS);

            if (k == (MAXMATCH + 1))      /* Check for EOF flag */
            {
                if (fwrite(&dict, sizeof (char), i, outfile) == EOF)
                {
                    printf("\nerror writing to output file");
                    exit(EXIT_FAILURE);
                }
                bytesdecompressed += i;
                printf("\r%ld", bytesdecompressed);
                return;
            }

            /* get match position from input stream */
            j = ((i - ReadBits(DICTBITS)) & (DICTSIZE - 1));

            if ((i + k) >= DICTSIZE)
            {
                do
                {
                    dict[i++] = dict[j++];
                    j &= (DICTSIZE - 1);
                    if (i == DICTSIZE)
                    {
                        if (fwrite(&dict, sizeof (char), DICTSIZE, outfile) == EOF)
                        {
                            printf("\nerror writing to output file");
                            exit(EXIT_FAILURE);
                        }
                        i = 0;
                        bytesdecompressed += DICTSIZE;
                        printf("\r%ld", bytesdecompressed);
                    }
                }
                while (--k);
            }
            else
            {
                if ((j + k) >= DICTSIZE)
                {
                    do
                    {
                        dict[i++] = dict[j++];
                        j &= (DICTSIZE - 1);
                    }
                    while (--k);
                }
                else
                {
                    do
                    {
                        dict[i++] = dict[j++];
                    }
                    while (--k);
                }
            }
        }
    }
}