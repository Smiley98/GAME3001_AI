#include "StarShip.h"

#include "Game.h"
#include "TextureManager.h"
#include "Util.h"

StarShip::StarShip()
{
	TextureManager::Instance().Load("../Assets/textures/ncl.png", "starship");

	const auto size = TextureManager::Instance().GetTextureSize("starship");
	SetWidth(static_cast<int>(size.x));
	SetHeight(static_cast<int>(size.y));
	GetTransform()->position = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->bounds = glm::vec2(GetWidth(), GetHeight());
	GetRigidBody()->velocity = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->acceleration = glm::vec2(0.0f, 0.0f);
	GetRigidBody()->isColliding = false;
	SetType(GameObjectType::AGENT);

	// Starting Motion Properties
	m_maxSpeed = 50.0f; // a maximum number of pixels moved per frame
	m_turnRate = 5.0f; // a maximum number of degrees to turn each time-step
	m_accelerationRate = 4.0f; // a maximum number of pixels to add to the velocity each frame

	SetCurrentDirection(glm::vec2(1.0f, 0.0f)); // Facing Right

}

StarShip::~StarShip()
= default;

void StarShip::Draw()
{
	// draw the target
	TextureManager::Instance().Draw("starship", 
		GetTransform()->position, static_cast<double>(GetCurrentHeading()), 255, true);
}

void StarShip::Update()
{
	m_move();
}

void StarShip::Clean()
{
}

float StarShip::GetMaxSpeed() const
{
	return m_maxSpeed;
}

float StarShip::GetTurnRate() const
{
	return m_turnRate;
}

float StarShip::GetAccelerationRate() const
{
	return m_accelerationRate;
}

glm::vec2 StarShip::GetDesiredVelocity() const
{
	return m_desiredVelocity;
}

void StarShip::SetMaxSpeed(const float speed)
{
	m_maxSpeed = speed;
}

void StarShip::SetTurnRate(const float angle)
{
	m_turnRate = angle;
}

void StarShip::SetAccelerationRate(const float rate)
{
	m_accelerationRate = rate;
}

void StarShip::SetDesiredVelocity(const glm::vec2 target_position)
{
	SetTargetPosition(target_position);
	m_desiredVelocity = Util::Normalize(target_position - GetTransform()->position);
	GetRigidBody()->velocity = m_desiredVelocity - GetRigidBody()->velocity;
}

void StarShip::Seek()
{
	SetDesiredVelocity(GetTargetPosition());

	const glm::vec2 steering_direction = GetDesiredVelocity() - GetCurrentDirection();

	LookWhereYoureGoing(steering_direction);

	GetRigidBody()->acceleration = GetCurrentDirection() * GetAccelerationRate();
}

void StarShip::LookWhereYoureGoing(const glm::vec2 target_direction)
{
	// 1. Try obstacle avoidance with left and right raycasts
	// 2. If no obstacles, steer towards target based on turn rate 
	const float target_rotation = Util::SignedAngle(GetCurrentDirection(), target_direction);
	const float turn_sensitivity = 25.0f;

	float box_width = pObstacle->GetWidth();
	float half_box_width = box_width * 0.5f;
	float box_height = pObstacle->GetHeight();
	float half_box_height = box_height * 0.5f;
	glm::vec2 box_start = pObstacle->GetTransform()->position - glm::vec2(half_box_width, half_box_height);

	glm::vec2 left_line_start = this->GetTransform()->position;
	glm::vec2 left_line_end = this->GetTransform()->position + Util::Direction(this->GetCurrentHeading() - 15.0f) * this->GetLOSDistance();

	glm::vec2 right_line_start = this->GetTransform()->position;
	glm::vec2 right_line_end = this->GetTransform()->position + Util::Direction(this->GetCurrentHeading() + 15.0f) * this->GetLOSDistance();
	
	if (CollisionManager::LineRectCheck(left_line_start, left_line_end, box_start, box_width, box_height))
	{
		SetCurrentHeading(GetCurrentHeading() + GetTurnRate());
	}
	else if (CollisionManager::LineRectCheck(right_line_start, right_line_end, box_start, box_width, box_height))
	{
		SetCurrentHeading(GetCurrentHeading() - GetTurnRate());
	}
	else if(abs(target_rotation) > turn_sensitivity)
	{
		if(target_rotation > 0.0f)
		{
			// turn right
			SetCurrentHeading(GetCurrentHeading() + GetTurnRate());
		}
		else if (target_rotation < 0.0f)
		{
			// turn left
			SetCurrentHeading(GetCurrentHeading() - GetTurnRate());
		}
	}
}

void StarShip::m_move()
{
	Seek();

	// maybe a switch - case

	// switch (behaviour)
	//    case(seek)
	//    case(arrive)
	//    case(flee)
	//    case(avoidance)

	//                      final Position  Position Term   Velocity      Acceleration Term
	// Kinematic Equation-> Pf            = Pi +            Vi * (time) + (0.5) * Ai * (time * time)

	const float dt = Game::Instance().GetDeltaTime();

	// accessing the position Term
	const glm::vec2 initial_position = GetTransform()->position;

	// compute the velocity Term
	const glm::vec2 velocity_term = GetRigidBody()->velocity * dt;

	// compute the acceleration Term
	const glm::vec2 acceleration_term = GetRigidBody()->acceleration * 0.5f; // * dt * dt


	// compute the new position
	glm::vec2 final_position = initial_position + velocity_term + acceleration_term;

	GetTransform()->position = final_position;

	// add our acceleration to velocity
	GetRigidBody()->velocity += GetRigidBody()->acceleration;

	// clamp our velocity at max speed
	GetRigidBody()->velocity = Util::Clamp(GetRigidBody()->velocity, GetMaxSpeed());
}
