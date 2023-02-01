#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Target.h"
#include "Obstacle.h"
#include "StarShip.h"

class PlayScene : public Scene
{
public:
	PlayScene();
	~PlayScene() override;

	// Scene LifeCycle Functions
	virtual void Draw() override;
	virtual void Update() override;
	virtual void Clean() override;
	virtual void HandleEvents() override;
	virtual void Start() override;
private:
	// IMGUI Function
	void GUI_Function();
	std::string m_guiTitle;
	glm::vec2 m_mousePosition;

	// Game Objects for the Scene
	Target* m_pTarget;
	Obstacle* m_pObstacle;
	StarShip* m_pStarShip;

	// Debugging Variables
	bool m_bDebugView;
};

#endif /* defined (__PLAY_SCENE__) */