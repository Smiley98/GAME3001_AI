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
	Cell GridPosition(glm::vec2 pixelPosition);
	glm::vec2 PixelPosition(Cell gridPosition);

	void RenderTile(Cell cell, TileType type);
	void Render();

private:
	std::array<std::array<int, GRID_SIZE>, GRID_SIZE> m_tiles
	{
		std::array<int, GRID_SIZE> { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 },
		std::array<int, GRID_SIZE> { 3, 1, 1, 1, 1, 1, 1, 1, 1, 3 },
		std::array<int, GRID_SIZE> { 3, 1, 1, 1, 1, 1, 1, 1, 1, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 2, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 2, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 2, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 2, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 2, 3 },
		std::array<int, GRID_SIZE> { 3, 0, 0, 0, 0, 0, 0, 2, 2, 3 },
		std::array<int, GRID_SIZE> { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3 }
	};
};

