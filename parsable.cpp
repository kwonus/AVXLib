#include <parsable.h>

parsable::parsable(const char* raw, UINT32 maxLen)
	: len{ raw != NULL ? 1 + Strnlen(raw, maxLen > 0 ? (int)maxLen : 4096) : -1 }
	, owner{ true }
{
	this->input = (char*) malloc(len+1);
	Strncpy(this->input, raw != NULL ? raw : "", (int)this->len);
	this->slicable = len >= 0 ? (char*)malloc(len + 1) : NULL;
	if (this->slicable != NULL)
		Strncpy(this->slicable, raw, (int)this->len);
}

parsable::parsable(parsable& str, UINT32 start, UINT32 cnt) // substring/slice
: len{ (str.slicable != NULL && start < this->len&& start + cnt < this->len + 1) ? str.len : -1 }
, owner{ false }
{
	if (this->len >= 0) {
		this->input = str.input + start;
		this->slicable = str.slicable + start;
		this->slicable[cnt] = '\0';
	}
}

std::vector<parsable*> parsable::parse(parsable& str, char delimiter) // substrings/slices
{
	std::vector<parsable*> results;

	if (str.len >= 0) {
		UINT32 previous = 0;
		for (UINT32 i = 0; i < str.len; i++)
			if (str.input[i] == delimiter) {
				if (previous < i)	// eliminate empty items
					results.push_back(new parsable(str, previous, i - previous));
				previous = i + 1;
			}
	}
	return results;
}
parsable::~parsable()
{
	if (owner) {
		free(this->input);
		if (this->slicable != NULL)
			free(this->slicable);
	}
}