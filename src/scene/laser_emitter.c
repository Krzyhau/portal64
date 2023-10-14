#include "laser_emitter.h"

#include "../scene/dynamic_scene.h"
#include "../defs.h"

#include "../levels/levels.h"
#include "../levels/cutscene_runner.h"
#include "../physics/collision_scene.h"
#include "../util/memory.h"

#include "../build/assets/models/dynamic_model_list.h"

#include "../../build/assets/materials/static.h"

#include "../util/dynamic_asset_loader.h"

#include <math.h>


void laserEmitterRender(void* data, struct DynamicRenderDataList* renderList, struct RenderState* renderState) {
    struct LaserEmitter *laserEmitter = (struct LaserEmitter *)data;

    Mtx* matrix = renderStateRequestMatrices(renderState, 1);

    if (!matrix) {
        return;
    }

    transformToMatrixL(&laserEmitter->transform, matrix, SCENE_SCALE);

    dynamicRenderListAddData(
        renderList,
        dynamicAssetModel(PROPS_LASER_EMITTER_DYNAMIC_MODEL),
        matrix,
        LASER_EMITTER_INDEX,
        &laserEmitter->transform.position,
        NULL
    );
}

void laserEmitterInit(struct LaserEmitter* laserEmitter, struct LaserEmitterDefinition* definition) {
    dynamicAssetModelPreload(PROPS_LASER_EMITTER_DYNAMIC_MODEL);

    laserEmitter->transform.position = definition->position;
    laserEmitter->transform.rotation = definition->rotation;
    laserEmitter->transform.scale = gOneVec;
    laserEmitter->roomIndex = definition->roomIndex;
    laserEmitter->signalIndex = definition->signalIndex;

    int dynamicId = dynamicSceneAdd(laserEmitter, laserEmitterRender, &laserEmitter->transform.position, 0.5f);
    dynamicSceneSetRoomFlags(dynamicId, ROOM_FLAG_FROM_INDEX(laserEmitter->roomIndex));

    laserEmitter->ownLaser = malloc(sizeof(struct Laser));
    laserInit(laserEmitter->ownLaser, NULL);
}

void laserEmitterUpdate(struct LaserEmitter* laserEmitter) {
    laserUpdatePosition(laserEmitter->ownLaser, &laserEmitter->transform, laserEmitter->roomIndex);
    laserUpdate(laserEmitter->ownLaser);
}