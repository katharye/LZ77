#ifndef LZ77_H
#define LZ77_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Константы настройки сжатия
#define MAXCOMPARES 32
#define NIL 0xFFFF
#define CHARBITS 8
#define THRESHOLD 2 
#define MATCHBITS 4
#define MAXMATCH ((1 << MATCHBITS) + THRESHOLD - 1)
#define DICTBITS 13
#define HASHBITS 12     
#define DICTSIZE (1 << DICTBITS)
#define HASHSIZE (1 << HASHBITS)
#define SECTORBIT 10
#define SECTORLEN (1 << SECTORBIT)
#define SECTORAND ((0xFFFF << SECTORBIT) & 0xFFFF)
#define ARCHIVE_MAGIC 0x4C5A5241 // Сигнатура "ARZL"

// Структура заголовка файла
#pragma pack(push, 1)
typedef struct {
    uint32_t magic;           
    uint16_t name_len;        
    uint32_t original_size;   
    uint32_t compressed_size; 
} FileHeader;
#pragma pack(pop)

// Глобальные переменные (определены в main.c)
extern unsigned char dict[DICTSIZE + MAXMATCH];
extern unsigned int hash[HASHSIZE], nextlink[DICTSIZE + 1];
extern unsigned int matchlength, matchpos;
extern unsigned int bitbuf, bitsin;
extern unsigned int masks[17];
extern FILE *infile, *outfile;

#endif // LZ77_H
