#ifndef __SCENE_LASER_CATCHER_H__
#define __SCENE_LASER_CATCHER_H__

#include "../math/transform.h"
#include "../levels/level_definition.h"
#include "../physics/collision_object.h"
#include "../sk64/skelatool_armature.h"

enum LaserCatcherFlags
{
    LaserCatcherFlagsPowered = (1 << 0),
    LaserCatcherFlagsHitByLaser = (1 << 1),
};

struct LaserCatcher {
    struct CollisionObject collisionObject;
    struct RigidBody rigidBody;
    struct SKArmature armature;
    short signalIndex;
    short flags;
    short dynamicId;
};

void laserCatcherInit(struct LaserCatcher* laserCatcher, struct LaserCatcherDefinition* definition);

void laserCatcherUpdate(struct LaserCatcher *laserCatcher);

#endif