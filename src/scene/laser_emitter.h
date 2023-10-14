#ifndef __SCENE_LASER_EMITTER_H__
#define __SCENE_LASER_EMITTER_H__

#include "../math/transform.h"
#include "../levels/level_definition.h"
#include "../effects/laser.h"

struct LaserEmitter {
    struct Transform transform;
    struct Laser ownLaser;
    short roomIndex;
    short signalIndex;
};

void laserEmitterInit(struct LaserEmitter* laserEmitter, struct LaserEmitterDefinition* definition);

void laserEmitterUpdate(struct LaserEmitter* laserEmitter);

#endif