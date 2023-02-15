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
	m_Map.Render();

	Util::DrawCircle(m_pTarget->GetTransform()->position, m_pTarget->GetWidth() * 0.5f);
	m_Map.RenderTile(m_Map.GridPosition(m_pTarget->GetTransform()->position), MUD);

	Util::DrawRect(m_pStarShip->GetTransform()->position - glm::vec2(m_pStarShip->GetWidth() * 0.5f, m_pStarShip->GetHeight() * 0.5f), m_pStarShip->GetWidth(), m_pStarShip->GetHeight());
	//CollisionManager::RotateAABB(m_pStarShip, m_pStarShip->GetCurrentHeading());

	DrawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

extern float total_time;

void PlayScene::Update()
{
	UpdateDisplayList();
	float t = cosf(total_time) * 0.5f + 0.5f;
	m_pTarget->GetTransform()->position = m_Map.Lerp(m_Map.start, m_Map.end, t);
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

	m_Map.Init(this);

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

	static glm::vec2 targetPosition;
	if (ImGui::SliderFloat2("Target Position", glm::value_ptr(targetPosition), 0.0f, 800.0f))
	{
		m_pTarget->GetTransform()->position = targetPosition;
	}

	static bool euclidean = true;
	if (ImGui::Checkbox("Is Euclidean", &euclidean))
	{
		m_Map.distanceType = euclidean ? EUCLIDEAN : MANHATTAN;
		m_Map.UpdateScores();
	}

	static Cell start = m_Map.start;
	if (ImGui::SliderInt2("Start", (int*)&start, 0, GRID_SIZE - 1))
	{
		m_Map.start = start;
		m_Map.UpdateScores();
	}

	static Cell end = m_Map.end;
	if (ImGui::SliderInt2("End", (int*)&end, 0, GRID_SIZE - 1))
	{
		m_Map.end = end;
		m_Map.UpdateScores();
	}

	ImGui::End();
}
