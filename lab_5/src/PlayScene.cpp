#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"

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
	m_map.Render();

	// Target quantization
	Util::DrawCircle(m_pTarget->GetTransform()->position, m_pTarget->GetWidth() * 0.5f);
	Cell targetCell = m_map.GridPosition(m_pTarget->GetTransform()->position);
	m_map.RenderTile(targetCell, MUD);

	// Neighbours render test
	//std::vector<Cell> neighbours = m_Map.Neighbours(m_Map.GridPosition(m_pTarget->GetTransform()->position));
	//for (const Cell& neighbour : neighbours)
	//	m_Map.RenderTile(neighbour, GRASS);

	// A* test!!!
	for (const Cell& cell : m_path)
		m_map.RenderTile(cell, { 1.0f, 0.0f, 0.0f });

	DrawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

extern float total_time;

void PlayScene::Update()
{
	UpdateDisplayList();
	float t = cosf(total_time) * 0.5f + 0.5f;
	m_pTarget->GetTransform()->position = m_map.Lerp(m_map.start, m_map.end, t);
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
	// Set GUI Title
	m_guiTitle = "Play Scene";

	// Add the Target to the Scene
	m_pTarget = new Target(); // instantiate an object of type Target
	m_pTarget->GetTransform()->position = glm::vec2(640.0f, 300.0f);
	AddChild(m_pTarget, 1);

	// Add the StarShip to the Scene
	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = glm::vec2(100.0f, 300.0f);
	m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	m_pStarShip->SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_pStarShip->SetEnabled(false);
	AddChild(m_pStarShip, 2);

	m_map.Init(this);
	m_path = m_map.FindPath();

	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

#include <glm/gtc/type_ptr.hpp>
void PlayScene::GUI_Function()
{
	ImGui::NewFrame();
	ImGui::Begin("GAME3001 - W2023 - Lab4", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );
	ImGui::Separator();

	static bool euclidean = true;
	if (ImGui::Checkbox("Is Euclidean", &euclidean))
	{
		m_map.distanceType = euclidean ? EUCLIDEAN : MANHATTAN;
		m_map.UpdateScores();
	}

	static Cell start = m_map.start;
	if (ImGui::SliderInt2("Start", (int*)&start, 0, GRID_SIZE - 1))
	{
		m_map.start = start;
		m_path = m_map.FindPath();
	}

	static Cell end = m_map.end;
	if (ImGui::SliderInt2("End", (int*)&end, 0, GRID_SIZE - 1))
	{
		m_map.end = end;
		m_path = m_map.FindPath();
	}

	ImGui::End();
}
