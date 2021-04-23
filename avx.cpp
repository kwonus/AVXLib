#include "avx.h"
#include "fivebitencoding.h"
#include <unordered_map>

using namespace std;

// Uses shared memory
static XVMem<AVWrit> Writ;
static XVMem<BYTE> Lexicon;
static XVMem<BYTE> Lemma;
static XVMem<BYTE> LemmaOOV;

// Uses memory on the heap
XVMem<AVBook> allocAVBook;
XVMem<AVChapter> allocAVChapter;
XVMem<AVVerse> allocAVVerse;
XVMem<BYTE> allocAVName;
XVMem<BYTE> allocAVWordClass;


static AVWrit*	 writ;		// [1+0xC0C93];
static AVBook*	 books;		// [66];
static AVChapter*chapters;	// [1+0x4A4];
static AVVerse*	 verses;	// [1+0x797D];
// Maps
static std::unordered_map<UINT64, AVLemma*> lemma;
static std::unordered_map<UINT16, char*> lemmaOOV;
static std::unordered_map<UINT16, AVLexicon*> lexicon;
static std::unordered_map<UINT16, AVWordClass*> wclass;
static std::unordered_map<UINT16, AVName*> names;

#define AVTEXT		"AV-Writ.dx5"
#define AVLEXICON	"AV-Lexicon.dxi"
#define AVLEMMA		"AV-Lemma.dxi"
#define AVLEMMAOOV	"AV-Lemma-OOV.dxi"
#define AVBOOK		"AV-Book.ix8"
#define AVCHAPTER	"AV-Chapter.ix2"
#define AVVERSE		"AV-Verse.ix1"
#define AVNAMES		"AV-Names.dxi"
#define AVWCLASS	"AV-WordClass.dxi"

// Only works on intel-byte-order
//
inline UINT16 uint16(BYTE* buffer) {
    return *((UINT16*)buffer);
}
// Only works on intel-byte-order
//
inline UINT32 uint32(BYTE* buffer) {
    return *((UINT32*)buffer);
}
// Only works on intel-byte-order
//
inline UINT64 uint64(BYTE* buffer) {
    return *((UINT64*)buffer);
}
extern "C" void release()
{
    lemma.clear();
    lemmaOOV.clear();
    lexicon.clear();
    wclass.clear();
    names.clear();

    Writ.Release();
    Lexicon.Release();
    Lemma.Release();
    LemmaOOV.Release();
    allocAVBook.Release();
    allocAVChapter.Release();
    allocAVVerse.Release();
    allocAVName.Release();
    allocAVWordClass.Release();

    writ = NULL;
    books = NULL;
    chapters = NULL;
    verses = NULL;
}
extern "C" UINT32 getWritCnt()
{
    return Writ.GetCnt(); // 1+0xC0C93
}
extern "C" UINT16 getBookCnt()
{
    return allocAVBook.GetCnt(); // 66;
}
extern "C" UINT32 getChapterCnt()
{
    return allocAVChapter.GetCnt(); // 1+0x4A4
}
extern "C" UINT32 getVerseCnt()
{
    return allocAVVerse.GetCnt(); // 1+0x797D
}
///
extern "C" AVWrit* getWrit(UINT32 idx)
{
    return idx < Writ.GetCnt() ? writ + idx : NULL;
}
extern "C" AVBook& getBook(UINT16 idx)
{
    return idx < allocAVBook.GetCnt() ? books[idx] : books[allocAVBook.GetCnt() - 1];
}
extern "C" AVBook& getBookByNum(UINT16 num)
{
    return num > 0 ? getBook(num-1) : books[0];
}
extern "C" AVChapter& getChapter(UINT16 idx)
{
    return idx < allocAVChapter.GetCnt() ? chapters[idx] : chapters[allocAVChapter.GetCnt()-1];
}
extern "C" AVVerse& getVerse(UINT16 idx)
{
    return idx < allocAVVerse.GetCnt() ? verses[idx] : verses[allocAVVerse.GetCnt()-1];
}
// These are the only variable length entries other than pos.  Record can be used directly for other firelds.
char* getLexicalEntry(UINT16 rawkey, BYTE sequence)	// seq=0:search; seq=1:display; seq=2:modern;
{
    UINT16 key = rawkey & 0x3FFF;
    if (key < 1 || key > 12567)
        return NULL;

    auto lex = lexicon.at(key);
    auto entries = (char*)lex;
    char* search = entries + (sizeof(UINT16) + sizeof(UINT16) + (lex->posCnt * sizeof(UINT32)));

    if (sequence == SEARCH)
        return search;

    char* display = search + 1 + strlen(search);

    if (sequence == DISPLAY)
        return (*display != (char)0) ? display : search;

    char* modern = display + 1 + strlen(display);

    if (sequence == MODERN)
        return (*modern != (char)0) ? modern : (*display != (char)0) ? display : search;

    return NULL;
}
extern "C" UINT16 getLemma(UINT32 pos, UINT16 wkey, char* data[], UINT16 arrayLen)
{
    UINT64 hashKey = (((UINT64)pos) << 32) + wkey;
    AVLemma* record = lemma.at(hashKey);

    if (record != NULL) {
        if (arrayLen < 1 || data == NULL)
            return record->lemmaCnt;

        BYTE* location = (BYTE*)(&record->lemma);
        UINT16 i;
        for (i = 0; i < record->lemmaCnt; i++, location += sizeof(UINT16)) {
            char* lemma;
            UINT16 key = uint16(location);
            if ((key & 0x8000) == 0x8000)   // this lemma is OOV
            {
                lemma = lemmaOOV[key];
            }
            else
            {
                bool modernized = (key & 0x4000) == 0x4000;
                lemma = !modernized ? getLexicalEntry(key, DISPLAY) : getLexicalEntry(key &0x7FFF, MODERN);
            }
            if (i < arrayLen)
                data[i] = lemma;
        }
        for (UINT16 r = i + 1; r < arrayLen; r++)
            data[i] = NULL;
        return record->lemmaCnt;
    }
    return 0;
}
///

extern "C" void initialize(char * folder)
{
    if (folder != NULL)
        Strncpy(g_hSharedHome, folder, MAX_PATH);
    else
        g_hSharedHome[0] = (char)0;

    writ = Writ.Acquire(AVTEXT, false, true);
    books = allocAVBook.Acquire(AVBOOK, false, false);
    chapters = allocAVChapter.Acquire(AVCHAPTER, false, false);
    verses = allocAVVerse.Acquire(AVVERSE, false, false);

    // Process Lexicon
    UINT16 lexnum = 1;
    {
        BYTE* lex = Lexicon.Acquire(AVLEXICON, false, true);
        int cnt = Lexicon.GetCnt(); // last insertion of lexnum should finish be 12567 (size/cnt is MUCH bigger)
        BYTE* last = lex + cnt - 1;

        for (lexnum = 1; lex <= last && ((UINT32*)lex)[1] != 0xFFFFFFFF; lexnum ++) {
            auto record = (AVLexicon*) lex;
            lexicon.insert({ lexnum, record });
            // add slot for entities
            lex += sizeof(UINT16);
            // add slot for size (POS)
            lex += sizeof(UINT16);
            // add slot for each POS
            lex += (sizeof(UINT32) * record->posCnt);
            // Get counts for search, modern, and search ..
            for (BYTE x = 1; x <= MODERN; x++)
                lex += (1 + strlen((char*)lex));
        }
    }
    // Process AVLemma
    {
        BYTE* lemm = Lemma.Acquire(AVLEMMA, false, true);
        int bcnt = Lemma.GetCnt();
        BYTE* last = lemm + bcnt - 1; // last UINT32 (+8 for previous record) of file are sizing data; and ignored here)

        UINT64 hashKey;
        for (int i = 0; lemm <= last && uint32(lemm) != 0xFFFFFFFF; i++) {
            auto record = (AVLemma*) lemm;

            hashKey = ((UINT64)(record->pos) << 32) + (UINT64)(record->wordKey);
            lemma.insert({ hashKey, record });

            // add slot for pos
            lemm += sizeof(UINT32);
            // add slot for wordkey
            lemm += sizeof(UINT16);
            // add slot for wordClass
            lemm += sizeof(UINT16);
            // add slot for Lemma Count and all lemma segment
            lemm += sizeof(UINT16);
            for (UINT16 x = 0; x < record->lemmaCnt; x++) {
                UINT16 key = uint16(lemm);
                lemm += sizeof(UINT16);
            }
        }
    }
    // Process AVLemmaOOV
    {
        BYTE* lemmOOV = LemmaOOV.Acquire(AVLEMMAOOV, false, true);
        int bcnt = LemmaOOV.GetCnt();
        BYTE* last = lemmOOV + bcnt - 1; // last UINT32 (+8 for previous record) of file are sizing data; and ignored here)

        UINT64 hashKey;
        UINT16 len = 0;
        for (/**/; lemmOOV < last; lemmOOV += (sizeof(UINT16) + len)) {
            auto record = (AVLemmaOOV*)lemmOOV;
            lemmaOOV.insert({ record->oovKey, &(record->lemma) });
            if (record->oovKey == 0x8F01)
                break;
            len = 1 + (record->oovKey & 0x0F00) >> 8;
        }
    }
    // Process AVNames
    {
        auto bytes = allocAVName;
        AVName* record;
        BYTE* data = bytes.Acquire(AVNAMES, false, false);
        int bcnt = bytes.GetCnt();
        for (auto end = data + bcnt; data < end; /**/) {
            record = (AVName*)data;
            auto nameKey = uint16(data);
            data += sizeof(UINT16);
            for (/**/; *data != '\0'; data++)
                ;
            data++;	// skip the null terminator
        }
    }
    // Process AVWordClass
    {
        auto bytes = allocAVWordClass;

        AVWordClass* record;
        BYTE* data = bytes.Acquire(AVWCLASS, false, false);
        int bcnt = bytes.GetCnt();

        for (auto end = data + bcnt; data < end; /**/) {
            record = (AVWordClass*)data;
            wclass.insert({ record->wordClass, record });
            data += 2 * sizeof(UINT16);
            data += record->width * sizeof(UINT32);
        }
    }
}