//
// Created by username on 9/24/2026.
//

#include "RigidBody.h"

void RigidBody::setDynamic(bool dynamic) {
	this->dynamic = dynamic;
	body_dirty = true;
}

bool RigidBody::getDynamic() {
	return dynamic;
}

void RigidBody::setVelocity(vec3 velocity) {
	this->velocity = velocity;
	if (!B3_IS_NULL(body_id))
		b3Body_SetLinearVelocity(body_id, reinterpret_cast<b3Vec3 &>(velocity));

}

vec3 RigidBody::getVelocity() {
	if (B3_IS_NULL(body_id)) return velocity;

	b3Vec3 b3_velocity = b3Body_GetLinearVelocity(body_id);
	velocity = reinterpret_cast<vec3 &>(b3_velocity);
	return velocity;
}

void RigidBody::setAngularVelocity(vec3 velocity) {
	b3Body_SetAngularVelocity(body_id, reinterpret_cast<b3Vec3 &>(velocity));
}

vec3 RigidBody::getAngularVelocity() {
	if (B3_IS_NULL(body_id)) return vec3(0);
	b3Vec3 angualr_velocity = b3Body_GetAngularVelocity(body_id);
	angular_velocity = reinterpret_cast<vec3 &>(angualr_velocity);
	return angular_velocity;
}

void RigidBody::setShapeBox(vec3 size) {
	this->size = size;
}
