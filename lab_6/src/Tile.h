#pragma once
#ifndef __TILE__
#define __TILE__

#include "NavigationObject.h"
#include "TileStatus.h"
#include "Label.h"
#include "NeighbourTile.h"

class Tile : public NavigationObject
{
public:
    // Constructor
    Tile();

    // Destructor
    ~Tile();

    // Life-Cycle Functions
    void Draw() override;
    void Update() override;
    void Clean() override;

    // Getters (Accessors) and Setters (Mutators)
    [[nodiscard]] Tile* GetNeighbourTile(NeighbourTile position) const;
    void SetNeighbourTile(NeighbourTile position, Tile* tile);
    std::vector<Tile*> GetNeighbourTiles();

    [[nodiscard]] float GetTileCost() const;
    void SetTileCost(float cost);

    [[nodiscard]] TileStatus GetTileStatus() const;
    void SetTileStatus(TileStatus status);

    [[nodiscard]] Tile* GetTileParent() const;
    void SetTileParent(Tile* tile_parent);

    // Utility Functions
    void AddLabels();
    void SetLabelsEnabled(bool state) const;

private:
    // private instance members (fields)
    float m_cost;
    TileStatus m_status;

    Label* m_costLabel;
    Label* m_statusLabel;

    Tile* m_pTileParent;

    std::vector<Tile*> m_neighbours;
    
};
#endif /* defined (__TILE__) */

