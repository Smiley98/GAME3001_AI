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
}

void PlayScene::GUI_Function()
{
	ImGui::NewFrame();
	ImGui::Begin("GAME3001 - W2023 - Lab 6.1", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );
	ImGui::Separator();

	static bool toggle_grid = false;
	if(ImGui::Checkbox("Toggle Grid", &toggle_grid))
	{
		
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
