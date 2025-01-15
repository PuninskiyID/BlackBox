#include "utils.h"
#include <sstream>

std::vector<std::string> str::split (const std::string &s, char delim) 
{
    std::vector<std::string> result;
    std::stringstream ss (s);
    std::string item;

    while (std::getline (ss, item, delim)) {
        result.push_back (item);
    }

    return result;
}

void str::ReadLines(std::deque<char*>&lines, char *fileData, size_t fileSize)
{
	if(fileData == NULL) return;
	lines.clear();
	char* p = fileData; /*pointer*/
	unsigned int s = 0; /*state*/
		
	for( ; fileSize; fileSize--, p++ )
	{
		if(s)
		{
			if(*p != '\n' && *p != '\r' && *p != 0) continue;
			*p = 0; s = 0; continue; 
		}
		else 
		{
		   if(*p == '\n' || *p == '\r' || *p == 0) { *p = 0; continue; } 
		   s = 1; lines.push_back(p); continue;
		}
	}
}

uint64_t Bits::signedToBits(int64_t _signed, unsigned int pos, unsigned int len)
{
	return ((uint64_t)(_signed & (-1ULL >> (64 - len))) & (-1ULL >> (64 - len - 1))) << pos;
}

int64_t Bits::bitsToSigned(uint64_t bits, unsigned int pos, unsigned int len)
{
	return ((bits & (1ULL << (pos + len - 1)))? -1LL << len : 0LL) | 
		((bits >> pos) & (-1ULL >> (64 - len - 1)));
}

int64_t Bits::bitsToSigned(uint64_t bits, unsigned int len)
{
	return ((bits & (1ULL << (len - 1)))? -1LL << len : 0LL) | 
		(bits & (-1ULL >> (64 - len - 1)));
}

uint64_t Bits::reverseBits(uint64_t bits, unsigned int len)
{
	uint64_t rev = bits & 1;
	for( bits >>= 1; len; --len, bits >>= 1 )
	{
		rev <<= 1;
		rev |= bits & 1;
	}
	return rev;
}
