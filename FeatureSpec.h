#include <XVMem_platform.h>
#include <FeatureMatch.h>
#include <slicable.h>
#include <vector>
#pragma once

struct FeatureSpec
{
	char*			specification;
	FeatureMatch*	matchAny;
};

class CFeatureSpec : slicable
{
public:
	std::vector<CFeatureMatch*>	matchAll;
	CFeatureSpec(slicable& str, UINT32 begin, UINT32 end) : slicable(str, begin, end)
	{
		if (this->len >= 0) {
			UINT32 previous = 0;
			for (UINT32 i = 0; i < this->len; i++)
				if (this->input[i] == '&') {
					if (previous < i)	// eliminate empty items
						matchAll.push_back(new CFeatureMatch(*this, previous, i - previous));
					previous = i + 1;
				}
		}
	}
	virtual ~CFeatureSpec()
	{
		;
	}
};