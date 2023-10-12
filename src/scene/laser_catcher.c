#include "laser_catcher.h"

#include "../scene/dynamic_scene.h"
#include "../defs.h"

#include "../levels/levels.h"
#include "../levels/cutscene_runner.h"
#include "../util/memory.h"

#include "../build/assets/models/dynamic_model_list.h"

#include "../../build/assets/materials/static.h"

#include "../util/dynamic_asset_loader.h"
#include "../../build/assets/models/dynamic_animated_model_list.h"
#include "../../build/assets/models/props/laser_catcher.h"

#include <math.h>


void laserCatcherRender(void* data, struct DynamicRenderDataList* renderList, struct RenderState* renderState) {
    struct LaserCatcher *laserCatcher = (struct LaserCatcher *)data;

    Mtx* matrix = renderStateRequestMatrices(renderState, 1);

    if (!matrix) {
        return;
    }

    transformToMatrixL(&laserCatcher->transform, matrix, SCENE_SCALE);

    Mtx *armature = renderStateRequestMatrices(renderState, PROPS_LASER_CATCHER_DEFAULT_BONES_COUNT);
    if (!armature) {
        return;
    }

    struct SKArmatureWithAnimations *armatureDef = dynamicAssetAnimatedModel(PROPS_LASER_CATCHER_DYNAMIC_ANIMATED_MODEL);

    transformToMatrixL(&armatureDef->armature->pose[PROPS_LASER_CATCHER_ROOT_BONE], &armature[PROPS_LASER_CATCHER_ROOT_BONE], 1.0f);
    transformToMatrixL(&armatureDef->armature->pose[PROPS_LASER_CATCHER_RING_BONE], &armature[PROPS_LASER_CATCHER_RING_BONE], 1.0f);

    dynamicRenderListAddData(
        renderList,
        armatureDef->armature->displayList, 
        matrix, 
        laserCatcher->flags & LaserCatcherFlagsPowered ? LASER_CATCHER_ACTIVATED_INDEX : LASER_CATCHER_INDEX,
        &laserCatcher->transform.position, 
        armature
    );
}

void laserCatcherInit(struct LaserCatcher* laserCatcher, struct LaserCatcherDefinition* definition) {
    dynamicAssetAnimatedModel(PROPS_LASER_CATCHER_DYNAMIC_ANIMATED_MODEL);

    laserCatcher->transform.position = definition->position;
    laserCatcher->transform.rotation = definition->rotation;
    laserCatcher->transform.scale = gOneVec;
    laserCatcher->roomIndex = definition->roomIndex;
    laserCatcher->signalIndex = definition->signalIndex;

    laserCatcher->dynamicId = dynamicSceneAdd(laserCatcher, laserCatcherRender, &laserCatcher->transform.position, 0.5f);
    dynamicSceneSetRoomFlags(laserCatcher->dynamicId, ROOM_FLAG_FROM_INDEX(laserCatcher->roomIndex));
}

void laserCatcherUpdate(struct LaserCatcher* laserCatcher) {
    
}