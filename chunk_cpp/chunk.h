
#include <raylib.h> 
#include <raymath.h>
#include <list>
#include <vector>
#include <array>
//#include "../blocks.h"
#include <cstdint>

using namespace std;

#pragma once

extern bool threading123;

#include <queue>
extern queue<Mesh*> uploadMeshToGPU;

enum class gottenchunksStates : int {
	done = 0,
	create = 1,
	generating = 2,
	update = 3,
	loadingCreating = 4,
	loadingUpdate = 5
};

using gottenChunksUsing = vector<vector<vector<gottenchunksStates>>>;
extern gottenChunksUsing gottenChunks;

struct allMeshes {
	Mesh chunkMesh = {};
};
extern list<list<list<allMeshes>>> allMeshFacesChunk;


inline constexpr int chunkXlength = 16;
inline constexpr int chunkYlength = 16;
inline constexpr int chunkZlength = 16;

extern int renderDistanceDiameter;
extern int renderDistanceDiameterHalfed;

void changeRenderDistanceRadius(int radius);

constexpr int chunkXYZ_TOTALSIZE = chunkXlength * chunkYlength * chunkZlength;

inline int Index(int x, int y, int z)
{
	return x
		+ y * chunkXlength
		+ z * chunkXlength * chunkYlength;
}

struct int3D {
	int x, y, z;
};

extern int3D CenterCoords;
extern int3D oldCenterCoords;

void chunk();
void setNoise();

inline int3D IndexToXYZ(int index)
{
	int3D XYZ;
	XYZ.z = index / (chunkXlength * chunkYlength);

	index %= (chunkXlength * chunkYlength);

	XYZ.y = index / chunkXlength;

	XYZ.x = index % chunkXlength;
	return XYZ;
}

int3D getCoordChunk_By_positionTimesChunk(Vector3 positionTimesChunk);

enum class Shape {
	cube = 0,
	halfcube = 1,
	stair = 2,
	fire = 3,
	flower = 4,
};

struct AtlasTile
{
	float u0, v0;
	float u1, v1;
};
constexpr float ATLAS_SIZE = 1024.0f;
constexpr float TILE_SIZE = 16.0f;
constexpr float UV_SIZE = TILE_SIZE / ATLAS_SIZE;
constexpr float PIXEL_SIZE = 1 / 1024.0f;

//struct blockyTexture {
//	AtlasTile ZPositive;
//	AtlasTile ZNegative;
//	AtlasTile XPositive;
//	AtlasTile XNegative;
//	AtlasTile YPositive;
//	AtlasTile YNegative;
//};

using blockyTexture = array<AtlasTile, 6>;

struct collisionBox {
	Vector3 size = {};
	Vector3 startposition = { 0.0, 0.0, 0.0 };
};

using collisionBoxes = std::vector<collisionBox>;

#ifndef __cplusplus
#error This must be compiled as C++
#endif

//struct blockInfo {
//	//float x, y, z;
//	Vector3 coord;
//	shape blockShape;
//	blockyTexture textureID;
//	rotation rotation;
//	vector<collisionBox> collisionBox; //can have complex squarery type of shape like a cube or a minecraft stair
//	int breakingEndurance = 1000;
//	int lightingValue = 0;
//	bool exist = false;
//};

using Rotation = uint8_t;
inline Rotation makeRotation(uint8_t x, uint8_t y, uint8_t z)
{
	return (x & 3)
		| ((y & 3) << 2)
		| ((z & 3) << 4);
}
inline uint8_t rotX(Rotation r)
{
	return r & 3;
}
inline uint8_t rotY(Rotation r)
{
	return (r >> 2) & 3;
}
inline uint8_t rotZ(Rotation r)
{
	return (r >> 4) & 3;
}
enum BlockID : uint8_t
{
	Air = 0,
	Dirt_block = 1,
	Stone_block = 2,
	Grass_block = 3,
	CobbleStore_block = 4,
	Glass_block = 5,
	fire_block = 6,
	flower_allium = 7,
	flower_blue_orchid = 8,
	flower_dandelion = 9,
	flower_houstonia = 10,
	flower_oxeye_daisy = 11,
	flower_paeonia = 12,
	flower_tulip_white = 13,
	oak_log = 14,
	oak_leaves = 15,
	gravel = 16,
	iron_ore = 17,
	coal_ore = 18,
	COUNT
};

struct blockInfo {
	//float x, y, z;
	BlockID id;        // Air, Grass, Dirt...
	Rotation rotation;
	//uint8_t data;     // idk maybe in the future
};
enum class determinationRotationBlock { // pre means the game decides how it should be rotated
	notPreDetermed, //any direction rotation
	XYZFixed, // allways same direction rotation (0,0,0)
	ZFixed, // only can rotate in the X or Y
	ZFixed_XYPreDetermed, // Z always the same, game decides X and Y
};
enum class tranparencyType {
    justOpaque,
    transparent,
    semi_Transparent
};
enum class collisionEnum {
	nothing,
	cube,
	halfBlock,
	stair,
	flower,
	COUNT
};
struct BlockDefinition
{
	blockyTexture texture;
	collisionBoxes collision;
	Shape shapeTexture;
	collisionBoxes mouseInteraction;
	int endurance;
	int light;
	bool glassLike;
	determinationRotationBlock rotationDetermination;
	tranparencyType renderType;
};

struct blockOfstructureInfo {
	BlockID blockId;
	int3D coords;
};
struct blockBreakingAnimation {
	int coord;
	float durabilidy;
};

extern array<BlockDefinition, (size_t)BlockID::COUNT> blockIdDefinition;

using ChunkBlocks = array<blockInfo, chunkXlength* chunkYlength* chunkZlength>;

struct Chunk {
	int x = 0, y = 0, z = 0;
	ChunkBlocks blocks = {};
	
	int quatityBlocks = 0;
	bool wasTerrainGenerated = false;
	vector<blockOfstructureInfo> structureToBePlaced = {};
	std::unordered_set<blockBreakingAnimation> blockBreaking;
	//vector<entitiesInfo> entities = {};
};  

struct ChunkKey {
	int x, y, z;

	bool operator==(const ChunkKey& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
};

struct ChunkKeyHash {
	size_t operator()(const ChunkKey& k) const {
		return ((std::hash<int>()(k.x) ^
			(std::hash<int>()(k.y) << 1)) >> 1) ^
			(std::hash<int>()(k.z) << 1);
	}
};

void createChunk(int x, int y, int z);

Chunk* getChunk(int x, int y, int z);

vector<collisionBox> rotateCollisionBoxes(
	const vector<collisionBox>& boxes,
	Rotation rot
);

