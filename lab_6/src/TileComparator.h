#pragma once
#ifndef __TILE_COMPARATOR__
#define __TILE_COMPARATOR__

#include "Tile.h"

class TileComparator
{
public:
	int operator() (const Tile* lhs, const Tile* rhs) const
	{
		return lhs->GetTileCost() > rhs->GetTileCost();
	}
};

#endif /* defined (__TILE_COMPARATOR__) */