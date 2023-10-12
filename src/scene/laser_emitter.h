#ifndef __SCENE_LASER_EMITTER_H__
#define __SCENE_LASER_EMITTER_H__

#include "../math/transform.h"
#include "../levels/level_definition.h"

struct LaserEmitter {
    struct Transform transform;
    short roomIndex;
    short signalIndex;
};

void laserEmitterInit(struct LaserEmitter* laser, struct LaserEmitterDefinition* definition);

void laserEmitterUpdate(struct LaserEmitter* laser);

#endif