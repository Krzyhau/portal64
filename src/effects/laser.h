#ifndef __LASER_H__
#define __LASER_H__

#include "../physics/collision_object.h"
#include "../math/transform.h"


struct LaserSegment{
    struct Vector3 start;
    struct Vector3 end;
};

struct Laser
{
    struct Transform transform;
    int currentRoom;
    struct CollisionObject *emitterObject;
    struct LaserSegment* segments;
    short numSegments;
};

void laserInit(struct Laser *laser, struct CollisionObject* emitterObject);

void laserUpdatePosition(struct Laser *laser, struct Transform *transform, int roomIndex);
void laserUpdate(struct Laser *laser);

#endif