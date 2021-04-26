#include <slicable.h>
#include <vector>
#pragma once

struct Feature
{
	char*	feature;
	char	featureType;
	UINT16* featureMatchVector;
};

class CFeature: slicable
{
public:
	CFeature(slicable& feature, UINT32 begin, UINT32 end, char type)
		: slicable(feature, begin, end), featureType{ type }
	{
		this->featureMatchVector = NULL;
	}
	virtual ~CFeature()
	{
		;
	}
	const char	featureType;
	UINT16* featureMatchVector;
};