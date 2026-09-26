#pragma once
#include "HBE/HBE.h"
#include "box3d/box3d.h"
using namespace HBE;

struct RigidBody;

class PhysicsSystem : public System {
	float fixed_step = 1.0f / 60.0f;
	float timeSinceLastStep = 0;
	b3WorldId world_id;
	event_subscription_id attach_subscription;
	event_subscription_id detach_subscription;
	event_subscription_id update_subscription;

public:
	SYSTEM_IDS(PhysicsSystem)
	PhysicsSystem(Scene *scene);
	void update(float deltaTime);

	void onRigidBodyAttached(Entity entity);
	void onRigidBodyDetached(Entity entity);
	~PhysicsSystem();
};
