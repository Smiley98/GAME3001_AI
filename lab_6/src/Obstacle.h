#pragma once
#ifndef __OBSTACLE__
#define __OBSTACLE__
#include "NavigationObject.h"

class Obstacle final : public NavigationObject
{
public:
	// constructors
	Obstacle();
	
	// destructor
	~Obstacle() override;
	
	// life cycle functions
	void Draw() override;
	void Update() override;
	void Clean() override;
private:
	
};

#endif /* defined (__OBSTACLE__) */