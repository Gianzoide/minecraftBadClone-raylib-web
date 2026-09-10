
#include <iostream>

#include <raylib.h>
#include "../chunk_cpp/chunk.h"
#include "../usefullFunc_cpp/usefullFunc.h"
#include "rlgl.h"
#include "../blocks.h"
#include "../camera_cpp/camera.h"
#include <algorithm>
#include "redering.h"
#include "../Entity_cpp/Entity.h"
using namespace std;

#include <string>
#include <vector> 
#include <mutex> 

template<typename T>
T& getMeshChunk(
    std::list<std::list<std::list<T>>>& mesh,
    int x,
    int y,
    int z)
{
    auto itX = mesh.begin();
    std::advance(itX, x);

    auto itY = itX->begin();
    std::advance(itY, y);

    auto itZ = itY->begin();
    std::advance(itZ, z);

    return *itZ;
}

//get chunk and put it all the blocks that are good to face render and put those into the blocksPerformaceRender list 
//BlockPtrs3D blocksPerformaceRender(
//    renderDistanceDiameter,
//    list<list<vector<vector<vector<blockInfo*>>>>>(
//        renderDistanceDiameter,
//        list<vector<vector<vector<blockInfo*>>>>(
//            renderDistanceDiameter,
//            vector<vector<vector<blockInfo*>>>(
//                chunkXlength,
//                vector<vector<blockInfo*>>(
//                    chunkYlength,
//                    vector<blockInfo*>(
//                        chunkHight,
//                        nullptr
//                    )
//                )
//            )
//        )
//    )
//);


//vector < blockInfo*> exoticBlocksRender = {};
//vector <array<int, 3>> exoticBlocksRender_ExtraCoordidentifier = {};
//
//BlockPtrs3DVectorPart blockyPerfChunClrener(BlockPtrs3DVectorPart(
//    chunkXlength,
//    vector<vector<blockInfo*>>(
//        chunkYlength,
//        vector<blockInfo*>(
//            chunkHight,
//            nullptr
//        )
//    )
//));
//
mutex gottenChunksMutex;
//void getChunks_goodBlocksTo_blocksPerformaceRender(Chunk& chunk, int3D& centerSnapshot) {
//
//    int toIndexizyTheChunkcoord = middle(renderDistanceDiameter);
//
//    //cout << endl << ringX << "/" << ringY << "/" << ringZ;
//
//    int3D ring = { chunk.x - centerSnapshot.x + toIndexizyTheChunkcoord, chunk.y - centerSnapshot.y + toIndexizyTheChunkcoord, chunk.z - centerSnapshot.z + toIndexizyTheChunkcoord };
//
//    BlockPtrs3DVectorPart& theChunkBruh = getMeshChunk(blocksPerformaceRender, ring.x, ring.y, ring.z);
//    theChunkBruh = blockyPerfChunClrener; // to erase
//
//	for (auto& block : chunk.blocks) {
//        auto& blockCoord = block.coord;
//
//        if (hasAllZerosAfterDot(blockCoord.x) && hasAllZerosAfterDot(blockCoord.y) && hasAllZerosAfterDot(blockCoord.z)
//            &&(block.blockShape == shape::cube || block.blockShape == shape::halfcube || block.blockShape == shape::stair)) {
//
//            theChunkBruh[blockCoord.x][blockCoord.y][blockCoord.z] = &block;
//
//			//blocksPerformaceRender[x][y][z] = &block; //cuby-easy shapes
//			continue;
//		}
//		if (block.blockShape == shape::exotic){ // for complex shapes
//
//			exoticBlocksRender.push_back(&block);
//			exoticBlocksRender_ExtraCoordidentifier.push_back({
//				chunkXlength * chunk.x,
//				chunkYlength * chunk.y,
//				chunkHight * chunk.z
//				});
//			continue;
//		}
//	}
//}

//then with blocksPerformaceRender and ExoticBlocksRender render the faces
inline Vector3 rotateY(const Vector3& v, int rotation) {
    switch (rotation % 4) {
        case 0: return v;
        case 1: return { v.z, v.y, -v.x };
        case 2: return { -v.x, v.y, -v.z };
        case 3: return { -v.z, v.y, v.x };
    }
    return v;
}
inline Vector3 rotateX(const Vector3& v, int rotation) {
    switch (rotation % 4) {
        case 0: return v;
        case 1: return { v.x, -v.z, v.y };
        case 2: return { v.x, -v.y, -v.z };
        case 3: return { v.x, v.z, -v.y };
    }
    return v;
}
inline Vector3 rotateZ(const Vector3& v, int rotation) {
    switch (rotation % 4) {
    case 0: return v;
    case 1: return { -v.y, v.x, v.z };
    case 2: return { -v.x, -v.y, v.z };
    case 3: return { v.y, -v.x, v.z };
    }
    return v;
}
inline Vector3 rotateXYZ(Vector3 v, int rotX, int rotY, int rotZ) {
    v = rotateZ(v, rotZ);
    v = rotateX(v, rotX);
    v = rotateY(v, rotY);
    return v;
}
inline Vector3 rotateXYZ_centered(Vector3 v, int rotX, int rotY, int rotZ) {
    // move to center
    v.x -= 0.5f;
    v.y -= 0.5f;
    v.z -= 0.5f;

    v = rotateXYZ(v, rotX, rotY, rotZ);

    // move back
    v.x += 0.5f;
    v.y += 0.5f;
    v.z += 0.5f;

    return v;
}
//void BeginItemRenderer()
//{
//    rlDrawRenderBatchActive();
//
//    rlDisableDepthTest();
//    //rlDisableBackfaceCulling();
//
//    rlMatrixMode(RL_PROJECTION);
//    rlPushMatrix();
//    rlLoadIdentity();
//
//    rlOrtho(
//        0.0,
//        GetScreenWidth(),
//        GetScreenHeight(),
//        0.0,
//        -1000.0,
//        1000.0
//    );
//
//    rlMatrixMode(RL_MODELVIEW);
//    rlPushMatrix();
//    rlLoadIdentity();
//}

void EndItemRenderer()
{
    rlDrawRenderBatchActive();

    rlPopMatrix();

    rlMatrixMode(RL_PROJECTION);
    rlPopMatrix();

    rlMatrixMode(RL_MODELVIEW);

    rlEnableDepthTest();
    rlEnableBackfaceCulling();
}

void DrawItemFace(
    const Vector3& v0,
    const Vector3& v1,
    const Vector3& v2,
    const Vector3& v3,
    const AtlasTile& tile,
    const Texture2D& atlas)
{

    rlSetTexture(atlas.id);

    rlBegin(RL_QUADS);
    rlColor4ub(255, 255, 255, 255);

    rlTexCoord2f(tile.u0, tile.v1);
    rlVertex3f(v0.x, v0.y, v0.z);

    rlTexCoord2f(tile.u1, tile.v1);
    rlVertex3f(v1.x, v1.y, v1.z);

    rlTexCoord2f(tile.u1, tile.v0);
    rlVertex3f(v2.x, v2.y, v2.z);

    rlTexCoord2f(tile.u0, tile.v0);
    rlVertex3f(v3.x, v3.y, v3.z);

    rlEnd();

    rlSetTexture(0);
}
//inline void renderOneFaceOfDirection(allFacesChunk& chunkfaces, Vector3 base)
//{
//    int currentTexture = -1;
//    
//    for (facesOfABlock& f : chunkfaces)
//    {
//        if (currentTexture != f.textureID)
//        {
//            if (currentTexture != -1)
//                rlEnd();
//
//            currentTexture = f.textureID;
//            rlSetTexture(currentTexture);
//
//            rlBegin(RL_QUADS);
//            rlColor4ub(255, 255, 255, 255);
//        }
//
//        const auto& face = f.face.face;
//
//        float bx = base.x + f.base.x;
//        float by = base.y + f.base.y;
//        float bz = base.z + f.base.z;
//
//        rlTexCoord2f(0.0f, 0.0f);
//        rlVertex3f(face[0].x + bx, face[0].y + by, face[0].z + bz);
//
//        rlTexCoord2f(1.0f, 0.0f);
//        rlVertex3f(face[1].x + bx, face[1].y + by, face[1].z + bz);
//
//        rlTexCoord2f(1.0f, 1.0f);
//        rlVertex3f(face[2].x + bx, face[2].y + by, face[2].z + bz);
//
//        rlTexCoord2f(0.0f, 1.0f);
//        rlVertex3f(face[3].x + bx, face[3].y + by, face[3].z + bz);
//    }
//
//    if (currentTexture != -1)
//        rlEnd();
//
//    rlSetTexture(0);
//}

// direções base das faces
const Vector3 dirs[6] = {
    { 1, 0, 0 },  // +X
    {-1, 0, 0 },  // -X
    { 0, 1, 0 },  // +Y
    { 0,-1, 0 },  // -Y
    { 0, 0, 1 },  // +Z
    { 0, 0,-1 }   // -Z
};

//// rotações 90° (sentido horário)
inline Vector3 rotXVector3(const Vector3& v) { return { v.x, v.z, -v.y }; }
inline Vector3 rotYVector3(const Vector3& v) { return { -v.z, v.y, v.x }; }
inline Vector3 rotZVector3(const Vector3& v) { return { v.y, -v.x, v.z }; }

//// encontra índice da direção
inline int findDirIndex(const Vector3& v)
{
    if (v.x > 0) return 0;
    if (v.x < 0) return 1;
    if (v.y > 0) return 2;
    if (v.y < 0) return 3;
    if (v.z > 0) return 4;
    return 5;
}

Vector3 rotate_Vector3(Vector3 v, Vector3 rotationDeg)
{
    float rx = rotationDeg.x * DEG2RAD;
    float ry = rotationDeg.y * DEG2RAD;
    float rz = rotationDeg.z * DEG2RAD;

    float cx = cosf(rx), sx = sinf(rx);
    float cy = cosf(ry), sy = sinf(ry);
    float cz = cosf(rz), sz = sinf(rz);

    // Rotate around X
    float y = v.y * cx - v.z * sx;
    float z = v.y * sx + v.z * cx;
    v.y = y;
    v.z = z;

    // Rotate around Y
    float x = v.x * cy + v.z * sy;
    z = -v.x * sy + v.z * cy;
    v.x = x;
    v.z = z;

    // Rotate around Z
    x = v.x * cz - v.y * sz;
    y = v.x * sz + v.y * cz;
    v.x = x;
    v.y = y;

    return v;
}

//inline int direOfIndex(int index, const Rotation& rotation)
//{
//    Vector3 v = dirs[index];
//
//    int rx = rotX(rotation) % 4;
//    int ry = rotY(rotation) % 4;
//    int rz = rotZ(rotation) % 4;
//
//    // Apply rotations in X -> Y -> Z order
//    for (int i = 0; i < rx; i++) v = rotXVector3(v);
//    for (int i = 0; i < ry; i++) v = rotYVector3(v);
//    for (int i = 0; i < rz; i++) v = rotZVector3(v);
//
//    return findDirIndex(v);
//}

constexpr int ROTATION_COUNT = 64;

std::array<std::array<uint8_t, 6>, ROTATION_COUNT> directionLookup;

inline int rotationToIndex(const Rotation r)
{
    return (rotX(r) & 3)
        | ((rotY(r) & 3) << 2)
        | ((rotZ(r) & 3) << 4);
}
void buildDirectionLookup()
{
    for (int rx = 0; rx < 4; rx++)
    {
        for (int ry = 0; ry < 4; ry++)
        {
            for (int rz = 0; rz < 4; rz++)
            {
                int rotationIndex = rx | (ry << 2) | (rz << 4);

                for (int face = 0; face < 6; face++)
                {
                    Vector3 v = dirs[face];

                    switch (rx)
                    {
                    case 1: v = rotXVector3(v); break;
                    case 2: v = rotXVector3(rotXVector3(v)); break;
                    case 3: v = rotXVector3(rotXVector3(rotXVector3(v))); break;
                    }

                    switch (ry)
                    {
                    case 1: v = rotYVector3(v); break;
                    case 2: v = rotYVector3(rotYVector3(v)); break;
                    case 3: v = rotYVector3(rotYVector3(rotYVector3(v))); break;
                    }

                    switch (rz)
                    {
                    case 1: v = rotZVector3(v); break;
                    case 2: v = rotZVector3(rotZVector3(v)); break;
                    case 3: v = rotZVector3(rotZVector3(rotZVector3(v))); break;
                    }

                    directionLookup[rotationIndex][face] = findDirIndex(v);
                }
            }
        }
    }
}
//SquareFace transformFace(const SquareFace& f, rotation rot, Vector3 base) {
//    SquareFace result{};
//
//    for (int i = 0; i < 4; i++) {
//        Vector3 v = rotateXYZ_centered(f.face[i], rot.XDegres, rot.YDegres, rot.ZDegres);
//        result.face[i] = {
//            snap(v.x + base.x),
//            snap(v.y + base.y),
//            snap(v.z + base.z)
//        };
//    }
//
//    return result;
//}
//bool facesMatch(const SquareFace& a, const SquareFace& b,
//    rotation rotA, rotation rotB,
//    Vector3 baseA, Vector3 baseB) {
//
//    SquareFace fa = transformFace(a, rotA, baseA);
//    SquareFace fb = transformFace(b, rotB, baseB);
//
//    int matches = 0;
//
//    for (int i = 0; i < 4; i++) {
//        for (int j = 0; j < 4; j++) {
//            if (
//                fabs(fa.face[i].x - fb.face[j].x) < 0.001f &&
//                fabs(fa.face[i].y - fb.face[j].y) < 0.001f &&
//                fabs(fa.face[i].z - fb.face[j].z) < 0.001f
//                ) {
//                matches++;
//                break;
//            }
//        }
//    }
//
//    return matches == 4;
//}

bool isFaceOccluded(shapeFace blockFace, shapeFace neighborFace) {
    if (blockFace == neighborFace) {
        if ((blockFace == oneForthTopLeftSquare && neighborFace == oneForthTopLeftSquare)
            || (blockFace == oneForthTopRigthSquare && neighborFace == oneForthTopRigthSquare)
            || (blockFace == oneForthBottomLeftSquare && neighborFace == oneForthBottomLeftSquare)
            || (blockFace == oneForthBottomRigthSquare && neighborFace == oneForthBottomRigthSquare)
        ) {
            return false;
        }
        return true;
    }
    //square
    if (neighborFace == square) {
        return true;
    }
    if (blockFace == square && neighborFace != square) {
        return false;
    }

    if ((blockFace == oneForthTopLeftSquare && neighborFace == oneForthTopRigthSquare) || (blockFace == oneForthTopRigthSquare && neighborFace == oneForthTopLeftSquare)) {
        return true;
    }
    return false;
}


inline void putInTheMeshBabyOnlyOneFace(vector<Vertex>& vertices, array <Vector3, 4> face, Rotation rotationBlock, const int3D& verticyCoord, const AtlasTile& textureCoord) {
    Vector3 v[4] = { face[0], face[1], face[2], face[3] };

    // Rotate
    for (Vector3& p : v)
    {
        if (rotationBlock != 0) {
            p = rotateXYZ_centered(
                p,
                rotX(rotationBlock),
                rotY(rotationBlock),
                rotZ(rotationBlock)
            );
        }

        p.x += verticyCoord.x;
        p.y += verticyCoord.y;
        p.z += verticyCoord.z;
    }
    // Triangle 1
    vertices.push_back(Vertex{ v[0].x,v[0].y,v[0].z,textureCoord.u0,textureCoord.v1 });
    vertices.push_back(Vertex{ v[1].x,v[1].y,v[1].z,textureCoord.u1,textureCoord.v1 });
    vertices.push_back(Vertex{ v[2].x,v[2].y,v[2].z,textureCoord.u1,textureCoord.v0 });

    // Triangle 2
    vertices.push_back(Vertex{ v[0].x,v[0].y,v[0].z,textureCoord.u0,textureCoord.v1 });
    vertices.push_back(Vertex{ v[2].x,v[2].y,v[2].z,textureCoord.u1,textureCoord.v0 });
    vertices.push_back(Vertex{ v[3].x,v[3].y,v[3].z,textureCoord.u0,textureCoord.v0 });
}

inline void putInTheMeshBaby(vector<Vertex>& vertices, vector<SquareFace>& faces, Rotation rotationBlock, const int3D& verticyCoord, const AtlasTile& textureCoord){
    for (SquareFace& face : faces)
    {
        Vector3 v[4] = { face.face[0], face.face[1], face.face[2], face.face[3] };

        // Rotate
        for (Vector3& p : v)
        {
            if (rotationBlock != 0) {
                p = rotateXYZ_centered(
                    p,
                    rotX(rotationBlock),
                    rotY(rotationBlock),
                    rotZ(rotationBlock)
                );
            }

            p.x += verticyCoord.x;
            p.y += verticyCoord.y;
            p.z += verticyCoord.z;
        }
        // Triangle 1
        vertices.push_back(Vertex{ v[0].x,v[0].y,v[0].z,textureCoord.u0,textureCoord.v1 });
        vertices.push_back(Vertex{ v[1].x,v[1].y,v[1].z,textureCoord.u1,textureCoord.v1 });
        vertices.push_back(Vertex{ v[2].x,v[2].y,v[2].z,textureCoord.u1,textureCoord.v0 });

        // Triangle 2
        vertices.push_back(Vertex{ v[0].x,v[0].y,v[0].z,textureCoord.u0,textureCoord.v1 });
        vertices.push_back(Vertex{ v[2].x,v[2].y,v[2].z,textureCoord.u1,textureCoord.v0 });
        vertices.push_back(Vertex{ v[3].x,v[3].y,v[3].z,textureCoord.u0,textureCoord.v0 });
    }
}

Chunk* returnChunkSaved(const int3D& coordNextChunk,
    int direction,
    array<Chunk*, 6>& nextChunkSaved)
{
    if (nextChunkSaved[direction] == nullptr)
    {
        nextChunkSaved[direction] = getChunk(coordNextChunk.x, coordNextChunk.y, coordNextChunk.z);
    }

    return nextChunkSaved[direction];
}

mutex uploadMeshToGPUMutex;
void createMeshFaces_ofAChunk(int3D chunkIndex, int3D chunkCoord) {

    array<Chunk*, 6> nextChunkSaved{}; // six side next to a 3d chunk 

    Chunk* chunk = getChunk(chunkCoord.x, chunkCoord.y, chunkCoord.z);
    ChunkBlocks& chunkBlocks = chunk->blocks;

    int maxQuantityOfBlocks = chunk->quatityBlocks;
    int blocksSeen = 0;

    vector<Vertex> vertices;
    vector<Vertex> verticesSemiTransparent;

    const int dx[6] = { 1, -1, 0, 0, 0, 0 };
    const int dy[6] = { 0, 0, 1, -1, 0, 0 };
    const int dz[6] = { 0, 0, 0, 0, 1, -1 };

    int oppositeFace[6] = { 1, 0, 3, 2, 5, 4 };
    uint8_t NextChunkDirection = 0;

    int3D chunkCoordMultiplied = { chunkCoord.x * chunkXlength, chunkCoord.y * chunkYlength, chunkCoord.z * chunkZlength };

    for (int x = 0; x < chunkXlength; ++x) {
        for (int y = 0; y < chunkYlength; ++y) {
            for (int z = 0; z < chunkZlength; ++z) {
                //if (maxQuantityOfBlocks == blocksSeen) {
                //    goto endThisShii;
                //}
                //blocksSeen++;

                if ((x + y + z) % 2 != 0) {
                    continue;
                }

                blockInfo* block = &chunkBlocks[Index(x, y, z)];
                uint8_t blockID = block->id;

                vector<Vertex>* verticesPTR;
                
                FacesToRender& shapeToRender = ChooseshapeToRender[(int)blockIdDefinition[blockID].shapeTexture];
                 
                Rotation blockRotation = block->rotation;
                blockyTexture& blockTexture = blockIdDefinition[blockID].texture;

                int rotIdx = rotationToIndex(blockRotation);
                for (int i = 0; i < 6; i++) {
                    int theIndex = directionLookup[rotIdx][i];
                    int nx = x + dx[i];
                    int ny = y + dy[i];
                    int nz = z + dz[i];

                    AtlasTile& textureAtlasCoord = blockTexture[theIndex];
                    if (textureAtlasCoord.u0 == 99999) {
                        verticesPTR = &verticesSemiTransparent;
                    }
                    else {
                        verticesPTR = &vertices;
                    }

                    vector<SquareFace>& faces = shapeToRender[theIndex];

                    blockInfo* nextBlock;

                    if (nx >= chunkXlength || nx < 0 ||
                        ny >= chunkYlength || ny < 0 ||
                        nz >= chunkZlength || nz < 0)
                    {
                        int3D chunkIndexNext = chunkCoord;
                        int3D nextChunkBlockCoord = {nx, ny, nz};

                        if (nx < 0)
                        {
                            nextChunkBlockCoord.x += chunkXlength; chunkIndexNext.x--; NextChunkDirection = 0;
                        }
                        else if (nx >= chunkXlength)
                        {
                            nextChunkBlockCoord.x -= chunkXlength; chunkIndexNext.x++; NextChunkDirection = 1;
                        }
                        else if (ny < 0)
                        {
                            nextChunkBlockCoord.y += chunkYlength; chunkIndexNext.y--; NextChunkDirection = 2;
                        }
                        else if (ny >= chunkYlength)
                        {
                            nextChunkBlockCoord.y -= chunkYlength; chunkIndexNext.y++; NextChunkDirection = 3;
                        }
                        else if (nz < 0)
                        {
                            nextChunkBlockCoord.z += chunkZlength; chunkIndexNext.z--; NextChunkDirection = 4;
                        }
                        else if (nz >= chunkZlength)
                        {
                            nextChunkBlockCoord.z -= chunkZlength; chunkIndexNext.z++; NextChunkDirection = 5;
                        }

                        Chunk* nextChunk = returnChunkSaved(chunkIndexNext, NextChunkDirection, nextChunkSaved);
                        if (nextChunk == nullptr || nextChunk->wasTerrainGenerated == false)
                        {
                            createChunk(chunkIndexNext.x, chunkIndexNext.y, chunkIndexNext.z);
                            nextChunk = returnChunkSaved(chunkIndexNext, NextChunkDirection, nextChunkSaved);
                            //if (nextChunk == nullptr)
                            //{
                            //    cout << endl << "[error rendering line 500]";
                            //}
                        }

                        nextBlock = &nextChunk->blocks[Index(nextChunkBlockCoord.x, nextChunkBlockCoord.y, nextChunkBlockCoord.z)];

                    }else {
                        nextBlock = &chunkBlocks[Index(nx, ny, nz)];
                    }

                    uint8_t nextBlockID = nextBlock->id;

                    if (nextBlockID == 0 && blockID == 0) {
                        continue;
                    }
                    
                    if (nextBlockID == 0) {
                        int3D verticyCoord = int3D{ x + chunkCoordMultiplied.x, y + chunkCoordMultiplied.y, z + chunkCoordMultiplied.z};
                        putInTheMeshBaby(*verticesPTR, faces, blockRotation, verticyCoord, textureAtlasCoord);
                        continue;
                    }

                    Rotation nextBlockRotation = nextBlock->rotation;
                    //check if next block adjacent face to the corrent block isnt necessary to render
                    int nextOpposite = directionLookup[rotationToIndex(nextBlockRotation)][oppositeFace[i]];

                    //FacesToRender& neighborShapeToRender = ChooseshapeToRender[blockIdDefinition[nextBlockID].shape];
                    vector<SquareFace>& neighborFaces = ChooseshapeToRender[(int)blockIdDefinition[nextBlockID].shapeTexture][nextOpposite];
                    AtlasTile& nextTextureID = blockIdDefinition[nextBlockID].texture[nextOpposite];

                    if (blockID == 0) {
                        int3D nextVerticyCoord = int3D{ nx + chunkCoordMultiplied.x, ny + chunkCoordMultiplied.y, nz + chunkCoordMultiplied.z };
                        putInTheMeshBaby(*verticesPTR, neighborFaces, nextBlockRotation, nextVerticyCoord, nextTextureID);
                        continue;
                    }

                    bool isGlassLike = blockIdDefinition[blockID].glassLike;
                    bool nextIsGlassLike = blockIdDefinition[nextBlockID].glassLike;

                    if (isGlassLike && nextIsGlassLike) {
                        continue;
                    }
                   
                    int3D verticyCoord = int3D{ x + chunkCoordMultiplied.x, y + chunkCoordMultiplied.y, z + chunkCoordMultiplied.z };
                    for (SquareFace& face : faces) { // NOT for neighbor block
                        shapeFace faceShape = face.shape;
                        auto& faceVerticies = face.face;
                        bool foundAnyFaceOccluded = false;
                        if (faceShape == alwaysRender) {
                            putInTheMeshBabyOnlyOneFace(*verticesPTR, faceVerticies, blockRotation, verticyCoord, textureAtlasCoord);
                            continue;
                        }
                        for (SquareFace& nextFace : neighborFaces) {
                            shapeFace neighborFaceShape = nextFace.shape;
                            if (neighborFaceShape == alwaysRender) {
                                continue;
                            }
                            if (isFaceOccluded(faceShape, neighborFaceShape) && nextIsGlassLike == false) {
                                foundAnyFaceOccluded = true;
                                break;
                            }
                        }
                        if (foundAnyFaceOccluded == false) {
                            putInTheMeshBabyOnlyOneFace(*verticesPTR, faceVerticies, blockRotation, verticyCoord, textureAtlasCoord);
                        }
                    }

                    int3D nextVerticyCoord = int3D{ nx + chunkCoordMultiplied.x, ny + chunkCoordMultiplied.y, nz + chunkCoordMultiplied.z };
                    for (SquareFace& nextFace : neighborFaces) { // for neighbor block
                        shapeFace neighborFaceShape = nextFace.shape;
                        auto& nextFaceVerticies = nextFace.face;
                        bool nexFoundAnyFaceOccluded = false;

                        if (neighborFaceShape == alwaysRender) {
                            putInTheMeshBabyOnlyOneFace(*verticesPTR, nextFaceVerticies, nextBlockRotation, nextVerticyCoord, nextTextureID);
                            continue;
                        }
                        for (SquareFace& face : faces) {
                            shapeFace faceShape = face.shape;
                            if (faceShape == alwaysRender) {
                                continue;
                            }
                            if (isFaceOccluded(neighborFaceShape, faceShape) && isGlassLike == false) {
                                nexFoundAnyFaceOccluded = true;
                                break;
                            }
                        }
                        if (nexFoundAnyFaceOccluded == false) {
                            putInTheMeshBabyOnlyOneFace(*verticesPTR, nextFaceVerticies, nextBlockRotation, nextVerticyCoord, nextTextureID);
                        }
                    }
                    
                }
            }
        }
    }

endThisShii:
    Mesh& meshChunk = getMeshChunk(allMeshFacesChunk, chunkIndex.x, chunkIndex.y, chunkIndex.z).chunkMesh;
    meshChunk = {};

    for (Vertex& verticeST : verticesSemiTransparent) {
        vertices.push_back(verticeST);
    }
    
    //if (vertices.empty()) {
    //    return;
    //}

    if (meshChunk.vertexCount != 0 ) {
        UnloadMesh(meshChunk);
    }

    meshChunk.vertexCount = (int)vertices.size();
    meshChunk.triangleCount = meshChunk.vertexCount / 3;

    meshChunk.vertices =
        (float*)RL_MALLOC(meshChunk.vertexCount * 3 * sizeof(float));

    meshChunk.texcoords =
        (float*)RL_MALLOC(meshChunk.vertexCount * 2 * sizeof(float));

    float* pos = meshChunk.vertices;
    float* uv = meshChunk.texcoords;
    for (const Vertex& v : vertices)
    {
        *pos++ = v.x;
        *pos++ = v.y;
        *pos++ = v.z;

        *uv++ = v.u;
        *uv++ = v.v;
    }
    lock_guard<mutex> lock(uploadMeshToGPUMutex);
    uploadMeshToGPU.push(&meshChunk);

    //DrawMesh(mesh, {}, MatrixIdentity());
    //for (int i = 0; i < exoticBlocksRender.size(); i++) {

    //    auto& block = exoticBlocksRender[i];

    //    Vector3 pos = {
    //        (float)block->x + exoticBlocksRender_ExtraCoordidentifier[i][0],
    //        (float)block->y + exoticBlocksRender_ExtraCoordidentifier[i][1],
    //        (float)block->z + exoticBlocksRender_ExtraCoordidentifier[i][2] };
    //}
}

Material blockMaterial;

void initRendering()
{
    blockMaterial = LoadMaterialDefault();
    blockMaterial.shader = cutoutShader;
    blockMaterial.maps[MATERIAL_MAP_DIFFUSE].texture = atlas;

    cutoutShader = LoadShader(0, "./shaders/cutout.fs");

    if (cutoutShader.id == rlGetShaderIdDefault())
    {
        TraceLog(LOG_ERROR, "cutout shader failed to load! Falling back to default.");
    }
}

void renderBlocks() {
    rlEnableBackfaceCulling();
    //rlDisableBackfaceCulling();

    // Collect transparent faces
    Vector3 camPosition = camera.position;
    Vector3 camTarget   = camera.target;

    // Render opaque blocks
    Vector3 camDir = Vector3Normalize(
        Vector3Subtract(camTarget, camPosition)
    );

    int3D actuallyCenterCoords = getCoordChunk_By_positionTimesChunk(main_player->positionTimesChunk);
    int3D chunkingDiffIntCoord = { actuallyCenterCoords.x - CenterCoords.x, actuallyCenterCoords.y - CenterCoords.y, actuallyCenterCoords.z - CenterCoords.z };
    int halfefdchunkXlength = chunkXlength * 0.5f;
    int halfefdchunkYlength = chunkYlength * 0.5f;
    int halfefdchunkHight = chunkZlength * 0.5f;

    struct ChunkRenderInfo
    {
        allMeshes* mesh;
        float distanceSq;
    };

    std::vector<ChunkRenderInfo> chunksToRender;
    chunksToRender.reserve(renderDistanceDiameter * renderDistanceDiameter * renderDistanceDiameter);

    int x = 0;
    for (auto& xChunk : allMeshFacesChunk)
    {
        int y = 0;
        for (auto& yChunk : xChunk)
        {
            int z = 0;
            for (allMeshes& zChunk : yChunk)
            {
                if (zChunk.chunkMesh.vertexCount == 0)
                {
                    z++;
                    continue;
                }

                Vector3 chunkCenter =
                {
                    (float)((CenterCoords.x + x - renderDistanceDiameterHalfed) * chunkXlength) + halfefdchunkXlength,
                    (float)((CenterCoords.y + y - renderDistanceDiameterHalfed) * chunkYlength) + halfefdchunkYlength,
                    (float)((CenterCoords.z + z - renderDistanceDiameterHalfed) * chunkZlength) + halfefdchunkHight
                };

                Vector3 toChunk = Vector3Normalize(
                    Vector3Subtract(chunkCenter, camera.position));

                float dot = Vector3DotProduct(camDir, toChunk);

                bool isMiddleChunk =
                    (x - chunkingDiffIntCoord.x >= renderDistanceDiameterHalfed - 1 &&
                        x - chunkingDiffIntCoord.x <= renderDistanceDiameterHalfed + 1) &&
                    (y - chunkingDiffIntCoord.y >= renderDistanceDiameterHalfed - 1 &&
                        y - chunkingDiffIntCoord.y <= renderDistanceDiameterHalfed + 1) &&
                    (z - chunkingDiffIntCoord.z >= renderDistanceDiameterHalfed - 1 &&
                        z - chunkingDiffIntCoord.z <= renderDistanceDiameterHalfed + 1);

                if (dot < 0.28f && !isMiddleChunk)
                {
                    z++;
                    continue;
                }

                float dx = chunkCenter.x - camera.position.x;
                float dy = chunkCenter.y - camera.position.y;
                float dz = chunkCenter.z - camera.position.z;

                chunksToRender.push_back({
                    &zChunk,
                    dx * dx + dy * dy + dz * dz
                    });

                z++;
            }
            y++;
        }
        x++;
    }

    // Farthest first
    std::sort(chunksToRender.begin(), chunksToRender.end(),
        [](const ChunkRenderInfo& a, const ChunkRenderInfo& b)
        {
            return a.distanceSq > b.distanceSq;
        });

    // Draw
    for (const auto& chunk : chunksToRender)
    {
        DrawMesh(chunk.mesh->chunkMesh, blockMaterial, MatrixIdentity());
    }

}

void shift3DList(
    std::list<std::list<std::list<allMeshes>>>& mesh,
    int dx,
    int dy,
    int dz)
{
    while (dx > 0)
    {
        mesh.splice(mesh.end(), mesh, mesh.begin());
        --dx;
    }

    while (dx < 0)
    {
        mesh.splice(mesh.begin(), mesh, std::prev(mesh.end()));
        ++dx;
    }

    for (auto& xLayer : mesh)
    {
        int y = dy;

        while (y > 0)
        {
            xLayer.splice(xLayer.end(), xLayer, xLayer.begin());
            --y;
        }

        while (y < 0)
        {
            xLayer.splice(xLayer.begin(), xLayer, std::prev(xLayer.end()));
            ++y;
        }
    }

    for (auto& xLayer : mesh)
    {
        for (auto& yLayer : xLayer)
        {
            int z = dz;

            while (z > 0)
            {
                yLayer.splice(yLayer.end(), yLayer, yLayer.begin());
                --z;
            }

            while (z < 0)
            {
                yLayer.splice(yLayer.begin(), yLayer, std::prev(yLayer.end()));
                ++z;
            }
        }
    }
}
void shiftGottenChunks(
    int dx,
    int dy,
    int dz)
{
    lock_guard<mutex> lock(gottenChunksMutex);
    int sizeX = gottenChunks.size();
    int sizeY = gottenChunks[0].size();
    int sizeZ = gottenChunks[0][0].size();

    gottenChunksUsing newChunks(
        sizeX,
        vector<vector<gottenchunksStates>>(
            sizeY,
            vector<gottenchunksStates>(sizeZ, gottenchunksStates::create)
        )
    );

    for (int x = 0; x < sizeX; x++)
    {
        for (int y = 0; y < sizeY; y++)
        {
            for (int z = 0; z < sizeZ; z++)
            {
                int oldX = x + dx;
                int oldY = y + dy;
                int oldZ = z + dz;

                if (
                    oldX >= 0 && oldX < sizeX &&
                    oldY >= 0 && oldY < sizeY &&
                    oldZ >= 0 && oldZ < sizeZ
                    )
                {
                    newChunks[x][y][z] = gottenChunks[oldX][oldY][oldZ];
                }
            }
        }
    }
    gottenChunks = std::move(newChunks);
}

void pushingThe3DLists(
    int xValueToPush,
    int yValueToPush,
    int zValueToPush)
{
    shiftGottenChunks(
        xValueToPush,
        yValueToPush,
        zValueToPush
    );

    shift3DList(
        allMeshFacesChunk,
        xValueToPush,
        yValueToPush,
        zValueToPush
    );
}