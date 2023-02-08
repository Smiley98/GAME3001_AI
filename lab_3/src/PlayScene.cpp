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
	DrawDisplayList();
	if(m_bDebugView)
	{
		// Draw Collider Bounds for the Target
		Util::DrawCircle(m_pTarget->GetTransform()->position, m_pTarget->GetWidth() * 0.5f);

		if(m_pStarShip->IsEnabled())
		{
			Util::DrawRect(m_pStarShip->GetTransform()->position -
				glm::vec2(m_pStarShip->GetWidth() * 0.5f, m_pStarShip->GetHeight() * 0.5f),
				m_pStarShip->GetWidth(), m_pStarShip->GetHeight());

			CollisionManager::RotateAABB(m_pStarShip, m_pStarShip->GetCurrentHeading());
		}
	}

	// Previous solution -- single raycast directly ahead
	//Util::DrawLine(m_pStarShip->GetTransform()->position,
	//	m_pStarShip->GetTransform()->position + m_pStarShip->GetCurrentDirection() * m_pStarShip->GetLOSDistance());

	// Updated solution - double raycast to each size
	Util::DrawLine(m_pStarShip->GetTransform()->position,
		m_pStarShip->GetTransform()->position + Util::Direction(m_pStarShip->GetCurrentHeading() - 15.0f) * m_pStarShip->GetLOSDistance());

	Util::DrawLine(m_pStarShip->GetTransform()->position,
		m_pStarShip->GetTransform()->position + Util::Direction(m_pStarShip->GetCurrentHeading() + 15.0f) * m_pStarShip->GetLOSDistance());

	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();

	//if (CollisionManager::AABBCheck(m_pObstacle, m_pStarShip))
	//{
	//	SoundManager::Instance().PlaySound("boom");
	//}

	if(m_pStarShip->IsEnabled())
	{
		CollisionManager::CircleAABBCheck(m_pTarget, m_pStarShip);
	}
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
	m_bDebugView = false; // turn off debug colliders

	// Add the Target to the Scene
	m_pTarget = new Target(); // instantiate an object of type Target
	m_pTarget->GetTransform()->position = glm::vec2(500.0f, 100.0f);
	AddChild(m_pTarget);

	m_pObstacle = new Obstacle();
	//m_pObstacle-
	AddChild(m_pObstacle);

	// Add the StarShip to the Scene
	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = glm::vec2(100.0f, 400.0f);
	m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	m_pStarShip->SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_pStarShip->SetEnabled(false);
	m_pStarShip->SetLOSDistance(200.0f);
	m_pStarShip->pObstacle = m_pObstacle;
	AddChild(m_pStarShip);

	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "boom", SoundType::SOUND_SFX);

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

#include <GLM/gtc/type_ptr.hpp>

void PlayScene::GUI_Function()
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("GAME3001 - W2023 - Lab2", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar );

	ImGui::Separator();

	// Debug Properties
	static bool toggleDebug = false;
	if(ImGui::Checkbox("Toggle Debug View", &toggleDebug))
	{
		m_bDebugView = toggleDebug;
	}

	ImGui::Separator();

	// Obstacle position slider
	static glm::vec2 obstaclePosition;
	if (ImGui::SliderFloat2("Obstacle Position", glm::value_ptr(obstaclePosition), 0.0f, 800.0f))
	{
		m_pObstacle->GetTransform()->position = obstaclePosition;
	}

	// Target Properties
	static float position[2] = { m_pTarget->GetTransform()->position.x,
		m_pTarget->GetTransform()->position.y };
	if(ImGui::SliderFloat2("Target Position", position, 0.0f, 800.0f))
	{
		m_pTarget->GetTransform()->position = glm::vec2(position[0], position[1]);
		m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	}

	ImGui::Separator();

	// StarShip Properties
	static bool toggleSeek = m_pStarShip->IsEnabled();
	if(ImGui::Checkbox("Toggle Seek", &toggleSeek))
	{
		m_pStarShip->SetEnabled(toggleSeek);
	}

	ImGui::Separator();

	static float acceleration = m_pStarShip->GetAccelerationRate();
	if(ImGui::SliderFloat("Acceleration Rate", &acceleration, 0.0f, 50.0f))
	{
		m_pStarShip->SetAccelerationRate(acceleration);
		m_pStarShip->GetRigidBody()->acceleration = 
			m_pStarShip->GetCurrentDirection() * m_pStarShip->GetAccelerationRate();
	}

	ImGui::Separator();

	static float turn_rate = m_pStarShip->GetTurnRate();
	if (ImGui::SliderFloat("Turn Rate", &turn_rate, 0.0f, 20.0f))
	{
		m_pStarShip->SetTurnRate(turn_rate);
	}

	ImGui::Separator();
	if(ImGui::Button("Reset"))
	{
		// reset StarShip's Position
		m_pStarShip->GetTransform()->position = glm::vec2(100.0f, 400.0f);

		// reset  the Target's Position
		m_pTarget->GetTransform()->position = glm::vec2(500.0f, 100.0f);

		// Reset current Heading (orientation)
		m_pStarShip->SetCurrentHeading(0.0f);
		m_pStarShip->SetCurrentDirection(glm::vec2(1.0f, 0.0f));

		// Reset Acceleration Rate
		m_pStarShip->SetAccelerationRate(4.0f);
		acceleration = 4.0f;

		// Reset the Turn Rate
		m_pStarShip->SetTurnRate(5.0f);
		turn_rate = 5.0f;

		// Reset the Target for the StarShip
		m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	}

	
	ImGui::End();
}
