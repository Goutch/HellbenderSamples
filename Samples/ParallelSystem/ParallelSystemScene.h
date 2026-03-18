#pragma once

#include <filesystem>

#include "HBE.h"
#include "QuadRendererSystem.h"
#include "RotatorSystem.h"
using namespace HBE;

class ParallelSystemScene : public Scene
{
	RotatorSystem* rotator_system;
	QuadRendererSystem* quad_renderer_system;

public:
	ParallelSystemScene() : Scene()
	{
		rotator_system = new RotatorSystem(this);
		quad_renderer_system = new QuadRendererSystem(this);
		setupScene();
	}

	~ParallelSystemScene()
	{
		delete rotator_system;
		delete quad_renderer_system;
	}

	void setupScene()
	{
		Entity camera_entity = createEntity3D();
		camera_entity.attach<Camera2D>();
		setCameraEntity(camera_entity);

		int n = 100;
		for (int i = 0; i < n; i++)
		{
			//do not create a "3D" entity since it has a node by default. this component prevent multithreading
			Entity entity = createEntity();
			entity.attach<Transform>();
			Rotator* rotator = entity.attach<Rotator>();
			rotator->speed = Random::floatRange(0.1, 1);
			entity.get<Transform>()->translate(vec3(
				Random::floatRange(-7, 7),
				Random::floatRange(-5, 5),
				0));

			QuadRenderer* renderer = entity.attach<QuadRenderer>();
			renderer->color = vec4(
				Random::floatRange(0.0, 1.0),
				Random::floatRange(0.0, 1.0),
				Random::floatRange(0.0, 1.0),
				1.0f);
			entity.get<Transform>()->setLocalScale(vec3(Random::floatRange(0.01, 0.2)));
		}
	}
};
