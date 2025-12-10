#ifndef ENCODE_H
#define ENCODE_H

void InitEncode(void);
void Encode(void);
void SendBits(unsigned int bits, unsigned int numbits);
void SendMatch(unsigned int matchlen, unsigned int matchdistance);
void SendChar(unsigned int character);
unsigned int LoadDict(unsigned int dictpos);
void DeleteData(unsigned int dictpos);

#endif // ENCODE_H
