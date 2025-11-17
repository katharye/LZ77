#include "encoder.h"
#include "io.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Глобальные переменные */
unsigned char dict[DICTSIZE + MAXMATCH];
unsigned int hash[HASHSIZE];
unsigned int nextlink[DICTSIZE];
unsigned int lastlink[DICTSIZE];
unsigned int matchlength, matchpos, bitbuf, bitsin;
unsigned int masks[17] = {0,1,3,7,15,31,63,127,255,511,1023,2047,4095,8191,16383,32767,65535};

/* initializes the search structures needed for compression */
void InitEncode(void)
{
    register unsigned int i;

    for (i = 0; i < HASHSIZE; i++) hash[i] = NIL;
    nextlink[DICTSIZE] = NIL;
}

/* loads dictionary with characters from the input stream */
unsigned int LoadDict(unsigned int dictpos)
{
    register unsigned int i, j;

    if ((i = fread(&dict[dictpos], sizeof (char), SECTORLEN, infile)) == EOF)
    {
        printf("\nerror reading from input file");
        exit(EXIT_FAILURE);
    }

    /* since the dictionary is a ring buffer, copy the characters at
       the very start of the dictionary to the end */
    if (dictpos == 0)
    {
        for (j = 0; j < MAXMATCH; j++) dict[j + DICTSIZE] = dict[j];
    }

    return i;
}

/* deletes data from the dictionary search structures */
void DeleteData(unsigned int dictpos)
{
    register unsigned int i, j, k;

    /* delete all references to the sector being deleted */
    k = dictpos + SECTORLEN;

    for (i = dictpos; i < k; i++)
    {
        if ((j = lastlink[i]) & HASHFLAG1)
        {
            if (j != NIL) hash[j & HASHFLAG2] = NIL;
        }
        else
            nextlink[j] = NIL;
    }
}

/* hash data just entered into dictionary */
void HashData(unsigned int dictpos, unsigned int bytestodo)
{
    register unsigned int i, j, k;

    if (bytestodo <= THRESHOLD)   /* not enough bytes in sector for match? */
        for (i = 0; i < bytestodo; i++)
        nextlink[dictpos + i] = lastlink[dictpos + i] = NIL;
    else
    {
        /* matches can't cross sector boundries */
        for (i = bytestodo - THRESHOLD; i < bytestodo; i++)
            nextlink[dictpos + i] = lastlink[dictpos + i] = NIL;

        j = (((unsigned int)dict[dictpos]) << SHIFTBITS) ^ dict[dictpos + 1];

        k = dictpos + bytestodo - THRESHOLD;  /* calculate end of sector */

        for (i = dictpos; i < k; i++)
        {
            lastlink[i] = (j = (((j << SHIFTBITS) & (HASHSIZE - 1)) ^ dict[i + THRESHOLD])) | HASHFLAG1;
            if ((nextlink[i] = hash[j]) != NIL) lastlink[nextlink[i]] = i;
            hash[j] = i;
        }
    }
}

/* finds match for string at position dictpos */
void FindMatch(unsigned int dictpos, unsigned int startlen)
{
    register unsigned int i, j, k;
    unsigned char l;

    i = dictpos; matchlength = startlen; k = MAXCOMPARES;
    l = dict[dictpos + matchlength];

    do
    {
        if ((i = nextlink[i]) == NIL) return;   /* get next string in list */

        if (dict[i + matchlength] == l)        /* possible larger match? */
        {
            for (j = 0; j < MAXMATCH; j++)          /* compare strings */
                if (dict[dictpos + j] != dict[i + j]) break;

            if (j > matchlength)  /* found larger match? */
            {
                matchlength = j;
                matchpos = i;
                if (matchlength == MAXMATCH) return;  /* exit if largest possible match */
                l = dict[dictpos + matchlength];
            }
        }
    }
    while (--k);  /* keep on trying until we run out of chances */
}

/* finds dictionary matches for characters in current sector */
void DictSearch(unsigned int dictpos, unsigned int bytestodo)
{
    register unsigned int i, j;

#if (GREEDY == 0)
    unsigned int matchlen1, matchpos1;

    /* non-greedy search loop (slow) */
    i = dictpos; j = bytestodo;

    while (j) /* loop while there are still characters left to be compressed */
    {
        FindMatch(i, THRESHOLD);

        if (matchlength > THRESHOLD)
        {
            matchlen1 = matchlength;
            matchpos1 = matchpos;

            for ( ; ; )
            {
                FindMatch(i + 1, matchlen1);

                if (matchlength > matchlen1)
                {
                    matchlen1 = matchlength;
                    matchpos1 = matchpos;
                    SendChar(dict[i++]);
                    j--;
                }
                else
                {
                    if (matchlen1 > j)
                    {
                        matchlen1 = j;
                        if (matchlen1 <= THRESHOLD) { SendChar(dict[i++]); j--; break; }
                    }

                    SendMatch(matchlen1, (i - matchpos1) & (DICTSIZE - 1));
                    i += matchlen1;
                    j -= matchlen1;
                    break;
                }
            }
        }
        else
        {
            SendChar(dict[i++]);
            j--;
        }
    }

#else
    /* greedy search loop (fast) */
    i = dictpos; j = bytestodo;

    while (j) /* loop while there are still characters left to be compressed */
    {
        FindMatch(i, THRESHOLD);

        if (matchlength > j) matchlength = j;     /* clamp matchlength */

        if (matchlength > THRESHOLD)  /* valid match? */
        {
            SendMatch(matchlength, (i - matchpos) & (DICTSIZE - 1));
            i += matchlength;
            j -= matchlength;
        }
        else
        {
            SendChar(dict[i++]);
            j--;
        }
    }
#endif
}

/* main encoder */
void Encode(void)
{
    unsigned int dictpos, deleteflag, sectorlen;
    unsigned long bytescompressed;

    InitEncode();
    dictpos = deleteflag = 0;
    bytescompressed = 0;

    while (1)
    {
        /* delete old data from dictionary */
        if (deleteflag) DeleteData(dictpos);

        /* grab more data to compress */
        if ((sectorlen = LoadDict(dictpos)) == 0) break;

        /* hash the data */
        HashData(dictpos, sectorlen);

        /* find dictionary matches */
        DictSearch(dictpos, sectorlen);

        bytescompressed += sectorlen;
        printf("\r%ld", bytescompressed);

        dictpos += SECTORLEN;

        /* wrap back to beginning of dictionary when its full */
        if (dictpos == DICTSIZE)
        {
            dictpos = 0;
            deleteflag = 1;   /* ok to delete now */
        }
    }

    /* Send EOF flag */
    SendMatch(MAXMATCH + 1, 0);

    /* Flush bit buffer */
    if (bitsin) SendBits(0, 8 - bitsin);

    return;
}