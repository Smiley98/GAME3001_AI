#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include "InputType.h"

// required for IMGUI
#include "imgui.h"
#include "imgui_sdl.h"
#include "Renderer.h"
#include "Util.h"

// Gives us glm::vecn::value_ptr() to speed-track imgui arrays
#include <glm/gtc/type_ptr.hpp>

PlayScene::PlayScene()
{
	PlayScene::Start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::Draw()
{
	// I will explain this if there's time, otherwise you can find it in the commits tab :)
	//TextureManager::Instance().Load("../Assets/textures/nebula.png", "bg");
	//TextureManager& tm = TextureManager::Instance();
	//static glm::vec2 size = tm.GetTextureSize("bg");
	//
	//static float scrollSpeed = 5.0f;
	//static float scrollPosition1 = 0.0f;
	//static float scrollPosition2 = -size.x;
	//
	//if (scrollPosition1 >= size.x)
	//	scrollPosition1 = -size.x;
	//if (scrollPosition2 >= size.x)
	//	scrollPosition2 = -size.x;
	//scrollPosition1 += scrollSpeed;
	//scrollPosition2 += scrollSpeed;
	//
	//TextureManager::Instance().Draw("bg", scrollPosition1, 0);
	//TextureManager::Instance().Draw("bg", scrollPosition2, 0);

	DrawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();
	m_pTarget->Update();

	m_pShip->SetTargetPosition(m_pTarget->GetTransform()->position);
	m_pShip->Update();

	if (CollisionManager::AABBCheck(m_pShip, m_pTarget))
	{
		m_pShip->GetTransform()->position = glm::vec2(400.0f, 300.0f);
	}
}

void PlayScene::Clean()
{
	RemoveAllChildren();
}

void PlayScene::HandleEvents()
{
	EventManager::Instance().Update();

	GetPlayerInput();

	GetKeyboardInput();
}

void PlayScene::GetPlayerInput()
{
	switch (m_pCurrentInputType)
	{
		case static_cast<int>(InputType::KEYBOARD_MOUSE):
		if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_A))
		{

		}
		break;
	}
}

void PlayScene::GetKeyboardInput()
{
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

	// Set Input Type
	m_pCurrentInputType = static_cast<int>(InputType::KEYBOARD_MOUSE);

	// Target Object
	m_pTarget = new Target();
	AddChild(m_pTarget);

	// Ship Object
	m_pShip = new Ship();
	AddChild(m_pShip);

	// TODO -- Load a sound with id "yay" to play automatically on-collision!
	SoundManager::Instance().Load("../Assets/audio/yay.ogg", "yay", SoundType::SOUND_SFX);

	/* DO NOT REMOVE */
	ImGuiWindowFrame::Instance().SetGuiFunction([this] { GUI_Function(); });
}

void PlayScene::GUI_Function() 
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("Debug View", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);
	
	static glm::vec2 targetPosition;
	if (ImGui::SliderFloat2("Target Position", glm::value_ptr(targetPosition), 0.0f, 500.0f))
	{
		m_pTarget->GetTransform()->position = targetPosition;
	}

	static glm::vec2 targetVelocity;
	if (ImGui::SliderFloat2("Target Velocity", glm::value_ptr(targetVelocity), -5.0f, 5.0f))
	{
		m_pTarget->GetRigidBody()->velocity = targetVelocity;
	}

	// Reset target object
	if (ImGui::Button("Target Reset"))
	{
		// We can perform a "soft reset" by reassigning the target's values
		//m_pTarget->GetTransform()->position = glm::vec2(100.0f, 100.0f);
		//m_pTarget->GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);

		// Or, we can perform a "hard reset" by deleting and re-creating the target
		// (note that you should use RemoveChild() to delete objects instead of delete).
		RemoveChild(m_pTarget);
		m_pTarget = new Target();
		AddChild(m_pTarget);

		// Reset target sliders
		targetPosition = m_pTarget->GetTransform()->position;
		targetVelocity = m_pTarget->GetRigidBody()->velocity;
	}

	static float steeringForce = 500.0f;
	if (ImGui::SliderFloat("Steering force", &steeringForce, 0.0f, 1000.0f))
	{
		m_pShip->SetMaxSpeed(steeringForce);
	}

	static bool enabled = true;
	if (ImGui::Checkbox("Toggle Ship", &enabled))
	{
		m_pShip->SetEnabled(enabled);
	}
	
	ImGui::End();
}
