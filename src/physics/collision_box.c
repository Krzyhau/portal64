#include "collision_box.h"

#include <math.h>

#define NORMAL_ZERO_BIAS    0.001f

int _collsionBuildPlaneContact(struct Transform* boxTransform, struct Plane* plane, struct Vector3* point, struct ContactPoint* contact) {
    transformPoint(boxTransform, point, &contact->point);

    contact->intersectionDepth = -planePointDistance(plane, &contact->point);

    if (contact->intersectionDepth <= 0.0f) {
        return 0;
    }

    contact->normal = plane->normal;

    vector3AddScaled(&contact->point, &plane->normal, -contact->intersectionDepth, &contact->point);
    return 1;
}


int collisionBoxCollidePlane(struct CollisionBox* box, struct Transform* boxTransform, struct Plane* plane, struct ContactPoint* output) {
    float boxDistance = planePointDistance(plane, &boxTransform->position);
    float maxBoxReach = vector3MagSqrd(&box->sideLength);

    if (boxDistance > maxBoxReach || boxDistance < 0.0f) {
        return 0;
    }

    struct Quaternion inverseBoxRotation;

    quatConjugate(&boxTransform->rotation, &inverseBoxRotation);

    struct Vector3 normalInBoxSpace;

    quatMultVector(&inverseBoxRotation, &plane->normal, &normalInBoxSpace);

    struct Vector3 deepestCorner;

    int splitAxis[3];
    int splitAxisCount = 0;

    for (int axis = 0; axis < 3; ++axis) {
        float normalValue = VECTOR3_AS_ARRAY(&normalInBoxSpace)[axis];
        if (normalValue < -NORMAL_ZERO_BIAS) {
            VECTOR3_AS_ARRAY(&deepestCorner)[axis] = VECTOR3_AS_ARRAY(&box->sideLength)[axis];
        } else if (normalValue > NORMAL_ZERO_BIAS) {
            VECTOR3_AS_ARRAY(&deepestCorner)[axis] = -VECTOR3_AS_ARRAY(&box->sideLength)[axis];
        } else {
            splitAxis[splitAxisCount] = axis;
            ++splitAxisCount;
        }
    }

    int contactIndex = 0;
    if (splitAxisCount == 0) {
        if (!_collsionBuildPlaneContact(boxTransform, plane, &deepestCorner, &output[contactIndex])) {
            return contactIndex;
        }
        ++contactIndex;
    } else if (splitAxisCount == 1) {
        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[0]] = VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[0]];
        if (!_collsionBuildPlaneContact(boxTransform, plane, &deepestCorner, &output[contactIndex])) {
            return contactIndex;
        }
        ++contactIndex;
        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[0]] = -VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[0]];
        if (!_collsionBuildPlaneContact(boxTransform, plane, &deepestCorner, &output[contactIndex])) {
            return contactIndex;
        }
        ++contactIndex;
    } else if (splitAxisCount == 2) {
        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[0]] = VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[0]];
        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[1]] = VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[1]];
        if (!_collsionBuildPlaneContact(boxTransform, plane, &deepestCorner, &output[contactIndex])) {
            return contactIndex;
        }
        ++contactIndex;

        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[0]] = -VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[0]];
        if (!_collsionBuildPlaneContact(boxTransform, plane, &deepestCorner, &output[contactIndex])) {
            return contactIndex;
        }
        ++contactIndex;

        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[0]] = VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[0]];
        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[1]] = -VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[1]];
        if (!_collsionBuildPlaneContact(boxTransform, plane, &deepestCorner, &output[contactIndex])) {
            return contactIndex;
        }
        ++contactIndex;

        VECTOR3_AS_ARRAY(&deepestCorner)[splitAxis[0]] = -VECTOR3_AS_ARRAY(&box->sideLength)[splitAxis[0]];
        if (!_collsionBuildPlaneContact(boxTransform, plane, &deepestCorner, &output[contactIndex])) {
            return contactIndex;
        }
        ++contactIndex;
    } else {
        // should not happen
        return contactIndex;
    }

    return contactIndex;
}

void collisionBoxCollideQuad(struct CollisionBox* box, struct Transform* boxTransform, struct CollisionQuad* quad) {
    struct Quaternion inverseBoxRotation;

    quatConjugate(&boxTransform->rotation, &inverseBoxRotation);

    struct Vector3 normalInBoxSpace;

    quatMultVector(&inverseBoxRotation, &quad->normal, &normalInBoxSpace);

}