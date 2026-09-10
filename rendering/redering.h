#pragma once

#include "../chunk_cpp/chunk.h"
#include "../blocks.h"
#include <array>

using namespace std;

void createMeshFaces_ofAChunk(int3D chunkIndex, int3D chunkCoord);

void renderBlocks();

void buildDirectionLookup();
 
//extern BlockPtrs3D exoticBlocksRender;

//void getChunks_goodBlocksTo_blocksPerformaceRender(Chunk& chunk, int3D& centerSnapshot);

//void putInTheMeshBaby(const vector<SquareFace>& faces, rotation rotationBlock, Vector3 coord, int textureId, int3D chunkCoord, int seeThrough);

template<typename T>
T& getMeshChunk(
    std::list<std::list<std::list<T>>>& mesh,
    int x,
    int y,
    int z
);

void pushingThe3DLists(int xValueToPush, int yValueToPush, int zValueToPush);

struct facesOfABlock {
    SquareFace face;
    Vector3 base;
    int textureID;
};

//using BlockPtrs3DVectorPart = vector<vector<vector<blockInfo*>>>;

//using BlockPtrs3D = list<list<list<BlockPtrs3DVectorPart>>>;

//extern BlockPtrs3D blocksPerformaceRender;

struct Vertex
{
    float x, y, z;
    float u, v;
};

using allFacesChunk = vector<facesOfABlock>;


//extern list<list<list<allFacesChunk>>> meshFacesChunk;
//
//extern list<list<list<allFacesChunk>>> meshFacesChunkForTransparency;

void DrawItemFace(
    const Vector3& v0,
    const Vector3& v1,
    const Vector3& v2,
    const Vector3& v3,
    const AtlasTile& tile,
    const Texture2D& atlas);


//constexpr float PI = 3.14159265358979323846f / 180.0f;

Vector3 rotate_Vector3(Vector3 v, Vector3 rotationDeg);

void EndItemRenderer();
void initRendering();

inline Vector3 rotateXYZ_centered(Vector3 v, int rotX, int rotY, int rotZ);