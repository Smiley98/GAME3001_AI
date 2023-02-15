#pragma once
#include <array>
#include <glm/glm.hpp>

enum TileType : int
{
	AIR,
	GRASS,
	WATER,
	MUD,
	COUNT
};

struct Cell
{
	int col; // x
	int row; // y
};

constexpr int GRID_SIZE = 10;
class TileMap
{
public:
	Cell GridPosition(glm::vec2 pixelPosition);	// "Quantization" -- pixels to nodes
	glm::vec2 PixelPosition(Cell gridPosition);	// "Localization" -- nodes to pixels

	void RenderTile(Cell cell, TileType type);
	void Render();

private:
	std::array<std::array<int, GRID_SIZE>, GRID_SIZE> m_tiles
	{
		std::array<int, GRID_SIZE> { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 1, 1, 1, 1, 1, 1, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 0, 0, 3 },
		std::array<int, GRID_SIZE> { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 }
	};
};
