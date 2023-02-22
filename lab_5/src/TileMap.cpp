#include "TileMap.h"
#include "Game.h"
#include "Util.h"
#include <queue>

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

glm::vec2 TileMap::Lerp(Cell a, Cell b, float t)
{
	return Util::Lerp(PixelPosition(a), PixelPosition(b), t);
}

void TileMap::Init(Scene* scene)
{
	const int tileWidth = Game::Instance().width / GRID_SIZE;
	const int tileHeight = Game::Instance().height / GRID_SIZE;
	for (int row = 0; row < GRID_SIZE; row++)
	{
		for (int col = 0; col < GRID_SIZE; col++)
		{
			glm::vec2 labelPosition = PixelPosition({ col, row });
			labelPosition.x += tileWidth * 0.5f;
			labelPosition.y += tileHeight * 0.5f;

			Label* label = new Label("", "Consolas", 12, {255, 0, 0, 255}, labelPosition);
			m_info[row][col].label = label;
		}
	}
	UpdateScores();
}

void TileMap::RenderTile(Cell cell, glm::vec3 color)
{
	const int tileWidth = Game::Instance().width / GRID_SIZE;
	const int tileHeight = Game::Instance().height / GRID_SIZE;
	glm::vec2 tilePosition{ cell.col * tileWidth, cell.row * tileHeight };
	Util::DrawFilledRect(tilePosition, tileWidth, tileHeight, glm::vec4(color, 1.0f));
	m_info[cell.row][cell.col].label->Draw();
}

void TileMap::RenderTile(Cell cell, TileType type)
{
	glm::vec3 tileColor;
	switch (type)
	{
	case AIR:
		tileColor = { 1.0f, 1.0f, 1.0f };
		break;

	case GRASS:
		tileColor = { 0.0f, 1.0f, 0.0f };
		break;

	case WATER:
		tileColor = { 0.0f, 0.0f, 1.0f };
		break;

	case MUD:
		tileColor = { 0.5f, 0.15f, 0.25f };
		break;
	}
	RenderTile(cell, tileColor);
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

	RenderTile(start, { 0.5f, 0.0f, 1.0f });
	RenderTile(end, { 1.0f, 0.0f, 1.0f });
}

void TileMap::UpdateScores()
{
	for (int row = 0; row < GRID_SIZE; row++)
	{
		for (int col = 0; col < GRID_SIZE; col++)
		{
			float g = distanceType == EUCLIDEAN ? Euclidean({ col, row }, end) : Manhattan({ col, row }, end);
			float h = Cost((TileType)m_tiles[row][col]);
			std::string text = std::to_string(g + h);
			text.resize(4);
			m_info[row][col].label->SetText("H: " + text);
		}
	}
}

Path TileMap::FindPath()
{
	const int tileCount = GRID_SIZE * GRID_SIZE;
	std::vector<Node> tileNodes(tileCount);
	std::priority_queue<Node, std::vector<Node>, decltype(&Compare)> openList(Compare);
	std::vector<bool> closedList(tileCount, false);
	tileNodes[Index(start)].parent = start;
	openList.push({ start });

	while (!openList.empty())
	{
		const Cell currentCell = openList.top().cell;

		// End condition (destination reached)
		if (currentCell == end)
		{
			break;
		}

		// Continue with A* by popping from the front of the queue and adding it to the close list so we don't re-visit it!
		openList.pop();
		closedList[Index(currentCell)] = true;

		float gNew, hNew;
		for (const Cell& neighbour : Neighbours(currentCell))
		{
			const int neighbourIndex = Index(neighbour);

			// Skip if already visited
			if (closedList[neighbourIndex])
				continue;

			gNew = distanceType == EUCLIDEAN ? Euclidean(currentCell, neighbour) : Manhattan(currentCell, neighbour);
			hNew = distanceType == EUCLIDEAN ? Euclidean(neighbour, end) : Manhattan(neighbour, end);
			hNew += Cost((TileType)m_tiles[neighbour.row][neighbour.col]);

			// Append if unvisited or best score
			if (tileNodes[neighbourIndex].F() <= FLT_EPSILON || (gNew + hNew) < tileNodes[neighbourIndex].F())
			{
				openList.push({ neighbour, gNew, hNew });
				tileNodes[neighbourIndex] = { neighbour, currentCell, gNew, hNew };
			}
		}
	}

	Path path;
	Cell currentCell = end;
	int currentIndex = Index(currentCell);

	if (tileNodes[currentIndex].parent == Cell{ -1, -1 })
		return {};

	while (!(tileNodes[currentIndex].parent == currentCell))
	{
		path.push_back(currentCell);
		currentCell = tileNodes[currentIndex].parent;
		currentIndex = Index(currentCell);
	}
	std::reverse(path.begin(), path.end());

	return path;
}

std::vector<Cell> TileMap::Neighbours(Cell cell)
{
	std::vector<Cell> neighbours;
	for (int row = cell.row - 1; row <= cell.row + 1 && row >= 0 && row < GRID_SIZE; row++)
	{
		for (int col = cell.col - 1; col <= cell.col + 1 && col >= 0 && col < GRID_SIZE; col++)
		{
			Cell neighbour{ col, row };
			if (!(neighbour == cell))
				neighbours.push_back(neighbour);
		}
	}
	return neighbours;
}

float Manhattan(Cell a, Cell b)
{
	return abs(b.col - a.col) + abs(b.row - a.row);
}

float Euclidean(Cell a, Cell b)
{
	float dx = b.col - a.col;
	float dy = b.row - a.row;
	return sqrtf(dx * dx + dy * dy);
}

float Cost(TileType type)
{
	static std::array<float, COUNT> costs
	{
		0.0f,	// Air
		10.0f,	// Grass
		20.0f,	// Water
		50.0f	// Mud
	};
	return costs[type];
}

bool Compare(const Node& a, const Node& b)
{
	return a.F() > b.F();
}

bool operator==(const Cell& a, const Cell& b)
{
	return a.row == b.row && a.col == b.col;
}
