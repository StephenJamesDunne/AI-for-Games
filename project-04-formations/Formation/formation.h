#pragma once
#include "Global.h"
#include "Agent.h"

class Formation
{
public:
	Formation();
	Formation(int);
	~Formation();
	TargetTrans GetTargetSlot(int i);

	void SetTargetSlot(int i, TargetTrans target);

	inline void setPosition(Vector2D pos)	{ anchor.setpostion(pos); };
	inline void setTarget(TargetTrans targ) { target = targ; };
	inline TargetTrans getTarget()			{  return target; };

	inline Vector2D getSlotPosition(int i)	{ return (anchor.getposition() + slot[i].position); };

	inline void setWidth(int w) { width = w; };

	Transform Move();

	Agent anchor;
	
private:

	TargetTrans slot[4];

	TargetTrans target;

	int width;

};

