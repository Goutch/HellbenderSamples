//
// Created by Gabri on 9/24/2026.
//

#include "PhysicsSystem.h"

PhysicsSystem::PhysicsSystem(Scene* scene) : System(scene)
{
    b3WorldDef world_def{};
    world_def.workerCount=1;
    world_def.
    world_id = b3CreateWorld(&world_def);
}
