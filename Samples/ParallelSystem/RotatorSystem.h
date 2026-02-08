#pragma once
#include "HBE.h"
using namespace HBE;

struct Rotator {
    COMPONENT_IDS(Rotator)
    vec3 rotation = vec3(0, 0, 1);
    float speed = 10;
};

class RotatorSystem : public HBE::System {
    enum ROTATOR_SYSTEM_STATE {
        ROTATOR_SYSTEM_STATE_MULTI_THREADED_PAGES = 0,
        ROTATOR_SYSTEM_STATE_MULTI_THREADED_ENTITIES = 1,
        ROTATOR_SYSTEM_STATE_SINGLE_THREADED = 2,
        ROTATOR_SYSTEM_STATE_INACTIVE = 3,
    };

    int state = ROTATOR_SYSTEM_STATE_MULTI_THREADED_PAGES;

public:
    RotatorSystem(Scene *scene);

    ~RotatorSystem();

    void update(float delta);

    void updatePageMultiThreaded(float delta);

    void updateEntityMultiThreaded(float delta);

    void updateSingleThreaded(float delta);
};
