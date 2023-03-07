#include "Obstacle.h"


#include "SoundManager.h"
#include "TextureManager.h"

Obstacle::Obstacle()
{
	TextureManager::Instance().Load("../Assets/textures/mine.png", "obstacle");

	const auto size = TextureManager::Instance().GetTextureSize("obstacle");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));

	GetTransform()->position = glm::vec2(300.0f, 300.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	setIsCentered(true);

	SetType(GameObjectType::OBSTACLE);
	GetRigidBody()->isColliding = false;
}

Obstacle::~Obstacle()
= default;

void Obstacle::Draw()
{
	TextureManager::Instance().Draw("obstacle", GetTransform()->position, 0, 255, true);
}

void Obstacle::Update()
{
}

void Obstacle::Clean()
{
}
