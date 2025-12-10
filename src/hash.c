#include "../include/lz77.h"
#include "../include/hash.h"
#include "../include/encode.h" // Для SendChar и SendMatch

static inline unsigned int HashFunction(unsigned int pos)
{
    unsigned int h = ((dict[pos] << 4) ^ dict[pos + 1]) & (HASHSIZE - 1);
    return h;
}

void HashData(unsigned int dictpos, unsigned int bytestodo)
{
    if (bytestodo <= THRESHOLD)
    {
        for (unsigned int i = 0; i < bytestodo; i++)
            nextlink[dictpos + i] = NIL;
    }
    else
    {
        unsigned int endpos = dictpos + bytestodo - THRESHOLD;
        for (unsigned int i = dictpos; i < endpos; i++)
        {
            unsigned int h = HashFunction(i);
            nextlink[i] = hash[h];
            hash[h] = i;
        }
        for (unsigned int i = endpos; i < dictpos + bytestodo; i++)
            nextlink[i] = NIL;
    }
}

void FindMatch(unsigned int dictpos, unsigned int startlen)
{
    matchlength = startlen;
    matchpos = NIL;

    unsigned char c = dict[dictpos + matchlength];
    unsigned int limit = MAXCOMPARES;

    unsigned int curr = nextlink[dictpos];
    while (curr != NIL && limit--)
    {
        if (dict[curr + matchlength] == c)
        {
            unsigned int j = 0;
            while (j < MAXMATCH && dict[dictpos + j] == dict[curr + j]) j++;

            if (j > matchlength)
            {
                matchlength = j;
                matchpos = curr;
                if (j == MAXMATCH) break;
                c = dict[dictpos + matchlength];
            }
        }
        curr = nextlink[curr];
    }
}

void DictSearch(unsigned int dictpos, unsigned int bytestodo)
{
    unsigned int i = dictpos;
    unsigned int bytesleft = bytestodo;

    while (bytesleft > 0)
    {
        FindMatch(i, THRESHOLD);

        if (matchlength > THRESHOLD)
        {
            unsigned int bestlen = matchlength;
            unsigned int bestpos = matchpos;

            while (1)
            {
                // Жадный поиск: проверяем, не будет ли лучше совпадение на следующей позиции
                FindMatch(i + 1, bestlen);
                if (matchlength > bestlen)
                {
                    SendChar(dict[i]);
                    i++;
                    bytesleft--;
                    bestlen = matchlength;
                    bestpos = matchpos;
                }
                else
                {
                    if (bestlen > bytesleft)
                        bestlen = bytesleft;
                    if (bestlen <= THRESHOLD)
                    {
                        SendChar(dict[i]);
                        i++;
                        bytesleft--;
                        break;
                    }
                    SendMatch(bestlen, (i - bestpos) & (DICTSIZE - 1));
                    i += bestlen;
                    bytesleft -= bestlen;
                    break;
                }
            }
        }
        else
        {
            SendChar(dict[i]);
            i++;
            bytesleft--;
        }
    }
}
