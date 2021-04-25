#include <TokenFeature.h>
#pragma once

struct TokenMatch
{
	char*			condition;
	TokenFeature	anyFeature[];
};