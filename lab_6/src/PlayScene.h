#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "Target.h"
#include "StarShip.h"
#include "Obstacle.h"
#include "PathNode.h"

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
	StarShip* m_pStarShip;
	std::vector<Obstacle*> m_pObstacles;
	void BuildObstaclePool();
	void CheckShipLOS(DisplayObject* object);

	// Path Nodes
	std::vector<PathNode*> m_pGrid;
	void BuildGrid();
	void ClearNodes();
	void ToggleGrid(bool isVisible);

	// Debugging Variables
	bool m_isGridEnabled;
};

#endif /* defined (__PLAY_SCENE__) */