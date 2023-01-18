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
	DrawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();
	m_pTarget->Update();
	m_pShip->Update();
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
	
	// Plane Sprite
	//m_pPlaneSprite = new Plane();
	//AddChild(m_pPlaneSprite);

	// Player Sprite
	//m_pPlayer = new Player();
	//AddChild(m_pPlayer);
	//m_playerFacingRight = true;

	// Target Object
	m_pTarget = new Target();
	AddChild(m_pTarget);

	// Ship Object
	m_pShip = new Ship();
	AddChild(m_pShip);

	// Back Button
	//m_pBackButton = new Button("../Assets/textures/backButton.png", "backButton", GameObjectType::BACK_BUTTON);
	//m_pBackButton->GetTransform()->position = glm::vec2(300.0f, 400.0f);
	//m_pBackButton->AddEventListener(Event::CLICK, [&]()-> void
	//{
	//	m_pBackButton->SetActive(false);
	//	Game::Instance().ChangeSceneState(SceneState::START);
	//});
	//
	//m_pBackButton->AddEventListener(Event::MOUSE_OVER, [&]()->void
	//{
	//	m_pBackButton->SetAlpha(128);
	//});
	//
	//m_pBackButton->AddEventListener(Event::MOUSE_OUT, [&]()->void
	//{
	//	m_pBackButton->SetAlpha(255);
	//});
	//AddChild(m_pBackButton);

	// Next Button
	//m_pNextButton = new Button("../Assets/textures/nextButton.png", "nextButton", GameObjectType::NEXT_BUTTON);
	//m_pNextButton->GetTransform()->position = glm::vec2(500.0f, 400.0f);
	//m_pNextButton->AddEventListener(Event::CLICK, [&]()-> void
	//{
	//	m_pNextButton->SetActive(false);
	//	Game::Instance().ChangeSceneState(SceneState::END);
	//});
	//
	//m_pNextButton->AddEventListener(Event::MOUSE_OVER, [&]()->void
	//{
	//	m_pNextButton->SetAlpha(128);
	//});
	//
	//m_pNextButton->AddEventListener(Event::MOUSE_OUT, [&]()->void
	//{
	//	m_pNextButton->SetAlpha(255);
	//});
	//AddChild(m_pNextButton);

	/* Instructions Label */
	//m_pInstructionsLabel = new Label("Press the backtick (`) character to toggle Debug View", "Consolas");
	//m_pInstructionsLabel->GetTransform()->position = glm::vec2(Config::SCREEN_WIDTH * 0.5f, 500.0f);
	//AddChild(m_pInstructionsLabel);

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

	if (ImGui::Button("Target Reset"))
	{
		// Reset target object
		m_pTarget->GetTransform()->position = glm::vec2(100.0f, 100.0f);
		m_pTarget->GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);

		// Reset target sliders
		targetPosition = m_pTarget->GetTransform()->position;
		targetVelocity = m_pTarget->GetRigidBody()->velocity;
	}

	//ImGui::Text("Player Input");
	//ImGui::RadioButton("Keyboard / Mouse", &m_pCurrentInputType, static_cast<int>(InputType::KEYBOARD_MOUSE)); ImGui::SameLine();
	//ImGui::RadioButton("Game Controller", &m_pCurrentInputType, static_cast<int>(InputType::GAME_CONTROLLER)); ImGui::SameLine();
	//ImGui::RadioButton("Both", &m_pCurrentInputType, static_cast<int>(InputType::ALL));
	//
	//ImGui::Separator();
	//
	//if(ImGui::Button("My Button"))
	//{
	//	std::cout << "My Button Pressed" << std::endl;
	//}
	//
	//ImGui::Separator();
	//
	//static float float3[3] = { 0.0f, 1.0f, 1.5f };
	//if(ImGui::SliderFloat3("My Slider", float3, 0.0f, 2.0f))
	//{
	//	std::cout << float3[0] << std::endl;
	//	std::cout << float3[1] << std::endl;
	//	std::cout << float3[2] << std::endl;
	//	std::cout << "---------------------------\n";
	//}
	
	ImGui::End();
}
