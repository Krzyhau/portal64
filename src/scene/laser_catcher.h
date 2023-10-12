#ifndef __SCENE_LASER_CATCHER_H__
#define __SCENE_LASER_CATCHER_H__

#include "../math/transform.h"
#include "../levels/level_definition.h"

enum LaserCatcherFlags
{
    LaserCatcherFlagsPowered = (1 << 0),
};

struct LaserCatcher {
    struct Transform transform;
    struct SKArmature armature;
    int dynamicId;
    short roomIndex;
    short signalIndex;
    short flags;
};

void laserCatcherInit(struct LaserCatcher* laserCatcher, struct LaserCatcherDefinition* definition);

void laserCatcherUpdate(struct LaserCatcher *laserCatcher);

#endif