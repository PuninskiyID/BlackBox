#include <vector>
#include <deque>
#include <string>
#include <stdint.h>

namespace str
{
	std::vector<std::string> split (const std::string &s, char delim);
    void ReadLines(std::deque<char*>&lines, char *fileData, size_t fileSize);
}

namespace Bits
{
	uint64_t signedToBits(int64_t _signed, unsigned int pos, unsigned int len);
	int64_t bitsToSigned(uint64_t bits, unsigned int pos, unsigned int len);
	int64_t bitsToSigned(uint64_t bits, unsigned int len);
	uint64_t reverseBits(uint64_t bits, unsigned int len);
}