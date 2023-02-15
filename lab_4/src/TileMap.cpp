#include "TileMap.h"
#include "Game.h"
#include "Util.h"

Cell TileMap::GridPosition(glm::vec2 pixelPosition)
{
	const int tileWidth = Game::Instance().width / GRID_SIZE;	// 80
	const int tileHeight = Game::Instance().height / GRID_SIZE;	// 60
	return { (int)pixelPosition.x / tileWidth, (int)pixelPosition.y / tileHeight };
}

glm::vec2 TileMap::PixelPosition(Cell gridPosition)
{
	const int tileWidth = Game::Instance().width / GRID_SIZE;
	const int tileHeight = Game::Instance().height / GRID_SIZE;
	return { gridPosition.col * tileWidth, gridPosition.row * tileHeight };
}

void TileMap::RenderTile(Cell cell, TileType type)
{
	const int tileWidth = Game::Instance().width / GRID_SIZE;
	const int tileHeight = Game::Instance().height / GRID_SIZE;
	glm::vec2 tilePosition{ cell.col * tileWidth, cell.row * tileHeight };
	glm::vec4 tileColor;

	switch (type)
	{
	case AIR:
		tileColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		break;

	case GRASS:
		tileColor = { 0.0f, 1.0f, 0.0f, 1.0f };
		break;

	case WATER:
		tileColor = { 0.0f, 0.0f, 1.0f, 1.0f };
		break;

	case MUD:
		tileColor = { 0.5f, 0.15f, 0.25f, 1.0f };
		break;
	}

	Util::DrawFilledRect(tilePosition, tileWidth, tileHeight, tileColor);
}

void TileMap::Render()
{
	for (int row = 0; row < GRID_SIZE; row++)
	{
		for (int col = 0; col < GRID_SIZE; col++)
		{
			RenderTile({ col, row }, (TileType)m_tiles[row][col]);
		}
	}
}