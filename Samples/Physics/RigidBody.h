#pragma once
#include "box3d/box3d.h"
#include "HBE/HBE.h"

struct RigidBody {
	COMPONENT_IDS(RigidBody);
	friend class PhysicsSystem;

private:
	bool dynamic = false;
	bool body_dirty = true;
	bool shapeDirty = true;
	vec3 velocity = vec3(0);
	vec3 angular_velocity = vec3(0);
	b3BodyId body_id = b3_nullBodyId;
	b3ShapeId shape_id = b3_nullShapeId;
	vec3 size = vec3(0.5,0.5,0.5);
public:
	void setDynamic(bool dynamic);
	bool getDynamic();

	void setVelocity(vec3 velocity);
	vec3 getVelocity();

	void setAngularVelocity(vec3 velocity);
	vec3 getAngularVelocity();

	void setShapeBox(vec3 size);
	void setShapeSphere(vec3 size);
};
