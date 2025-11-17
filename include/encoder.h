#ifndef ENCODER_H
#define ENCODER_H

#include <stdio.h>

/* Константы сжатия */
#define GREEDY    0
#define MAXCOMPARES 75
#define NIL       0xFFFF
#define CHARBITS  8
#define THRESHOLD 2
#define MATCHBITS 4
#define MAXMATCH  ((1 << MATCHBITS) + THRESHOLD - 1)
#define DICTBITS  13
#define HASHBITS  10
#define DICTSIZE  (1 << DICTBITS)
#define HASHSIZE  (1 << HASHBITS)
#define SHIFTBITS ((HASHBITS + THRESHOLD) / (THRESHOLD + 1))
#define SECTORBIT 10
#define SECTORLEN (1 << SECTORBIT)
#define HASHFLAG1 0x8000
#define HASHFLAG2 0x7FFF

/* Глобальные переменные для кодировщика */
extern unsigned char dict[];
extern unsigned int hash[], nextlink[], lastlink[];
extern unsigned int matchlength, matchpos, bitbuf, bitsin;
extern unsigned int masks[];
extern FILE *infile, *outfile;

/* Функции кодировщика */
void InitEncode(void);
unsigned int LoadDict(unsigned int dictpos);
void DeleteData(unsigned int dictpos);
void HashData(unsigned int dictpos, unsigned int bytestodo);
void FindMatch(unsigned int dictpos, unsigned int startlen);
void DictSearch(unsigned int dictpos, unsigned int bytestodo);
void Encode(void);

#endif