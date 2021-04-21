#ifndef ENCODING_5_BIT_H 
#define ENCODING_5_BIT_H 

#include <XVMem_platform.h> 

UINT32 EncodePOS(char* input7charsMaxWithHyphen); // input string must be ascii (max 6 chars; or 7 with hyphen in position 1 through 3)
char* DecodePOS(UINT32 encoding);
//UINT16* Encode(char* input3charsMax, int maxSegments); // input string must be ascii
//char* Decode(UINT16* encoded);

#endif
