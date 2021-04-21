#include <XVMem_platform.h>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <string>

using namespace std;

// trim from start (in place)
static inline void ltrim(std::string& str) {
	size_t startpos = str.find_first_not_of(" \t");
	if (string::npos != startpos)
	{
		str = str.substr(startpos);
	}
}

// trim from end (in place)
static inline void rtrim(std::string& str) {
	size_t endpos = str.find_last_not_of(" \t");
	size_t startpos = str.find_first_not_of(" \t");
	if (std::string::npos != endpos)
	{
		str = str.substr(0, endpos + 1);
		str = str.substr(startpos);
	}
	else {
		str.erase(std::remove(std::begin(str), std::end(str), ' '), std::end(str));
	}
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

// For Part-of-Speech:
UINT32 EncodePOS(char* input7charsMaxWithHyphen) { // input string must be ascii
	auto len = strlen(input7charsMaxWithHyphen);
	if (len < 1 || len > 7)
		return 0;
	auto encoded = (UINT32)0x0;
	auto input = trim_copy(input7charsMaxWithHyphen);
	len = input.length();
	if (len < 1 || len > 7)
		return 0;

	auto hyphen = (UINT32) input.find_first_of("-");
	if (hyphen > 0 && hyphen <= 3)
		hyphen <<= 30;
	else if (len > 6)	// 6 characters max if a compliant hyphen is not part of the string
		return 0;
	else
		hyphen = (UINT32)0x0;

	int c = 0;
	char buffer[6];	// 6x 5bit characters
	for (auto i = 0; i < len; i++) {
		auto b = (BYTE)input[i];
		switch (b) {
			case '-':
				continue;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
				b -= (BYTE)('0');
				b += (BYTE)(27);
		}
		buffer[c++] = tolower((char) b);
	}
	auto position = (UINT32)0x02000000;
	for (auto i = 0; i < 6 - len; i++) {
		position >>= 5;
	}
	for (auto i = 0; i < len; i++) {
		char letter = buffer[i] & 0x1F;
		if (letter == 0)
			break;

		encoded |= (UINT32)letter * position;
		position >>= 5;
	}
	return (UINT32)(encoded | hyphen);
}

//  For Part-of-Speech:
char* DecodePOS(UINT32 encoding) {
	char buffer[7];	// 6x 5bit characters + 2bits for hyphen position = 32 bits;

	auto hyphen = (UINT32)(encoding & 0xC0000000);
	if (hyphen > 0)
		hyphen >>= 30;

	auto index = 0;
	for (auto mask = (UINT32) (0x1F << 25); mask >= 0x1F; mask >>= 5) {
		auto digit = encoding & mask >> (5 * (5 - index));
		if (digit == 0)
			continue;
		BYTE b = (BYTE)digit;
		if (b <= 26)
			b |= 0x60;
		else {
			b -= (BYTE) 27;
			b += (BYTE) '0';
		}
		if (hyphen == index)
			buffer[index++] = '-';
		buffer[index++] = (char) b;
	}
	char* decoded = (char*)malloc((index+1) * sizeof(char));
	for (int i = 0; i < index; i++)
		decoded[i] = buffer[i];
	decoded[index] = 0;
	return decoded;
}
// These are no longer used [created for Z-08 / deprecated in Z-14 ]
#ifdef Z08
UINT16* Encode(char* input3charsMax, int maxSegments) { // input string must be ascii
	auto encoded = (UINT16*)NULL;
	auto ld = strlen(input3charsMax);
	if (ld < 1 || ld > 7) {
		return encoded;
	}
	auto last = ld / 3;
	if (ld % 3 == 0)
		last--;
	int le = last + 1;
	if (le > maxSegments)
		return encoded;

	encoded = (UINT16 *) malloc(le * sizeof(UINT16));
	auto i = 0;
	for (i = 0; i < last; i++) {
		encoded[i] = 0x8000; // overflow-bit
	}
	encoded[last] = 0x0000; // termination flag (no overflow)

	auto onsetLen = ld - (3 * last);
	auto start = 3 - onsetLen;
	auto position = (UINT16)(0x0400);
	if (onsetLen < 3)
		position >>= start * 5;

	i = -1;

	for (auto s = 0; s < le; s++) {
		for (auto z = start; z < 3; z++) {
			i++;
			auto b = (BYTE) tolower(input3charsMax[i]);
			switch (b) {
				case '-':
					b = (BYTE)(27); break;
				case '\'':
					b = (BYTE)(28); break;
				case ',':
					b = (BYTE)(29); break;
				case '.':
				case '!':
					b = (BYTE)(30); break;// no room at the inn
				case '?':
					b = (BYTE)(31); break;
				default:
					b &= (BYTE)(0x1F); break;
			}

			if (b == (BYTE) 0)
				break;

			encoded[s] |= (UINT16) (b * position);
			position >>= 5;
		}
		start = 0;
		position = 0x400;
	}
	return encoded;
}

char* Decode(UINT16* encoded) {
	if (encoded != NULL || encoded[0] != 0) {
		char buffer[128];
		UINT16 mask = 0x1F;
		int c = 0;

		for (auto s = 0; /**/; s++) {
			UINT16 segment = encoded[s];
			for (auto bit = (UINT16)(0x01 << 10); bit > 0; bit >>= 5) {
				UINT16 masked = segment & (bit * mask);
				auto digit = (BYTE)(masked / bit);

				if (digit == 0)
					continue;

				if (digit <= 26) {
					digit |= 0x60;
					buffer[c++] = (char)digit;
				}
				else {
					switch (digit) {
					case 27:
						buffer[c++] = ('-'); break;
					case 28:
						buffer[c++] = ('\''); break;
					case 29:
						buffer[c++] = (','); break;
					case 30:
						buffer[c++] = ('.'); break;
					case 31:
						buffer[c++] = ('?'); break;
					}
				}
			}
			if ((segment & 0x8000) == 0)
				break; // this is okay; overflow not set ... we're done even though the array may be bigger
		}
		char* decoded = (char*)malloc((c + 1) * sizeof(char));
		for (int i = 0; i < c; i++)
			decoded[i] = buffer[i];
		decoded[c] = (char) 0;
		return decoded;
	}
}
UINT16 ArrayLen(UINT16* encoded) {
	UINT16 len = 0;
	if (encoded != NULL) {
		len = 1;
		for (auto segment = encoded; *segment & 0x8000 != 0; segment++)
			len++;
	}
	return len;
}
#endif
