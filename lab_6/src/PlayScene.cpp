#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include <windows.h>

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	DrawDisplayList();

	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();
	CheckShipLOS(m_pTarget);
}

void PlayScene::Clean()
{
	RemoveAllChildren();
}

void PlayScene::HandleEvents()
{
	EventManager::Instance().Update();

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_ESCAPE))
	{
		Game::Instance().Quit();
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_1))
	{
		Game::Instance().ChangeSceneState(SceneState::START);
	}

	if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_2))
	{
		Game::Instance().ChangeSceneState(SceneState::END);
	}
}

void PlayScene::Start()
{
	m_guiTitle = "Play Scene";

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);
	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));

	m_pTarget = new Target();
	m_pTarget->GetTransform()->position = { 600.0f, 300.0f };
	AddChild(m_pTarget);

	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = { 150.0f, 300.0f };
	AddChild(m_pStarShip);

	BuildObstaclePool();

	m_pObstacles[0]->GetTransform()->position = { 380.0f, 80.0f };
	m_pObstacles[0]->SetHeight(50);

	m_pObstacles[1]->GetTransform()->position = { 380.0f, 280.0f };
	m_pObstacles[1]->SetWidth(100);

	m_pObstacles[2]->GetTransform()->position = { 380.0f, 480.0f };

	BuildGrid();
}

void PlayScene::GUI_Function()
{
	ImGui::NewFrame();
	ImGui::Begin("GAME3001 - W2023 - Lab 6.1", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );
	ImGui::Separator();

	if (ImGui::Checkbox("Toggle Grid", &m_isGridEnabled))
	{
		ToggleGrid(m_isGridEnabled);
	}

	// StarShip Properties
	static int shipPosition[] = { static_cast<int>(m_pStarShip->GetTransform()->position.x),
		static_cast<int>(m_pStarShip->GetTransform()->position.y) };
	if (ImGui::SliderInt2("Ship Position", shipPosition, 0, 800))
	{
		m_pStarShip->GetTransform()->position.x = static_cast<float>(shipPosition[0]);
		m_pStarShip->GetTransform()->position.y = static_cast<float>(shipPosition[1]);
	}

	// allow the ship to rotate
	static int angle;
	if (ImGui::SliderInt("Ship Direction", &angle, -360, 360))
	{
		m_pStarShip->SetCurrentHeading(static_cast<float>(angle));
	}

	ImGui::Separator();

	// Target Properties
	static int targetPosition[] = { static_cast<int>(m_pTarget->GetTransform()->position.x),
		static_cast<int>(m_pTarget->GetTransform()->position.y) };
	if (ImGui::SliderInt2("Target Position", targetPosition, 0, 800))
	{
		m_pTarget->GetTransform()->position.x = static_cast<float>(targetPosition[0]);
		m_pTarget->GetTransform()->position.y = static_cast<float>(targetPosition[1]);
	}

	ImGui::Separator();

	// Add Obstacle position control for each obstacle
	for (unsigned i = 0; i < m_pObstacles.size(); ++i)
	{
		int obstaclePosition[] = { static_cast<int>(m_pObstacles[i]->GetTransform()->position.x),
		static_cast<int>(m_pObstacles[i]->GetTransform()->position.y) };
		std::string label = "Obstacle" + std::to_string(i + 1) + " Position";
		if (ImGui::SliderInt2(label.c_str(), obstaclePosition, 0, 800))
		{
			m_pObstacles[i]->GetTransform()->position.x = static_cast<float>(obstaclePosition[0]);
			m_pObstacles[i]->GetTransform()->position.y = static_cast<float>(obstaclePosition[1]);
			BuildGrid();
		}
	}

	ImGui::Separator();
	ImGui::End();
}

void PlayScene::BuildObstaclePool()
{
	for (int i = 0; i < 3; i++)
	{
		m_pObstacles.push_back(new Obstacle);
		AddChild(m_pObstacles[i]);
	}
}

void PlayScene::CheckShipLOS(DisplayObject* object)
{
	m_pStarShip->SetHasLOS(false);

	float range = Util::GetClosestEdge(m_pStarShip->GetTransform()->position, object);
	if (range <= m_pStarShip->GetLOSDistance())
	{
		std::vector<DisplayObject*> contacts;
		for (DisplayObject* contact : GetDisplayList())
		{
			if (contact->GetType() == GameObjectType::PATH_NODE) continue;
			if (contact->GetType() != m_pStarShip->GetType() && contact->GetType() != object->GetType())
			{
				float contactRange = Util::GetClosestEdge(m_pStarShip->GetTransform()->position, contact);
				if (contactRange <= range)
				{
					contacts.push_back(contact);
				}
			}
		}

		bool hasLOS = CollisionManager::LOSCheck(m_pStarShip,
			m_pStarShip->GetTransform()->position + m_pStarShip->GetCurrentDirection() * m_pStarShip->GetLOSDistance(),
			contacts, object);
		m_pStarShip->SetHasLOS(hasLOS);
		m_pStarShip->SetLOSColour(hasLOS ? glm::vec4{0.0f, 1.0f, 0.0f, 1.0f} : glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
	}
}

void PlayScene::ToggleGrid(bool isVisible)
{
	for (PathNode* node : m_pGrid)
	{
		node->SetVisible(isVisible);
	}
}

void PlayScene::BuildGrid()
{
	// Ensure we've destroyed the previous grid before building a new grid
	ClearNodes();

	const int tileSize = Config::TILE_SIZE;
	const int tileOffset = tileSize * 0.5;

	for (int row = 0; row < Config::ROW_NUM; row++)
	{
		for (int col = 0; col < Config::COL_NUM; col++)
		{
			PathNode* node = new PathNode;
			node->GetTransform()->position = { col * tileSize + tileOffset, row * tileSize + tileOffset };

			bool keep = true;
			for (Obstacle* obstacle : m_pObstacles)
			{
				if (CollisionManager::AABBCheck(node, obstacle))
				{
					keep = false;
				}
			}
			if (keep)
			{
				AddChild(node);
				m_pGrid.push_back(node);
			}
			else
			{
				delete node;
			}
		}
	}
	ToggleGrid(m_isGridEnabled);
}

void PlayScene::ClearNodes()
{
	for (PathNode* node : m_pGrid)
	{
		RemoveChild(node);
	}
	m_pGrid.clear();
}
