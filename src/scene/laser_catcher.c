#include "laser_catcher.h"

#include "../scene/dynamic_scene.h"
#include "../defs.h"

#include "../levels/levels.h"
#include "../levels/cutscene_runner.h"
#include "../physics/collision_scene.h"
#include "../util/memory.h"
#include "../util/time.h"

#include "../build/assets/models/dynamic_model_list.h"

#include "../../build/assets/materials/static.h"

#include "../util/dynamic_asset_loader.h"
#include "../../build/assets/models/dynamic_animated_model_list.h"
#include "../../build/assets/models/props/laser_catcher.h"

#include <math.h>

struct CollisionBox gLaserCatcherBox = {
    {0.5f, 0.1f, 0.5f},
};

struct ColliderTypeData gLaserCatcherCollider = {
    CollisionShapeTypeBox,
    &gLaserCatcherBox,
    0.0f,
    1.0f,
    &gCollisionBoxCallbacks
};

void laserCatcherRender(void* data, struct DynamicRenderDataList* renderList, struct RenderState* renderState) {
    struct LaserCatcher *laserCatcher = (struct LaserCatcher *)data;

    Mtx* matrix = renderStateRequestMatrices(renderState, 1);

    if (!matrix) {
        return;
    }

    transformToMatrixL(&laserCatcher->rigidBody.transform, matrix, SCENE_SCALE);

    Mtx *armature = renderStateRequestMatrices(renderState, PROPS_LASER_CATCHER_DEFAULT_BONES_COUNT);
    if (!armature) {
        return;
    }

    transformToMatrixL(&laserCatcher->armature.pose[PROPS_LASER_CATCHER_ROOT_BONE], &armature[PROPS_LASER_CATCHER_ROOT_BONE], 1.0f);
    transformToMatrixL(&laserCatcher->armature.pose[PROPS_LASER_CATCHER_RING_BONE], &armature[PROPS_LASER_CATCHER_RING_BONE], 1.0f);

    dynamicRenderListAddData(
        renderList,
        laserCatcher->armature.displayList,
        matrix,
        laserCatcher->flags & LaserCatcherFlagsPowered ? LASER_CATCHER_ACTIVATED_INDEX : LASER_CATCHER_INDEX,
        &laserCatcher->rigidBody.transform.position,
        armature
    );
}

void laserCatcherHitByLaser(void *data)
{
    struct LaserCatcher *laserCatcher = (struct LaserCatcher *)data;
    laserCatcher->flags |= LaserCatcherFlagsHitByLaser;
}

void laserCatcherInit(struct LaserCatcher* laserCatcher, struct LaserCatcherDefinition* definition) {
    struct SKArmatureWithAnimations *armature = dynamicAssetAnimatedModel(PROPS_LASER_CATCHER_DYNAMIC_ANIMATED_MODEL);
    skArmatureInit(&laserCatcher->armature, armature->armature);

    collisionObjectInit(&laserCatcher->collisionObject, &gLaserCatcherCollider, &laserCatcher->rigidBody, 1.0f, COLLISION_LAYERS_TANGIBLE | COLLISION_LAYERS_BLOCK_LASER);
    rigidBodyMarkKinematic(&laserCatcher->rigidBody);
    collisionSceneAddDynamicObject(&laserCatcher->collisionObject);

    laserCatcher->rigidBody.transform.position = definition->position;
    laserCatcher->rigidBody.transform.rotation = definition->rotation;
    laserCatcher->rigidBody.transform.scale = gOneVec;
    laserCatcher->rigidBody.currentRoom = definition->roomIndex;
    laserCatcher->signalIndex = definition->signalIndex;

    collisionObjectUpdateBB(&laserCatcher->collisionObject);

    laserCatcher->dynamicId = dynamicSceneAdd(laserCatcher, laserCatcherRender, &laserCatcher->rigidBody.transform.position, 0.5f);
    dynamicSceneSetRoomFlags(laserCatcher->dynamicId, ROOM_FLAG_FROM_INDEX(laserCatcher->rigidBody.currentRoom));

    laserCatcher->collisionObject.data = laserCatcher;
    laserCatcher->collisionObject.laser = laserCatcherHitByLaser;
}

void laserCatcherUpdate(struct LaserCatcher* laserCatcher) {

    laserCatcher->flags &= ~LaserCatcherFlagsPowered;
    if(laserCatcher->flags & LaserCatcherFlagsHitByLaser){
        
        laserCatcher->flags &= ~LaserCatcherFlagsHitByLaser;
        laserCatcher->flags |= LaserCatcherFlagsPowered;
    }

    if(laserCatcher->flags & LaserCatcherFlagsPowered) {
        signalsSend(laserCatcher->signalIndex);

        struct Vector3 angularVelocity = (struct Vector3){.x = 0.0f, .y = -8.0f, .z = 0.0f};

        quatApplyAngularVelocity(
            &laserCatcher->armature.pose[PROPS_LASER_CATCHER_RING_BONE].rotation,
            &angularVelocity,
            FIXED_DELTA_TIME,
            &laserCatcher->armature.pose[PROPS_LASER_CATCHER_RING_BONE].rotation
        );
    }
    
}