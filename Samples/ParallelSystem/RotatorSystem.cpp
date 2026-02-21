#include "RotatorSystem.h"

#include <execution>

RotatorSystem::RotatorSystem(Scene *scene) : System(scene) {
    scene->onUpdate.subscribe(update_subscription_id,this, &RotatorSystem::update);
}

RotatorSystem::~RotatorSystem() {
    scene->onUpdate.unsubscribe(update_subscription_id);
}

void RotatorSystem::update(float delta) {
    if (Input::getKeyDown(KEY_T)) {
        state++;
        std::string state_name;

        state = state % 4;
        switch (state) {
            case ROTATOR_SYSTEM_STATE_SINGLE_THREADED:
                state_name = "ROTATOR_SYSTEM_STATE_SINGLE_THREADED";
                break;
            case ROTATOR_SYSTEM_STATE_MULTI_THREADED_PAGES:
                state_name = "ROTATOR_SYSTEM_STATE_MULTI_THREADED_PAGES";
                break;
            case ROTATOR_SYSTEM_STATE_MULTI_THREADED_ENTITIES:
                state_name = "ROTATOR_SYSTEM_STATE_MULTI_THREADED_ENTITIES";
                break;
            case ROTATOR_SYSTEM_STATE_INACTIVE:
                state_name = "ROTATOR_SYSTEM_STATE_INACTIVE";
                break;
        }
        Log::debug("state now at " + state_name);
    }
    switch (state) {
        case ROTATOR_SYSTEM_STATE_SINGLE_THREADED:
            HB_PROFILE_BEGIN("RotatorSystem::updateSingleThreaded");
            updateSingleThreaded(delta);
            HB_PROFILE_END("RotatorSystem::updateSingleThreaded");
            break;
        case ROTATOR_SYSTEM_STATE_MULTI_THREADED_PAGES:
            HB_PROFILE_BEGIN("RotatorSystem::updatePageMultiThreaded");
            updatePageMultiThreaded(delta);
            HB_PROFILE_END("RotatorSystem::updatePageMultiThreaded");
            break;
        case ROTATOR_SYSTEM_STATE_MULTI_THREADED_ENTITIES:
            HB_PROFILE_BEGIN("RotatorSystem::updateEntityMultiThreaded");
            updateEntityMultiThreaded(delta);
            HB_PROFILE_END("RotatorSystem::updateEntityMultiThreaded");
            break;
        case ROTATOR_SYSTEM_STATE_INACTIVE:
            break;
    }
}



void RotatorSystem::updatePageMultiThreaded(float delta) {
    auto rotator_group = scene->group<Transform, Rotator>();
    rotator_group.forEachPageParallel([delta](entity_handle handle, Transform &transform, Rotator &rotator) {
        for (int i = 0; i < 1000; ++i) {
            float expensive_opperation = pow(sqrt(rotator.speed), 2.0f);
            rotator.speed = expensive_opperation*i;
        }
    });
}

void RotatorSystem::updateEntityMultiThreaded(float delta) {
    auto rotator_group = scene->group<Transform, Rotator>();
    rotator_group.forEachEntityParallel([delta](entity_handle handle, Transform &transform, Rotator &rotator) {
        for (int i = 0; i < 1000; ++i) {
            float expensive_opperation = pow(sqrt(rotator.speed), 2.0f);
            rotator.speed = expensive_opperation*i;
        }
    });
}

void RotatorSystem::updateSingleThreaded(float delta) {
    auto rotator_group = scene->group<Transform, Rotator>();
    rotator_group.forEach([delta](entity_handle handle, Transform &transform, Rotator &rotator) {
        for (int i = 0; i < 1000; ++i) {
            float expensive_opperation = pow(sqrt(rotator.speed), 2.0f);
            rotator.speed = expensive_opperation*i;
        }
    });
}
