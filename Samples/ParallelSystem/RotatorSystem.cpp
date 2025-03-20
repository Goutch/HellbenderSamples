//
// Created by username on 3/19/2025.
//

#include "RotatorSystem.h"

RotatorSystem::RotatorSystem(Scene* scene): System(scene)
{
	scene->onUpdate.subscribe(this, &RotatorSystem::update);
}

void RotatorSystem::update(float delta)
{
	if (!use_multi_threading)
	{
		auto rotator_group = scene->group<Transform, Rotator>();
		for (auto [handle, transform, rotator] : rotator_group)
		{
			transform.rotate(rotator.rotation * rotator.speed * delta);
		}
	}
	else
	{
		auto rotator_group = scene->group<Transform, Rotator>();
	}
}
