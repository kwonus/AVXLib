#include <SearchFragment.h>
#pragma once

struct SearchClause
{
	SearchFragment*	fragments;
	char*			segment;
	char			polarity;

};