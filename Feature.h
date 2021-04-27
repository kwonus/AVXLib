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
		this->featureVector = NULL;
	}
	virtual ~CFeature()
	{
		;
	}
	const char	featureType;
	inline const UINT32* getFeatureVector()
	{
		if (featureVector == NULL) {
			;
		}
		return featureVector;
	}
private:
	UINT32* featureVector;
};