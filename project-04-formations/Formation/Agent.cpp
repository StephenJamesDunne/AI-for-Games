#include "stdafx.h"
#include "Agent.h"


Agent::Agent(float max_vel, float max_acc, float max_angvel, float max_angacc)
{
	max_velocity = max_vel;
	max_acceleration = max_acc;

	max_angvelocity = max_angvel;
	max_angacceleration = max_angacc;

	orientation.angle = 0.f;
}

Agent::Agent()
{
}


Agent::~Agent()
{
}
