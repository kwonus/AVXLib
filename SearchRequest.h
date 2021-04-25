#include <SearchClause.h>
#include <SearchControls.h>
#pragma once

struct SearchRequest
{
	SearchClause*	clauses;
	SearchControls	controls;
	UINT64			count;
};