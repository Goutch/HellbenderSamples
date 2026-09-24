#pragma once
#include "HBE/core/scene/ecs/Component.h"
#include "HBE/core/scene/System.h"
#include "box3d/box3d.h"
using namespace HBE;

struct RigidBody {
    COMPONENT_IDS(QuadRenderer)
};

struct Collider
{
    COMPONENT_IDS(Collider)
};

class PhysicsSystem : HBE::System
{
    b3WorldId world_id;

    PhysicsSystem(Scene* scene);
};
