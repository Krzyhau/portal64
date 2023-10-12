#ifndef __LASER_CUBE_H__
#define __LASER_CUBE_H__

#include "../physics/collision_object.h"
#include "../levels/level_definition.h"
#include "../sk64/skelatool_armature.h"

enum LaserCubeFlags
{
    LaserCubeFlagsPowered = (1 << 0),
};

struct LaserCube {
    struct CollisionObject collisionObject;
    struct RigidBody rigidBody;
    short dynamicId;
    float fizzleTime;
    short flags;
};

void laserCubeInit(struct LaserCube *laserCube, struct LaserCubeDefinition *definition);
void laserCubeUpdate(struct LaserCube *laserCube);

#endif