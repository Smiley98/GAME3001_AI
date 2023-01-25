#pragma once
#include <glm/vec2.hpp>
#include <glm/common.hpp>
#include "Util.h"

glm::vec2 Seek(glm::vec2 target, glm::vec2 seeker, glm::vec2 seekerVelocity, float maxVelocity)
{
	// Step 1 -- calculate "desired velocity" vector by getting direction FROM seeker TO target.
	// ***AB = B - A***
	glm::vec2 toTarget = target - seeker;
	toTarget = Util::Normalize(toTarget);
	toTarget *= maxVelocity;

	//return seekerVelocity - toTarget;
	return toTarget - seekerVelocity;
}
