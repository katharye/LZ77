#include "io.h"
#include "encoder.h"
#include <stdlib.h>
#include <stdio.h>

/* writes multiple bit codes to the output stream */
void SendBits(unsigned int bits, unsigned int numbits)
{
    bitbuf |= (bits << bitsin);
    bitsin += numbits;

    if (bitsin > 16)         /* special case when # bits in buffer exceeds 16 */
    {
        if (putc(bitbuf & 0xFF, outfile) == EOF)
        {
            printf("\nerror writing to output file");
            exit(EXIT_FAILURE);
        }
        bitbuf = bits >> (8 - (bitsin - numbits));
        bitsin -= 8;
    }

    while (bitsin >= 8)
    {
        if (putc(bitbuf & 0xFF, outfile) == EOF)
        {
            printf("\nerror writing to output file");
            exit(EXIT_FAILURE);
        }
        bitbuf >>= 8;
        bitsin -= 8;
    }
}

/* reads multiple bit codes from the input stream */
unsigned int ReadBits(unsigned int numbits)
{
    register unsigned int i;

    i = bitbuf >> (8 - bitsin);

    while (numbits > bitsin)
    {
        if ((bitbuf = getc(infile)) == EOF)
        {
            printf("\nerror reading from input file");
            exit(EXIT_FAILURE);
        }
        i |= (bitbuf << bitsin);
        bitsin += 8;
    }

    bitsin -= numbits;
    return (i & masks[numbits]);
}

/* sends a match to the output stream */
void SendMatch(unsigned int matchlen, unsigned int matchdistance)
{
    SendBits(1, 1);
    SendBits(matchlen - (THRESHOLD + 1), MATCHBITS);
    SendBits(matchdistance, DICTBITS);
}

/* sends one character (or literal) to the output stream */
void SendChar(unsigned int character)
{
    SendBits(0, 1);
    SendBits(character, CHARBITS);
}