#pragma once
#include "HBE.h"
using namespace HBE;

struct Rotator
{
	vec3 rotation = vec3(0, 0, 1);
	float speed = 10;
};

class RotatorSystem : public HBE::System
{
	bool use_multi_threading = false;

	RotatorSystem(Scene* scene);


	void update(float delta);
};
