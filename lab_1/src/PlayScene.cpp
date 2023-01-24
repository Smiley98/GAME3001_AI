#include "PlayScene.h"
#include "Game.h"
#include "EventManager.h"
#include "InputType.h"

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
	TextureManager::Instance().Load("../Assets/textures/nebula.png", "bg");
	TextureManager& tm = TextureManager::Instance();
	static glm::vec2 size = tm.GetTextureSize("bg");

	static float scrollSpeed = 5.0f;
	static float scrollPosition1 = 0.0f;
	static float scrollPosition2 = -size.x;

	if (scrollPosition1 >= size.x)
		scrollPosition1 = -size.x;
	if (scrollPosition2 >= size.x)
		scrollPosition2 = -size.x;
	scrollPosition1 += scrollSpeed;
	scrollPosition2 += scrollSpeed;

	TextureManager::Instance().Draw("bg", scrollPosition1, 0);
	TextureManager::Instance().Draw("bg", scrollPosition2, 0);

	DrawDisplayList();
	SDL_SetRenderDrawColor(Renderer::Instance().GetRenderer(), 255, 255, 255, 255);
}

void PlayScene::Update()
{
	UpdateDisplayList();
	glm::vec2 mousePosition = EventManager::Instance().GetMousePosition();
	float distance = Util::Distance(m_pPlayer->GetTransform()->position, m_pPlaneSprite->GetTransform()->position);
	if (distance < 100.0f)
	{
		Game::Instance().ChangeSceneState(SceneState::END);
	}

	//std::cout << "x: " << mousePosition.x <<" y: " << mousePosition.y << std::endl;
	// exact same as the above cout
	//printf("x: %f y: %f\n", mousePosition.x, mousePosition.y);
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
	{
		// left-right
		if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_A))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_LEFT);
			m_playerFacingRight = false;
			m_pPlayer->GetTransform()->position += glm::vec2(-10.0f, 0.0f);
		}
		else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_D))
		{
			m_pPlayer->SetAnimationState(PlayerAnimationState::PLAYER_RUN_RIGHT);
			m_playerFacingRight = true;
			m_pPlayer->GetTransform()->position += glm::vec2(10.0f, 0.0f);
		}
		else
		{
			m_pPlayer->SetAnimationState(m_playerFacingRight ?
				PlayerAnimationState::PLAYER_IDLE_RIGHT : PlayerAnimationState::PLAYER_IDLE_LEFT);
		}

		// up-down
		if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_W))
		{
			m_pPlayer->GetTransform()->position += glm::vec2(0.0f, -10.0f);
		}
		else if (EventManager::Instance().IsKeyDown(SDL_SCANCODE_S))
		{
			m_pPlayer->GetTransform()->position += glm::vec2(0.0f, 10.0f);
		}
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
	m_pPlaneSprite = new Plane();
	m_pPlaneSprite->GetTransform()->position = glm::vec2(100.0f, 100.0f);
	AddChild(m_pPlaneSprite);
	//RemoveChild(m_pPlaneSprite);// <-- call this to stop an item from being displayed

	// Player Sprite
	m_pPlayer = new Player();
	m_pPlayer->GetTransform()->position = glm::vec2(700.0f, 500.0f);
	AddChild(m_pPlayer);
	m_playerFacingRight = true;

	// Back Button
	m_pBackButton = new Button("../Assets/textures/backButton.png", "backButton", GameObjectType::BACK_BUTTON);
	m_pBackButton->GetTransform()->position = glm::vec2(300.0f, 400.0f);
	m_pBackButton->AddEventListener(Event::CLICK, [&]()-> void
	{
		m_pBackButton->SetActive(false);
		Game::Instance().ChangeSceneState(SceneState::START);
	});

	m_pBackButton->AddEventListener(Event::MOUSE_OVER, [&]()->void
	{
		m_pBackButton->SetAlpha(128);
	});

	m_pBackButton->AddEventListener(Event::MOUSE_OUT, [&]()->void
	{
		m_pBackButton->SetAlpha(255);
	});
	AddChild(m_pBackButton);

	// Next Button
	m_pNextButton = new Button("../Assets/textures/nextButton.png", "nextButton", GameObjectType::NEXT_BUTTON);
	m_pNextButton->GetTransform()->position = glm::vec2(500.0f, 400.0f);
	m_pNextButton->AddEventListener(Event::CLICK, [&]()-> void
	{
		m_pNextButton->SetActive(false);
		Game::Instance().ChangeSceneState(SceneState::END);
	});

	m_pNextButton->AddEventListener(Event::MOUSE_OVER, [&]()->void
	{
		m_pNextButton->SetAlpha(128);
	});

	m_pNextButton->AddEventListener(Event::MOUSE_OUT, [&]()->void
	{
		m_pNextButton->SetAlpha(255);
	});

	AddChild(m_pNextButton);

	/* Instructions Label */
	m_pInstructionsLabel = new Label("Press the backtick (`) character to toggle Debug View", "Consolas");
	m_pInstructionsLabel->GetTransform()->position = glm::vec2(Config::SCREEN_WIDTH * 0.5f, 500.0f);

	AddChild(m_pInstructionsLabel);

	/* DO NOT REMOVE */
	ImGuiWindowFrame::Instance().SetGuiFunction([this] { GUI_Function(); });
}

#include <glm/gtc/type_ptr.hpp>

void PlayScene::GUI_Function() 
{
	// Always open with a NewFrame
	ImGui::NewFrame();

	// See examples by uncommenting the following - also look at imgui_demo.cpp in the IMGUI filter
	//ImGui::ShowDemoWindow();
	
	ImGui::Begin("Your Window Title Goes Here", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoMove);

	ImGui::Text("Player Input");
	ImGui::RadioButton("Keyboard / Mouse", &m_pCurrentInputType, static_cast<int>(InputType::KEYBOARD_MOUSE)); ImGui::SameLine();
	ImGui::RadioButton("Game Controller", &m_pCurrentInputType, static_cast<int>(InputType::GAME_CONTROLLER)); ImGui::SameLine();
	ImGui::RadioButton("Both", &m_pCurrentInputType, static_cast<int>(InputType::ALL));

	ImGui::Separator();

	if(ImGui::Button("My Button"))
	{
		std::cout << "My Button Pressed" << std::endl;
	}

	ImGui::Separator();

	static float float3[3] = { 0.0f, 1.0f, 1.5f };
	if(ImGui::SliderFloat3("My Slider", float3, 0.0f, 2.0f))
	{
		std::cout << float3[0] << std::endl;
		std::cout << float3[1] << std::endl;
		std::cout << float3[2] << std::endl;
		std::cout << "---------------------------\n";
	}

	// 1-D plane movement
	//float x = 0.0f;
	//ImGui::SliderFloat("Plane slider", &x, -1.0f, 1.0f);
	//m_pPlaneSprite->GetTransform()->position += glm::vec2(x, 0.0f);

	//glm::value_ptr treats a vector as an array so we don't have to declare an auxillary variable like below. The choice is yours!
	glm::vec2 xy;
	ImGui::SliderFloat2("Plane slider", glm::value_ptr(xy), -1.0f, 1.0);
	 
	//float xy[2] = { 0.0f, 0.0f };
	//ImGui::SliderFloat2("Plane slider", xy, -1.0f, 1.0);
	//m_pPlaneSprite->GetTransform()->position += glm::vec2(xy[0], xy[1]);
	
	ImGui::End();
}
