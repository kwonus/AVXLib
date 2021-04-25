#include <FeatureSpec.h>
#pragma once

struct SearchFragment
{
	UINT32*			positionAspects;
	FeatureSpec*	spec;	// spec is "All Of" features in the specification
	char*			text;
};