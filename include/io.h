#ifndef IO_H
#define IO_H

#include <stdio.h>

/* Функции ввода/вывода битов */
void SendBits(unsigned int bits, unsigned int numbits);
unsigned int ReadBits(unsigned int numbits);
void SendMatch(unsigned int matchlen, unsigned int matchdistance);
void SendChar(unsigned int character);

#endif