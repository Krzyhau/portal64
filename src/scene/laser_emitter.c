#include "laser_emitter.h"

#include "../scene/dynamic_scene.h"
#include "../defs.h"

#include "../levels/levels.h"
#include "../levels/cutscene_runner.h"
#include "../util/memory.h"

#include "../build/assets/models/dynamic_model_list.h"

#include "../../build/assets/materials/static.h"

#include "../util/dynamic_asset_loader.h"

#include <math.h>


void laserEmitterRender(void* data, struct DynamicRenderDataList* renderList, struct RenderState* renderState) {
    struct LaserEmitter *laser = (struct LaserEmitter *)data;

    Mtx* matrix = renderStateRequestMatrices(renderState, 1);

    if (!matrix) {
        return;
    }

    transformToMatrixL(&laser->transform, matrix, SCENE_SCALE);

    dynamicRenderListAddData(
        renderList,
        dynamicAssetModel(PROPS_LASER_EMITTER_DYNAMIC_MODEL),
        matrix,
        LASER_EMITTER_INDEX,
        &laser->transform.position,
        NULL
    );
}

void laserEmitterInit(struct LaserEmitter* laser, struct LaserEmitterDefinition* definition) {
    dynamicAssetModelPreload(PROPS_LASER_EMITTER_DYNAMIC_MODEL);

    laser->transform.position = definition->position;
    laser->transform.rotation = definition->rotation;
    laser->transform.scale = gOneVec;
    laser->roomIndex = definition->roomIndex;
    laser->signalIndex = definition->signalIndex;

    int dynamicId = dynamicSceneAdd(laser, laserEmitterRender, &laser->transform.position, 0.5f);
    dynamicSceneSetRoomFlags(dynamicId, ROOM_FLAG_FROM_INDEX(laser->roomIndex));
}

void laserEmitterUpdate(struct LaserEmitter* laser) {
    
}