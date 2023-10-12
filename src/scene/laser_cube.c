#include "laser_cube.h"

#include "defs.h"
#include "../physics/collision_box.h"
#include "../physics/collision_scene.h"
#include "dynamic_scene.h"
#include "scene.h"
#include "../util/time.h"
#include "../levels/cutscene_runner.h"
#include "../decor/decor_object.h"
#include "../util/dynamic_asset_loader.h"

#include "../build/assets/materials/static.h"

#include "../build/assets/models/dynamic_model_list.h"

struct CollisionBox gLaserCubeCollisionBox = {
    {0.3165f, 0.3165f, 0.3165f}
};

struct ColliderTypeData gLaserCubeCollider = {
    CollisionShapeTypeBox,
    &gLaserCubeCollisionBox,
    0.0f,
    0.5f,
    &gCollisionBoxCallbacks
};

void laserCubeRender(void *data, struct DynamicRenderDataList *renderList, struct RenderState *renderState) {
    struct LaserCube *laserCube = (struct LaserCube*)data;

    Mtx* matrix = renderStateRequestMatrices(renderState, 1);

    if (!matrix) {
        return;
    }

    transformToMatrixL(&laserCube->rigidBody.transform, matrix, SCENE_SCALE);

    short materialIndex = LASER_CUBE_INDEX;
    if (laserCube->fizzleTime > 0.0f)
    {
        materialIndex = LASER_CUBE_FIZZLED_INDEX;
    }
    else if (laserCube->flags & LaserCubeFlagsPowered)
    {
        materialIndex = LASER_CUBE_ACTIVATED_INDEX;
    }

    dynamicRenderListAddDataTouchingPortal(
        renderList, 
        decorBuildFizzleGfx(dynamicAssetModel(CUBE_LASER_CUBE_DYNAMIC_MODEL), laserCube->fizzleTime, renderState), 
        matrix, 
        materialIndex,
        &laserCube->rigidBody.transform.position, 
        NULL,
        laserCube->rigidBody.flags
    );
}

void laserCubeInit(struct LaserCube *laserCube, struct LaserCubeDefinition *definition) {
    dynamicAssetModelPreload(CUBE_LASER_CUBE_DYNAMIC_MODEL);

    collisionObjectInit(&laserCube->collisionObject, &gLaserCubeCollider, &laserCube->rigidBody, 1.0f, COLLISION_LAYERS_TANGIBLE | COLLISION_LAYERS_GRABBABLE | COLLISION_LAYERS_FIZZLER);
    collisionSceneAddDynamicObject(&laserCube->collisionObject);

    laserCube->rigidBody.transform.position = definition->position;
    laserCube->rigidBody.transform.rotation = definition->rotation;
    laserCube->rigidBody.transform.scale = gOneVec;
    laserCube->rigidBody.currentRoom = definition->roomIndex;
    laserCube->rigidBody.flags |= RigidBodyFlagsGrabbable;

    collisionObjectUpdateBB(&laserCube->collisionObject);

    laserCube->dynamicId = dynamicSceneAdd(laserCube, laserCubeRender, &laserCube->rigidBody.transform.position, 0.4f);

    dynamicSceneSetRoomFlags(laserCube->dynamicId, ROOM_FLAG_FROM_INDEX(laserCube->rigidBody.currentRoom));

    laserCube->fizzleTime = 0.0f;
}

void laserCubeUpdate(struct LaserCube* laserCube) {
    if (decorObjectUpdateFizzler(&laserCube->collisionObject, &laserCube->fizzleTime) == FizzleCheckResultEnd) {
        dynamicSceneRemove(laserCube->dynamicId);
        collisionSceneRemoveDynamicObject(&laserCube->collisionObject);
        laserCube->dynamicId = INVALID_DYNAMIC_OBJECT;
    }

    dynamicSceneSetRoomFlags(laserCube->dynamicId, ROOM_FLAG_FROM_INDEX(laserCube->rigidBody.currentRoom));
}