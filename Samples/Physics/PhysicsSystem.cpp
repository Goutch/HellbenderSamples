#include "PhysicsSystem.h"
#include "RigidBody.h"

PhysicsSystem::PhysicsSystem(Scene *scene) : System(scene) {
	b3WorldDef world_def = b3DefaultWorldDef();
	world_def.workerCount = 6;
	world_id = b3CreateWorld(&world_def);
	scene->onUpdate.subscribe(update_subscription, this, &PhysicsSystem::update);
	scene->onAttach<RigidBody>().subscribe(attach_subscription, this, &PhysicsSystem::onRigidBodyAttached);
	scene->onDetach<RigidBody>().subscribe(detach_subscription, this, &PhysicsSystem::onRigidBodyDetached);
}

void PhysicsSystem::update(float delta) {
	timeSinceLastStep += delta;
	if (timeSinceLastStep < fixed_step) {
		return;
	}
	timeSinceLastStep -= fixed_step;

	auto group = scene->group<RigidBody, Transform>();
	HB_PROFILE_BEGIN("Physics allocate bodies");
	//update bodies that are dirty to the right position and reallocate bodies where type changed.
	for (auto [handle,body,transform]: group) {
		if (body.body_dirty) {
			//reallocate body
			if (!B3_IS_NULL(body.body_id)) {
				b3DestroyBody(body.body_id);
				body.body_id = b3_nullBodyId;
			}
			b3BodyDef body_def = b3DefaultBodyDef();
			body_def.type = body.dynamic ? b3_dynamicBody : b3_staticBody;
			vec3 transform_position = transform.position();
			quat transform_rotation = transform.rotation();
			body_def.linearVelocity = reinterpret_cast<b3Vec3 &>(body.velocity);
			body_def.angularVelocity = reinterpret_cast<b3Vec3 &>(body.angular_velocity);
			body_def.position = reinterpret_cast<b3Vec3 &>(transform_position);
			body_def.rotation = b3Quat(b3Vec3(transform_rotation.x, transform_rotation.y, transform_rotation.z), 1.0f);

			body.body_id = b3CreateBody(world_id, &body_def);

			if (!B3_IS_NULL(body.shape_id)) {
				b3DestroyShape(body.shape_id, false);
				body.shape_id = b3_nullShapeId;
			}

			b3BoxHull box = b3MakeBoxHull(body.size.x, body.size.y, body.size.z);
			b3ShapeDef shape_def = b3DefaultShapeDef();
			shape_def.density = 1.0f;
			shape_def.baseMaterial.friction = 0.3f;
			body.shape_id = b3CreateHullShape(body.body_id, &shape_def, &box.base);

			body.body_dirty = false;
		}
	}
	HB_PROFILE_END("Physics allocate bodies");
	HB_PROFILE_BEGIN("Physics step");
	b3World_Step(world_id, fixed_step, 4);
	HB_PROFILE_END("Physics step");
	HB_PROFILE_BEGIN("Physics transfer data");
	for (auto [handle,body,transform]: group) {
		b3Vec3 position = b3Body_GetPosition(body.body_id);
		b3Quat rotation = b3Body_GetRotation(body.body_id);

		transform.setPosition(reinterpret_cast<vec3 &>(position));
		transform.setRotation(quat(rotation.s, rotation.v.x, rotation.v.y, rotation.v.z));
	}
	HB_PROFILE_END("Physics transfer data");
}

void PhysicsSystem::onRigidBodyAttached(Entity entity) {
	entity.get<RigidBody>()->body_dirty = true;
}

void PhysicsSystem::onRigidBodyDetached(Entity entity) {
	RigidBody *rigid_body = entity.get<RigidBody>();
	b3DestroyBody(rigid_body->body_id);
}

PhysicsSystem::~PhysicsSystem() {
	b3DestroyWorld(world_id);
	world_id = b3_nullWorldId;

	scene->onAttach<RigidBody>().unsubscribe(attach_subscription);
	scene->onDetach<RigidBody>().unsubscribe(detach_subscription);
}
