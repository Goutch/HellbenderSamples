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
        ROTATOR_SYSTEM_STATE_SINGLE_THREADED = 0,
        ROTATOR_SYSTEM_STATE_MULTI_THREADED = 1,
        ROTATOR_SYSTEM_STATE_INACTIVE = 2,
    };

    int state = ROTATOR_SYSTEM_STATE_SINGLE_THREADED;

public:
    RotatorSystem(Scene *scene);

    ~RotatorSystem();

    void update(float delta);

    void updateMultiThreaded(float delta);

    void updateSingleThreaded(float delta);
};
