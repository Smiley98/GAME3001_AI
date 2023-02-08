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

		// Draw Collider Bounds for the Obstacle
		Util::DrawRect(m_pObstacle->GetTransform()->position -
			glm::vec2(m_pObstacle->GetWidth() * 0.5f, m_pObstacle->GetHeight() * 0.5f),
			m_pObstacle->GetWidth(), m_pObstacle->GetHeight());

		if(m_pStarShip->IsEnabled())
		{
			Util::DrawRect(m_pStarShip->GetTransform()->position -
				glm::vec2(m_pStarShip->GetWidth() * 0.5f, m_pStarShip->GetHeight() * 0.5f),
				m_pStarShip->GetWidth(), m_pStarShip->GetHeight());

			CollisionManager::RotateAABB(m_pStarShip, m_pStarShip->GetCurrentHeading());

			// draw whiskers
			Util::DrawLine(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetLeftLeftLOSEndPoint(), m_pStarShip->GetLineColour(0));
			Util::DrawLine(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetLeftLOSEndPoint(), m_pStarShip->GetLineColour(1));
			Util::DrawLine(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetMiddleLOSEndPoint(), m_pStarShip->GetLineColour(2));
			Util::DrawLine(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetRightLOSEndPoint(), m_pStarShip->GetLineColour(3));
			Util::DrawLine(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetRightRightLOSEndPoint(), m_pStarShip->GetLineColour(4));
			
		}
	}

	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();

	//CollisionManager::AABBCheck(m_pTarget, m_pObstacle);

	if(m_pStarShip->IsEnabled())
	{
		CollisionManager::CircleAABBCheck(m_pTarget, m_pStarShip);

		CollisionManager::AABBCheck(m_pStarShip, m_pObstacle);

		// obstacle information
		const auto boxWidth = m_pObstacle->GetWidth();
		const int halfBoxWidth = boxWidth * 0.5f;
		const auto boxHeight = m_pObstacle->GetHeight();
		const int halfBoxHeight = boxHeight * 0.5f;
		const auto boxStart = 
			m_pObstacle->GetTransform()->position - glm::vec2(halfBoxWidth, halfBoxHeight);

		// Check every whisker to see if it is colliding with the obstacle
		m_pStarShip->GetCollisionWhiskers()[0] =
			CollisionManager::LineRectCheck(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetLeftLeftLOSEndPoint(), boxStart, boxWidth, boxHeight);
		m_pStarShip->GetCollisionWhiskers()[1] =
			CollisionManager::LineRectCheck(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetLeftLOSEndPoint(), boxStart, boxWidth, boxHeight);
		m_pStarShip->GetCollisionWhiskers()[2] =
			CollisionManager::LineRectCheck(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetMiddleLOSEndPoint(), boxStart, boxWidth, boxHeight);
		m_pStarShip->GetCollisionWhiskers()[3] =
			CollisionManager::LineRectCheck(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetRightLOSEndPoint(), boxStart, boxWidth, boxHeight);
		m_pStarShip->GetCollisionWhiskers()[4] =
			CollisionManager::LineRectCheck(m_pStarShip->GetTransform()->position,
				m_pStarShip->GetRightRightLOSEndPoint(), boxStart, boxWidth, boxHeight);

		for (int i = 0; i < 5; ++i)
		{
			m_pStarShip->SetLineColour(i,
				(m_pStarShip->GetCollisionWhiskers()[i]) ? 
				glm::vec4(1, 0, 0, 1) : glm::vec4(0, 1, 0, 1));
		}
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
	m_pTarget->GetTransform()->position = glm::vec2(640.0f, 300.0f);
	AddChild(m_pTarget, 1);

	// Add the StarShip to the Scene
	m_pStarShip = new StarShip();
	m_pStarShip->GetTransform()->position = glm::vec2(100.0f, 300.0f);
	m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	m_pStarShip->SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // facing right
	m_pStarShip->SetEnabled(false);
	AddChild(m_pStarShip, 2);

	m_pObstacle = new Obstacle();
	m_pObstacle->GetTransform()->position = glm::vec2(450.0f, 300.0f);
	AddChild(m_pObstacle, 1);

	// Preload Sounds

	SoundManager::Instance().Load("../Assets/Audio/yay.ogg", "yay", SoundType::SOUND_SFX);
	SoundManager::Instance().Load("../Assets/Audio/thunder.ogg", "thunder", SoundType::SOUND_SFX);

	ImGuiWindowFrame::Instance().SetGuiFunction(std::bind(&PlayScene::GUI_Function, this));
}

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

	// Target Properties
	static float target_position[2] = { m_pTarget->GetTransform()->position.x,
		m_pTarget->GetTransform()->position.y };
	if(ImGui::SliderFloat2("Target Position", target_position, 0.0f, 800.0f))
	{
		m_pTarget->GetTransform()->position = glm::vec2(target_position[0], target_position[1]);
		m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	}

	ImGui::Separator();

	// Obstacle Properties
	static float obstacle_position[2] = { m_pObstacle->GetTransform()->position.x,
		m_pObstacle->GetTransform()->position.y };
	if (ImGui::SliderFloat2("Obstacle Position", obstacle_position, 0.0f, 800.0f))
	{
		m_pObstacle->GetTransform()->position = glm::vec2(obstacle_position[0], obstacle_position[1]);
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

	// whisker properties
	static float whisker_angle = m_pStarShip->GetWhiskerAngle();
	if(ImGui::SliderFloat("Whisker Angle", &whisker_angle, 10.0f, 60.0f))
	{
		m_pStarShip->UpdateWhiskers(whisker_angle);
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

		// Reset the Turn Rate
		m_pStarShip->SetTurnRate(5.0f);

		// Reset the Target for the StarShip
		m_pStarShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	}

	
	ImGui::End();
}
