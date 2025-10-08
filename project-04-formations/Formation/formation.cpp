#include "stdafx.h"
#include "formation.h"

Formation::Formation()
{
}

Formation::Formation(int w)
{
	anchor = Agent(5, 10, 1, 0.1);
	anchor.setGroup(arrivey,1,1);

	width = w;

	slot[0].position = Vector2D(0, 1 * width);
	slot[0].orientation.angle = 0.f;

	slot[1].position = Vector2D(1 * width, 0);
	slot[1].orientation.angle = M_PI / 4;

	slot[2].position = Vector2D(-1 * width, 0);
	slot[2].orientation.angle = -M_PI / 4;

	slot[3].position = Vector2D(1 * width, -1 * width);
	slot[3].orientation.angle = M_PI / 2;
}

Formation::~Formation()
{
}

TargetTrans Formation::GetTargetSlot(int i)
{
	TargetTrans temp;

	Vector2D pos = anchor.getposition() + slot[i].position;
	float angle = anchor.getorientation().angle - pos.getAngle() + M_PI/2;

	float x = cosf(angle) * (slot[i].position.x) - sinf(angle) * (slot[i].position.y) + anchor.getposition().x;

	float y = sinf(angle) * (slot[i].position.x) + cosf(angle) * (slot[i].position.y) + anchor.getposition().y;
	
	temp.position = Vector2D(x,y);
	temp.orientation.angle = anchor.getorientation().angle + slot[i].orientation.angle;
	return temp;
}

void Formation::SetTargetSlot(int i, TargetTrans target)
{
	slot[i] = target;
}

Transform Formation::Move()
{
	Transform trans;
	Behavior behave;
	std::vector<Agent> agents;
	std::vector<Obstacle> obstacles;
	trans = behave.SelectBehavior(target, anchor, agents, obstacles);
	return trans;
}


