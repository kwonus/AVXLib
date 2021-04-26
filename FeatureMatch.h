#include <Feature.h>
#include <slicable.h>
#include <vector>
#pragma once

struct FeatureMatch
{
	char*	condition;
	Feature	features[];
};

class CFeatureMatch : slicable
{
public:
	std::vector<CFeature*>	features;

	CFeatureMatch(slicable& str, UINT32 begin, UINT32 end) : slicable(str, begin, end)
	{
		if (this->len >= 0) {
			UINT32 previous = 0;
			for (UINT32 i = 0; i < this->len; i++)
				if (this->input[i] == '&') {
					if (previous < i)	// eliminate empty items
						features.push_back(new CFeature(*this, previous, i - previous, char(0)));
					previous = i + 1;
				}
		}
	}

	virtual ~CFeatureMatch()
	{
		;
	}
};