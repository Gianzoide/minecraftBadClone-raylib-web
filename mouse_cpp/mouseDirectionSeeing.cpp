#include "../rendering/redering.h"
#include "../Entity_cpp/Entity.h"
#include "../usefullFunc_cpp/usefullFunc.h"
#include <raymath.h>
#include <limits>
#include <cmath>
#include "../chunk_cpp/chunk.h"
#include <cfloat>
#include "../movement_cpp/movement.h"
#include "../particles_cpp/particles.h"

using namespace std;

vector<int3D> getChunkThatEntitySees(
    Vector3 origin,
    Vector3 dir,
    float maxDistance
) {
    vector<int3D> result;

    dir = Vector3Normalize(dir);

    // Current chunk (world space!)
    int3D current = getCoordChunk_By_positionTimesChunk(origin);

    int stepX = (dir.x > 0) ? 1 : -1;
    int stepY = (dir.y > 0) ? 1 : -1;
    int stepZ = (dir.z > 0) ? 1 : -1;

    float tMaxX, tMaxY, tMaxZ;
    float tDeltaX, tDeltaY, tDeltaZ;

    float chunkSizeX = chunkXlength;
    float chunkSizeY = chunkYlength;
    float chunkSizeZ = chunkZlength;

    // Compute initial tMax and tDelta
    auto compute = [&](float originCoord, float dirCoord, float chunkSize, int step, int chunkCoord, float& tMax, float& tDelta) {
        if (dirCoord == 0.0f) {
            tMax = tDelta = INFINITY;
            return;
        }

        float nextBoundary = (step > 0)
            ? (chunkCoord + 1) * chunkSize
            : chunkCoord * chunkSize;

        tMax = (nextBoundary - originCoord) / dirCoord;
        tDelta = chunkSize / fabs(dirCoord);
        };

    compute(origin.x, dir.x, chunkSizeX, stepX, current.x, tMaxX, tDeltaX);
    compute(origin.y, dir.y, chunkSizeY, stepY, current.y, tMaxY, tDeltaY);
    compute(origin.z, dir.z, chunkSizeZ, stepZ, current.z, tMaxZ, tDeltaZ);

    float distance = 0.0f;

    while (distance < maxDistance) {
        result.push_back(current);

        if (tMaxX < tMaxY && tMaxX < tMaxZ) {
            current.x += stepX;
            distance = tMaxX;
            tMaxX += tDeltaX;
        }
        else if (tMaxY < tMaxZ) {
            current.y += stepY;
            distance = tMaxY;
            tMaxY += tDeltaY;
        }
        else {
            current.z += stepZ;
            distance = tMaxZ;
            tMaxZ += tDeltaZ;
        }
    }

    return result;
}

bool seesBlock(Vector3 origin, Vector3 dir, float maxDist, Vector3 blockPos, Vector3 blockSize) {
    dir = Vector3Normalize(dir);

    Vector3 boxMin = blockPos;
    Vector3 boxMax = Vector3Add(blockPos, blockSize);

    float tmin = 0.0f;
    float tmax = maxDist;

    // X
    if (fabs(dir.x) < 1e-6f) {
        if (origin.x < boxMin.x || origin.x > boxMax.x) return false;
    }
    else {
        float tx1 = (boxMin.x - origin.x) / dir.x;
        float tx2 = (boxMax.x - origin.x) / dir.x;

        float t1 = fmin(tx1, tx2);
        float t2 = fmax(tx1, tx2);

        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
    }

    // Y
    if (fabs(dir.y) < 1e-6f) {
        if (origin.y < boxMin.y || origin.y > boxMax.y) return false;
    }
    else {
        float ty1 = (boxMin.y - origin.y) / dir.y;
        float ty2 = (boxMax.y - origin.y) / dir.y;

        float t1 = fmin(ty1, ty2);
        float t2 = fmax(ty1, ty2);

        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
    }

    // Z
    if (fabs(dir.z) < 1e-6f) {
        if (origin.z < boxMin.z || origin.z > boxMax.z) return false;
    }
    else {
        float tz1 = (boxMin.z - origin.z) / dir.z;
        float tz2 = (boxMax.z - origin.z) / dir.z;

        float t1 = fmin(tz1, tz2);
        float t2 = fmax(tz1, tz2);

        tmin = fmax(tmin, t1);
        tmax = fmin(tmax, t2);
    }

    // final check
    return tmax >= tmin && tmin <= maxDist && tmax >= 0.0f;
}

bool checkIfItsTheClosest(
	collisionBox* closestCollisionSeen,
	Vector3 closestPos,
	collisionBox& collision,
	Vector3 blockWorldPosition,
	Vector3 origin
) {
	if (closestCollisionSeen == nullptr) {
		return true;
	}

	float oldDist = Vector3Distance(origin, closestPos);
	float newDist = Vector3Distance(origin, blockWorldPosition);

	return newDist < oldDist;
}

void updateRenderingAdjecentChunks(int3D chunk, Vector3 blockCoord){
    gottenChunks[chunk.x][chunk.y][chunk.z] = gottenchunksStates::update;
    if (blockCoord.x >= chunkXlength - 1 && !(chunk.x >= renderDistanceDiameter - 1)) { //x
        gottenChunks[chunk.x + 1][chunk.y    ][chunk.z    ] = gottenchunksStates::update;
    }
    if (blockCoord.x <= 0 && !(chunk.x <= 0)) {
        gottenChunks[chunk.x - 1][chunk.y    ][chunk.z    ] = gottenchunksStates::update;
    }
    if (blockCoord.y >= chunkYlength - 1 && !(chunk.y >= renderDistanceDiameter - 1)) { //y
        gottenChunks[chunk.x    ][chunk.y + 1][chunk.z    ] = gottenchunksStates::update;
    }
    if (blockCoord.y <= 0 && !(chunk.y <= 0)) {
        gottenChunks[chunk.x    ][chunk.y - 1][chunk.z    ] = gottenchunksStates::update;
    }
    if (blockCoord.z >= chunkZlength - 1 && !(chunk.z >= renderDistanceDiameter - 1)) { //z
        gottenChunks[chunk.x    ][chunk.y    ][chunk.z + 1] = gottenchunksStates::update;
    }
    if (blockCoord.z <= 0 && !(chunk.z <= 0)) {
        gottenChunks[chunk.x    ][chunk.y    ][chunk.z - 1] = gottenchunksStates::update;
    }
}

bool rayHitAABB(Vector3 origin, Vector3 dir, float maxDist,
    Vector3 boxPos, Vector3 boxSize,
    float& tHit)
{
    Vector3 invDir = { 1.0f / dir.x, 1.0f / dir.y, 1.0f / dir.z };

    Vector3 t1 = Vector3Multiply(Vector3Subtract(boxPos, origin), invDir);
    Vector3 t2 = Vector3Multiply(Vector3Subtract(Vector3Add(boxPos, boxSize), origin), invDir);

    Vector3 tmin = Vector3Min(t1, t2);
    Vector3 tmax = Vector3Max(t1, t2);

    float tNear = fmax(fmax(tmin.x, tmin.y), tmin.z);
    float tFar = fmin(fmin(tmax.x, tmax.y), tmax.z);

    if (tNear > tFar || tFar < 0.0f || tNear > maxDist)
        return false;

    tHit = tNear;
    return true;
}

int3D worldChunkToRenderChunk(const int3D& worldChunk)
{
    return {
        worldChunk.x - CenterCoords.x + renderDistanceDiameterHalfed,
        worldChunk.y - CenterCoords.y + renderDistanceDiameterHalfed,
        worldChunk.z - CenterCoords.z + renderDistanceDiameterHalfed
    };
}

void hoveringBlock(Entity& entity)
{
    bool rightClicked = false;
    bool Left_Clicked = false;
    bool Left_ClickedHolding = false;

    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) rightClicked = true;
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) Left_Clicked = true;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) Left_ClickedHolding = true;

    Vector3 origin = Vector3Add(entity.positionTimesChunk, entity.cameraVisionposition);
    int3D closestWorldChunk = {};

    Vector3 newDirection = Vector3Normalize(DirectionForward);

    vector<int3D> chunkThatEntitySees = getChunkThatEntitySees(origin, newDirection, entity.reachabilityDistance);

    blockInfo* closestBlockIt = nullptr;
    int closestBlockItCoord = 0;
    Chunk* closestChunk = nullptr;
    collisionBox* closestCollisionSeen = nullptr;

    Vector3 closestBlockSeenPos = { 0,0,0 };
    float closestT = std::numeric_limits<float>::max();

    bool found = false;

    Vector3 hitPoint = { 0,0,0 };
    Vector3 hitNormal = { 0,0,0 };

    for (int3D& coords : chunkThatEntitySees)
    {
        Chunk* chunk = getChunk(coords.x, coords.y, coords.z);
        if (!chunk) continue;

        Vector3 chunkOffsetWorld = {
            (float)coords.x * chunkXlength,
            (float)coords.y * chunkYlength,
            (float)coords.z * chunkZlength
        };
       
        ChunkBlocks& chunkblocks = chunk->blocks;
        
        //for (auto it = chunk->blocks.begin(); it != chunk->blocks.end(); ++it)
        for(int index = 0; index < chunkXYZ_TOTALSIZE; index++)
        {
            int3D littleBruh = IndexToXYZ(index);
            blockInfo& block = chunkblocks[index];
            if (block.id == 0) {
                continue;
            }
            for (collisionBox& collision : blockIdDefinition[block.id].mouseInteraction)
            {
                Vector3 blockWorldPosition = collision.startposition;
                blockWorldPosition = Vector3Add(blockWorldPosition, chunkOffsetWorld);
                blockWorldPosition = { blockWorldPosition.x + littleBruh.x, blockWorldPosition.y + littleBruh.y, blockWorldPosition .z + littleBruh .z};
                float tHit;
                if (!rayHitAABB(origin, newDirection, entity.reachabilityDistance,
                    blockWorldPosition, collision.size, tHit))
                    continue;

                if (tHit < closestT)
                {
                    closestT = tHit;

                    closestBlockIt = &block;
                    closestBlockItCoord = index;
                    closestChunk = chunk;
                    closestCollisionSeen = &collision;

                    closestBlockSeenPos = blockWorldPosition;

                    closestWorldChunk = coords;

                    // compute hit point
                    hitPoint = Vector3Add(origin, Vector3Scale(newDirection, tHit));

                    // detect face normal
                    Vector3 min = blockWorldPosition;
                    Vector3 max = Vector3Add(blockWorldPosition, collision.size);

                    float e = 0.001f;
                    hitNormal = { 0,0,0 };

                    if (fabs(hitPoint.x - min.x) < e) hitNormal = { -1,0,0 };
                    else if (fabs(hitPoint.x - max.x) < e) hitNormal = { 1,0,0 };
                    else if (fabs(hitPoint.y - min.y) < e) hitNormal = { 0,-1,0 };
                    else if (fabs(hitPoint.y - max.y) < e) hitNormal = { 0,1,0 };
                    else if (fabs(hitPoint.z - min.z) < e) hitNormal = { 0,0,-1 };
                    else if (fabs(hitPoint.z - max.z) < e) hitNormal = { 0,0,1 };

                    found = true;
                }
            }
        }
    }

    if (!found) return;

    int3D rendercoordChunk = worldChunkToRenderChunk(closestWorldChunk);
 
    Vector3 chunkCoordToTheRender = {
        (float)closestWorldChunk.x * chunkXlength,
        (float)closestWorldChunk.y * chunkYlength,
        (float)closestWorldChunk.z * chunkZlength
    };

    int3D littleBruh = IndexToXYZ(closestBlockItCoord);
    Vector3 baseCoord = { (float)littleBruh.x, (float)littleBruh.y, (float)littleBruh.z };
    Vector3 placeBaseCoord = Vector3Add(baseCoord, hitNormal);
    // ================= DELETE =================
    if (Left_ClickedHolding)
    {
        int3D chunkCoord = int3D{ closestChunk->x, closestChunk->y, closestChunk->z };
        PlayerGamemode playersMode = main_player->gamemode;
        if (playersMode == PlayerGamemode::creative || playersMode == PlayerGamemode::spectator) {
            if (Left_Clicked) {
                createParticles(chunkCoord, baseCoord, closestBlockIt->id);
                closestChunk->blocks[closestBlockItCoord] = {};
                closestChunk->quatityBlocks--;
                updateRenderingAdjecentChunks(rendercoordChunk, baseCoord);
            }
        }
        else {
            if (Left_Clicked) {
                createParticles(chunkCoord, baseCoord, closestBlockIt->id);
            }
            BlockID blockIdToBreakingAnimation = closestChunk->blocks[closestBlockItCoord].id;
            Shape shapeToAnimation = blockIdDefinition[blockIdToBreakingAnimation].shapeTexture;
            blockyTexture textureToAnimation = blockIdDefinition[blockIdToBreakingAnimation].texture;
            Rotation rotationBlock = closestChunk->blocks[closestBlockItCoord].rotation;

            uint8_t i = 0;
            for (auto& faces : ChooseshapeToRender[(int)shapeToAnimation]) {
                AtlasTile& textureDirection = AtlasTile{0,0,1,1};
                for (SquareFace& face : faces) {

                    auto fac = face.face;
                    for (int v = 0; v < 4; v++) {

                        fac[v] = rotateXYZ_centered(
                            fac[v],
                            rotX(rotationBlock),
                            rotY(rotationBlock),
                            rotZ(rotationBlock)
                        );
                    }
                    // 2. Calculate the normal AFTER rotation
                    Vector3 edge1 = Vector3Subtract(fac[1], fac[0]);
                    Vector3 edge2 = Vector3Subtract(fac[2], fac[0]);

                    Vector3 faceNormal = Vector3Normalize(Vector3CrossProduct(edge1, edge2));
                    Vector3 offset = Vector3Scale(faceNormal, 0.01f);

                    for (int v = 0; v < 4; v++) {
                        fac[v] = Vector3Add(fac[v], offset);
                        fac[v] = Vector3Add(fac[v], baseCoord); 
                        fac[v] = Vector3Add(fac[v], chunkCoordToTheRender);
                    }
                    DrawItemFace(fac[0],fac[1],fac[2],fac[3],textureDirection, breakingAnimation);
                }
                i++;
            }
        }
    }

    // ================= PLACE =================
    if (rightClicked)
    {
        ItemInfo thingPlayerIsHolding;
        thingPlayerIsHolding = main_player->getThingPlayerHolding();

        if (thingPlayerIsHolding.whereSlot < 0) {
            goto endThis;
        }

        Chunk* chunk = closestChunk;
    
        int3D newRendercoordChunk = rendercoordChunk;

        // handle chunk borders
        if (placeBaseCoord.x < 0) {
            chunk = getChunk(closestChunk->x - 1, closestChunk->y, closestChunk->z);
            newRendercoordChunk.x--;
            placeBaseCoord.x += chunkXlength;
        }
        else if (placeBaseCoord.x >= chunkXlength) {
            chunk = getChunk(closestChunk->x + 1, closestChunk->y, closestChunk->z);
            placeBaseCoord.x -= chunkXlength;
            newRendercoordChunk.x++;
        }

        if (placeBaseCoord.y < 0) {
            chunk = getChunk(closestChunk->x, closestChunk->y - 1, closestChunk->z);
            newRendercoordChunk.y--;
            placeBaseCoord.y += chunkYlength;
        }
        else if (placeBaseCoord.y >= chunkYlength) {
            chunk = getChunk(closestChunk->x, closestChunk->y + 1, closestChunk->z);
            placeBaseCoord.y -= chunkYlength;
            newRendercoordChunk.y++;
        }

        if (placeBaseCoord.z < 0) {
            chunk = getChunk(closestChunk->x, closestChunk->y, closestChunk->z - 1);
            newRendercoordChunk.z--;
            placeBaseCoord.z += chunkZlength;
        }
        else if (placeBaseCoord.z >= chunkZlength) {
            chunk = getChunk(closestChunk->x, closestChunk->y, closestChunk->z + 1);
            placeBaseCoord.z -= chunkZlength;
            newRendercoordChunk.z++;
        }

        //rotation based on entity direction
        Vector3 flatDir = { newDirection.x, newDirection.y, 0.0f };
        flatDir = Vector3Normalize(flatDir);

        //Rotation rotEntityView = makeRotation(0, 0, 0);
        int3D rotEntityView = {0,0,0};

        if (fabs(flatDir.x) > fabs(flatDir.y))
        {
            //rotEntityView = makeRotation(0, 0, flatDir.x > 0 ? 1 : 3);
            rotEntityView.z = flatDir.x > 0 ? 1 : 3;
        }
        else
        {
            //rotEntityView = makeRotation(0, 0, flatDir.y > 0 ? 2 : 0);
            rotEntityView.z = flatDir.y > 0 ? 2 : 0 ;
        }

        if (hitNormal.z < 0)
        {
            rotEntityView = { rotEntityView.x, 2, rotEntityView.z};
            //rotEntityView = makeRotation(
            //    rotX(rotEntityView),
            //    2,
            //    rotZ(rotEntityView)
            //);
        }
        
        BlockID blockId = thingPlayerIsHolding.blockId;
        //chunk->blocks.emplace_back(
        blockInfo theBlo;
        theBlo.id = blockId;
        int3D myLittlePony = { (int)placeBaseCoord.x, (int)placeBaseCoord.y, (int)placeBaseCoord.z };
        theBlo.rotation = getBlockDetermedRotation(blockIdDefinition[blockId].rotationDetermination, myLittlePony, rotEntityView);

        //blockInfo& newBlock = chunk->blocks.back();

        //getMeshChunk(blocksPerformaceRender, newRendercoordChunk.x, newRendercoordChunk.y, newRendercoordChunk.z)[(int)placeBaseCoord.x][(int)placeBaseCoord.y][(int)placeBaseCoord.z] = &newBlock;
        
        chunk->blocks[Index(placeBaseCoord.x, placeBaseCoord.y, placeBaseCoord.z)] = theBlo;
        chunk->quatityBlocks++;
        updateRenderingAdjecentChunks(newRendercoordChunk, placeBaseCoord);
    }
    endThis:
    
    // ================= OUTLINE =================
    Vector3 renderPos = Vector3Add(baseCoord, chunkCoordToTheRender);

    for (collisionBox& collision : blockIdDefinition[closestBlockIt->id].mouseInteraction)
    {
        Vector3 size = collision.size;
        Vector3 center = Vector3Add(renderPos, collision.startposition);
        center = Vector3Add(center, Vector3Scale(size, 0.5f));

        for (int i = 0; i < 3; i++)
        {
            float scale = 1.0f + i * 0.005f;

            DrawCubeWires(
                center,
                size.x * scale,
                size.y * scale,
                size.z * scale,
                BLACK
            );
        }
    }
}