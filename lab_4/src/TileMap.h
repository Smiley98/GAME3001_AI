#pragma once
#include <array>
#include <glm/glm.hpp>
#include "Label.h"

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

struct Tile
{
	Label* label;
};

float Manhattan(Cell a, Cell b);
float Euclidean(Cell a, Cell b);
float Cost(TileType type);

constexpr int GRID_SIZE = 10;
class TileMap
{
public:
	Cell GridPosition(glm::vec2 pixelPosition);	// "Quantization" -- pixels to nodes
	glm::vec2 PixelPosition(Cell gridPosition);	// "Localization" -- nodes to pixels

	void Init(Scene* scene);

	void RenderTile(Cell cell, glm::vec3 color);
	void RenderTile(Cell cell, TileType type);
	void Render();

	Cell start{ 1, 8 };
	Cell end{ 8, 1 };

private:
	std::array<std::array<Tile, GRID_SIZE>, GRID_SIZE> m_info;
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
