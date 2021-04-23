#ifndef AVX_HEADER
#define AVX_HEADER
#include <XVMem.h>
#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */


extern "C"
{
using AVWrit = struct AVWritDX5
{
    UINT64 srclang;
    UINT16 verseIdx;
    UINT16 wordKey;
    BYTE punc;
    BYTE transition;
    UINT16 pnwc;
    UINT32 pos;
};

const BYTE NAME_LEN = 16;
const BYTE ABBR_LEN = 12;
using AVBook = struct AVBookIX8
{
    BYTE book;
    BYTE chapterCnt;
    UINT16 chapterIdx;
//		char name[16];
//		char abbreviations[12];
    UINT64 name;
    UINT64 name_part2;
    UINT64 abbreviations;
    UINT32 abbreviations_part2;
};

using AVChapter = struct AVChapterIX2
{
    UINT32 writIdx;
    UINT16 verseIdx;
    UINT16 wordCnt;
};

using AVVerse = struct AVVerseIX1
{
    BYTE book;
    BYTE chapter;
    BYTE verse;
    BYTE wordCnt;
};

using AVLemma = struct AVLemmaDXI
{
    UINT32 pos;
    UINT16 wordKey;
    UINT16 wordClass;
    UINT16 lemmaCnt;
    UINT16* lemma;
};

using AVLemmaOOV = struct AVLemmaOovDXI
{
    UINT16 oovKey;
    char lemma;
};

using AVLexicon = struct AVLexiconDXI
{
    UINT16 entities;
    UINT16 posCnt;
    UINT32* pos;
//		char* search;
//		char* display;
//		char* modern;
};

using AVWordClass = struct AVWordClassDXI
{
    UINT16 wordClass;
    UINT16 width;
    UINT32* pos;
};

using AVName = struct AVNameDXI
{
    UINT16 wordKey;
    BYTE   meanings;
};

void initialize(char* folder);
void release();
UINT32 getWritCnt();
UINT16 getBookCnt();
UINT32 getChapterCnt();
UINT32 getVerseCnt();
AVWrit* getWrit(UINT32 idx);
AVBook& getBook(UINT16 idx);
AVBook& getBookByNum(UINT16 num);
AVChapter& getChapter(UINT16 idx);
AVVerse& getVerse(UINT16 idx);
const BYTE SEARCH = 1;	// sequence
const BYTE DISPLAY = 2;	// sequence
const BYTE MODERN = 3;	// sequence
char* getLexicalEntry(UINT16 key, BYTE sequence);
UINT16 getLemma(UINT32 pos, UINT16 wkey, char* data[], UINT16 arrayLen);

}
#pragma pack(pop)   /* restore original alignment from stack */
#endif