#pragma once
#include <array>
#include <vector>
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

enum DistanceType
{
	MANHATTAN,
	EUCLIDEAN
};

struct Cell
{
	int col = - 1; // x
	int row = - 1; // y
};

struct Node
{
	Node()
	{
		Init();
	}

	Node(Cell cell)
	{
		Init(cell);
	}

	Node(Cell cell, float g, float h)
	{
		Init(cell, {}, g, h);
	}

	Node(Cell cell, Cell parent, float g, float h)
	{
		Init(cell, parent, g, h);
	}

	void Init(Cell cell = {}, Cell parent = {}, float g = 0.0f, float h = 0.0f)
	{
		this->cell = cell;
		this->parent = parent;
		this->g = g;
		this->h = h;
	}

	float F() const { return g + h; }

	Cell cell;
	Cell parent;
	float g;
	float h;
};

struct Tile
{
	Label* label;
};

using Path = std::vector<Cell>;

float Manhattan(Cell a, Cell b);
float Euclidean(Cell a, Cell b);
float Cost(TileType type);
bool Compare(const Node& a, const Node& b);

constexpr int GRID_SIZE = 10;
class TileMap
{
public:
	Cell GridPosition(glm::vec2 pixelPosition);	// "Quantization" -- pixels to nodes
	glm::vec2 PixelPosition(Cell gridPosition);	// "Localization" -- nodes to pixels
	glm::vec2 Lerp(Cell a, Cell b, float t);	// Interpolates between tile centres as pixels
	int Index(Cell cell) { return cell.row * GRID_SIZE + cell.col; }

	void Init(Scene* scene);

	void RenderTile(Cell cell, glm::vec3 color);
	void RenderTile(Cell cell, TileType type);
	void Render();

	void UpdateScores();

	Path FindPath();

	Cell start{ 1, 8 };
	Cell end{ 8, 1 };
	DistanceType distanceType = EUCLIDEAN;

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
