#include "RotatorSystem.h"

#include <execution>

RotatorSystem::RotatorSystem(Scene *scene) : System(scene) {
    scene->onUpdate.subscribe(this, &RotatorSystem::update);
}

RotatorSystem::~RotatorSystem() {
    scene->onUpdate.unsubscribe(this);
}

void RotatorSystem::update(float delta) {
    if (Input::getKeyDown(KEY_T)) {
        state++;
        std::string state_name;
        switch (state) {
            case ROTATOR_SYSTEM_STATE_SINGLE_THREADED:
                state_name = "ROTATOR_SYSTEM_STATE_SINGLE_THREADED";
                break;
            case ROTATOR_SYSTEM_STATE_MULTI_THREADED:
                state_name = "ROTATOR_SYSTEM_STATE_MULTI_THREADED";
                break;
            case ROTATOR_SYSTEM_STATE_INACTIVE:
                state_name = "ROTATOR_SYSTEM_STATE_INACTIVE";
                break;
        }
        Log::debug("state now at " + state_name);
        state = state % 3;
    }
    switch (state) {
        case ROTATOR_SYSTEM_STATE_SINGLE_THREADED:
            HB_PROFILE_BEGIN("RotatorSystem::updateSingleThreaded");
            updateSingleThreaded(delta);
            HB_PROFILE_END("RotatorSystem::updateSingleThreaded");
            break;
        case ROTATOR_SYSTEM_STATE_MULTI_THREADED:
            HB_PROFILE_BEGIN("RotatorSystem::updateMultiThreaded");
            updateMultiThreaded(delta);
            HB_PROFILE_END("RotatorSystem::updateMultiThreaded");
            break;
        case ROTATOR_SYSTEM_STATE_INACTIVE:
            break;
    }
}


void RotatorSystem::updateMultiThreaded(float delta) {
    auto rotator_group = scene->group<Transform, Rotator>();
    rotator_group.forEachParallel([delta](entity_handle handle, Transform &transform, Rotator &rotator) {
        float expensive_opperation = pow(sqrt(rotator.speed), 2.0f);
        transform.rotate(rotator.rotation * expensive_opperation * delta);
    });
}

void RotatorSystem::updateSingleThreaded(float delta) {
    auto rotator_group = scene->group<Transform, Rotator>();
    rotator_group.forEach([delta](entity_handle handle, Transform &transform, Rotator &rotator) {
        float expensive_opperation = pow(sqrt(rotator.speed), 2.0f);
        transform.rotate(rotator.rotation * expensive_opperation * delta);
    });
}
