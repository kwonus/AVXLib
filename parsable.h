#pragma once
#include <XVMem_platform.h>
#include <vector>

const UINT32 ParsableDefaultMaxLen = 4096;
class parsable
{
public:
	parsable(const char* raw, UINT32 maxLen = ParsableDefaultMaxLen);
	parsable(parsable& str, UINT32 begin, UINT32 end); // substring/slice
	std::vector<parsable*> parse(parsable& str, char delimiter); // substrings/slices

	virtual ~parsable();
	const int len;

	inline operator const char* () const { return this->input; }

protected:
	char* input;
	char* slicable;
	const bool owner;

private:
	//	Use this carefully, because calling the destructor on the owner will invalidate memory of all copies
	parsable(const parsable& str) : len{ str.len }, owner { false }
	{
		this->input = str.input;
		this->slicable = str.slicable;
	}
};