#include "../include/lz77.h"
#include "../include/encode.h"
#include "../include/hash.h"

void SendBits(unsigned int bits, unsigned int numbits)
{
    bitbuf |= (bits << bitsin);
    bitsin += numbits;

    while (bitsin >= 8)
    {
        if (putc(bitbuf & 0xFF, outfile) == EOF)
        {
            printf("\nОшибка записи");
            exit(EXIT_FAILURE);
        }
        bitbuf >>= 8;
        bitsin -= 8;
    }
}

void SendMatch(unsigned int matchlen, unsigned int matchdistance)
{
    SendBits(1, 1);
    SendBits(matchlen - (THRESHOLD + 1), MATCHBITS);
    SendBits(matchdistance, DICTBITS);
}

void SendChar(unsigned int character)
{
    SendBits(0, 1);
    SendBits(character, CHARBITS);
}

void InitEncode(void)
{
    for (unsigned int i = 0; i < HASHSIZE; i++) hash[i] = NIL;
    nextlink[DICTSIZE] = NIL;
}

unsigned int LoadDict(unsigned int dictpos)
{
    unsigned int read_bytes = fread(&dict[dictpos], sizeof(char), SECTORLEN, infile);
    if (read_bytes == 0 && ferror(infile))
    {
        printf("\nОшибка чтения");
        exit(EXIT_FAILURE);
    }
    if (dictpos == 0)
    {
        memcpy(&dict[DICTSIZE], &dict[0], MAXMATCH);
    }
    return read_bytes;
}

void DeleteData(unsigned int dictpos)
{
    unsigned int sector = dictpos & SECTORAND;
    for (unsigned int i = 0; i < DICTSIZE; i++)
    {
        if ((nextlink[i] & SECTORAND) == sector) nextlink[i] = NIL;
    }
    for (unsigned int i = 0; i < HASHSIZE; i++)
    {
        if ((hash[i] & SECTORAND) == sector) hash[i] = NIL;
    }
}

void Encode(void)
{
    InitEncode();

    unsigned int dictpos = 0;
    int deleteflag = 0;

    while (1)
    {
        if (deleteflag) DeleteData(dictpos);

        unsigned int sectorlen = LoadDict(dictpos);
        if (sectorlen == 0) break;

        HashData(dictpos, sectorlen);
        DictSearch(dictpos, sectorlen);

        dictpos += SECTORLEN;
        if (dictpos == DICTSIZE)
        {
            dictpos = 0;
            deleteflag = 1;
        }
    }

    SendMatch(MAXMATCH + 1, 0); 
    if (bitsin) SendBits(0, 8 - bitsin); 
}
