#include <Feature.h>
#pragma once

struct FeatureMatch
{
	char*	condition;
	Feature	features[];
};