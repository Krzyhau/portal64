#include "laser.h"

#include "../scene/dynamic_scene.h"
#include "../defs.h"

#include "../levels/levels.h"
#include "../levels/cutscene_runner.h"
#include "../physics/collision_scene.h"
#include "../physics/collision.h"
#include "../physics/contact_insertion.h"
#include "../util/memory.h"
#include "../build/assets/materials/static.h"

#include "../math/vector3.h"
#include "../math/quaternion.h"

#define LASER_MAX_SEGMENTS 16
#define LASER_MAX_DISTANCE 128.0f

void laserGenerateSegmentMesh(struct Laser* laser, struct LaserSegment* segment, Vtx* vtx, struct Transform *fromView, int index){

    struct Vector3 laserTangent;
    struct Vector3 laserNormal;
    struct Vector3 laserCotangent;

    vector3Sub(&segment->end, &segment->start, &laserTangent);
    vector3Normalize(&laserTangent, &laserTangent);

    vector3Sub(&fromView->position, &segment->start, &laserNormal);
    vector3Normalize(&laserNormal, &laserNormal);

    vector3Cross(&laserTangent, &laserNormal, &laserCotangent);
    vector3Normalize(&laserCotangent, &laserCotangent);

    Vtx *curr = vtx;

    float width = 0.5f;

    for (int i = 0; i < 4; i++) {
        int posIndex = i >> 1;
        int widthSign = i & 0x1;

        struct Vector3 finalPos = (posIndex == 0) ? segment->start : segment->end;
        vector3AddScaled(&finalPos, &laserCotangent, (widthSign) ? width : -width, &finalPos);

        curr->v.ob[0] = (short)(finalPos.x * SCENE_SCALE);
        curr->v.ob[1] = (short)(finalPos.y * SCENE_SCALE);
        curr->v.ob[2] = (short)(finalPos.z * SCENE_SCALE);

        curr->v.flag = 0;

        // use half of texture for first segment, second half for the rest
        curr->v.tc[0] = posIndex ? (32 << 4): 0;
        if(index > 0) curr->v.tc[0] += (32 << 4);
        curr->v.tc[1] = widthSign ? 0 : ((32 << 5) - 64); // offset to correct dumb texture lol

        curr->v.cn[0] = 255;
        curr->v.cn[1] = 255;
        curr->v.cn[2] = 255;
        curr->v.cn[3] = 255;

        curr++;
    }
}

void laserRender(void *data, struct RenderScene *renderScene, struct Transform *fromView){
    struct Laser* laser = (struct Laser*)data;
    if(laser->numSegments == 0) return;

    Vtx *vertices = renderStateRequestVertices(renderScene->renderState, laser->numSegments * 4);
    Vtx *currVtx = vertices;

    for(int i = 0; i < laser->numSegments; ++i){
        laserGenerateSegmentMesh(laser, &laser->segments[i], currVtx, fromView, i);
        currVtx += 4;
    }

    Gfx *displayList = renderStateAllocateDLChunk(renderScene->renderState, laser->numSegments + ((laser->numSegments + 7) >> 3) + 1);
    Gfx* dl = displayList;

    for (int i = 0; i < laser->numSegments; ++i) {
        int relativeVertex = (i << 2) & 0x1f;

        if (relativeVertex == 0) {
            int verticesLeft = (laser->numSegments - i) << 2;

            if (verticesLeft > 32) {
                verticesLeft = 32;
            }

            gSPVertex(dl++, &vertices[i << 2], verticesLeft, 0);
        }

        gSP2Triangles(
            dl++, 
            relativeVertex, 
            relativeVertex + 1, 
            relativeVertex + 2, 
            0, 
            relativeVertex + 2,
            relativeVertex + 1,
            relativeVertex + 3,
            0
        );
    }

    gSPEndDisplayList(dl++);

    renderSceneAdd(renderScene, displayList, NULL, LASER_BEAM_INDEX, &laser->transform.position, NULL);
}

void laserInit(struct Laser *laser, struct CollisionObject* emitterObject) {
    // don't bother to estimate position and radius of the laser. just make it so big that it's always drawn
    dynamicSceneAddViewDependant(laser, laserRender, &laser->transform.position, 999.0f);

    laser->emitterObject = emitterObject;

    laser->segments = malloc(sizeof(struct LaserSegment) * LASER_MAX_SEGMENTS);
    laserSetActive(laser, 1);

    laser->localDirection = (struct Vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
}

void laserSetActive(struct Laser *laser, int activeState){
    if (activeState) {
        laser->numSegments = 1;
    } else {
        laser->numSegments = 0;
    }
}

void laserUpdatePosition(struct Laser *laser, struct Transform *transform, int roomIndex){
    laser->transform.position = transform->position;
    laser->transform.rotation = transform->rotation;
    laser->transform.scale = transform->scale;
    laser->currentRoom = roomIndex;
}

int laserRaycast(struct Laser* laser, struct Ray* ray, int room, struct RaycastHit *hit)
{
    int result;

    short prevCollisionLayer = 0;

    if (laser->numSegments <= 1 && laser->emitterObject != NULL)
    {
        prevCollisionLayer = laser->emitterObject->collisionLayers;
        laser->emitterObject->collisionLayers = 0;
    }

    result = collisionSceneRaycast(&gCollisionScene, room, ray, COLLISION_LAYERS_BLOCK_LASER, LASER_MAX_DISTANCE, 0, hit);

    if (laser->numSegments <= 1 && laser->emitterObject != NULL)
    {
        laser->emitterObject->collisionLayers = prevCollisionLayer;
    }

    return result;
}

void laserUpdate(struct Laser *laser){
    if(laser->numSegments == 0) return;
    laser->numSegments = 1;

    // first segment used as emitter's starting point
    struct Vector3 firstStart = {.x = 0.0f, .y = 0.0f, .z = 0.0f};
    struct Vector3 firstEnd;
    vector3Scale(&laser->localDirection, &firstEnd, 0.5f);
    transformPoint(&laser->transform, &firstStart, &laser->segments[0].start);
    transformPoint(&laser->transform, &firstEnd, &laser->segments[0].end);

    struct Ray ray;
    ray.origin = laser->segments[0].start;
    vector3Sub(&laser->segments[0].end, &laser->segments[0].start, &ray.dir);
    vector3Normalize(&ray.dir, &ray.dir);

    int currentRoom = laser->currentRoom;
    while(currentRoom >= 0 && laser->numSegments < LASER_MAX_SEGMENTS){
        struct RaycastHit hit;

        if(laserRaycast(laser, &ray, currentRoom, &hit) == 0){
            break;
        }

        laser->segments[laser->numSegments].start = ray.origin;
        laser->segments[laser->numSegments].end = hit.at;

        laser->numSegments++;

        // TODO: logic for when hit a portal

        if(hit.object != NULL && hit.object->laser != NULL){
            hit.object->laser(hit.object->data);
        }

        break;
    }
}
