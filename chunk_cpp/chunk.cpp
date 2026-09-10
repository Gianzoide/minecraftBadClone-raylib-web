#include <iostream>
#include <vector>
#include <list>
#include <array>


#include "usefullFunc.h"
#include <unordered_map>

#include "../rendering/redering.h"

#include <raylib.h>
#include "chunk.h"

#include <raymath.h>
#include <algorithm>

#include <random>

//#include "../blocks.h"

int3D CenterCoords{ 0, 0, 0 };

using namespace std;

std::unordered_map<ChunkKey, Chunk, ChunkKeyHash> worldChunks;

vector<collisionBox> rotateCollisionBoxes(
	const vector<collisionBox>& boxes,
	Rotation rot
) {
	vector<collisionBox> result;

	Matrix rotationMatrix =
		MatrixRotateX(DEG2RAD * rotX(rot)) *
		MatrixRotateY(DEG2RAD * rotY(rot)) *
		MatrixRotateZ(DEG2RAD * rotZ(rot));

	for (const collisionBox& box : boxes) {

		Vector3 min = box.startposition;
		Vector3 max = Vector3Add(min, box.size);

		// 8 corners (LOCAL space)
		Vector3 corners[8] = {
			{min.x, min.y, min.z},
			{max.x, min.y, min.z},
			{min.x, max.y, min.z},
			{max.x, max.y, min.z},
			{min.x, min.y, max.z},
			{max.x, min.y, max.z},
			{min.x, max.y, max.z},
			{max.x, max.y, max.z}
		};

		// rotate around LOCAL center
		Vector3 center = { 0.5f, 0.5f, 0.5f }; // block center

		for (int i = 0; i < 8; i++) {
			corners[i] = Vector3Subtract(corners[i], center);
			corners[i] = Vector3Transform(corners[i], rotationMatrix);
			corners[i] = Vector3Add(corners[i], center);
		}

		// rebuild AABB
		Vector3 newMin = corners[0];
		Vector3 newMax = corners[0];

		for (int i = 1; i < 8; i++) {
			newMin = Vector3Min(newMin, corners[i]);
			newMax = Vector3Max(newMax, corners[i]);
		}

		collisionBox newBox;
		newBox.startposition = newMin;
		newBox.size = Vector3Subtract(newMax, newMin);

		result.push_back(newBox);
	}

	return result;
}

#include <mutex>
std::mutex chunksMutex;

#include "../FastNoiseLite.h"
FastNoiseLite noise;
void setNoise() {
	noise.SetSeed(1337);
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(4);
}

#include <cmath>

float GetTerrainHeight(float x, float y)
{
	// Terreno geral (continentes)
	float continents =
		noise.GetNoise(x * 2.0f, y * 2.0f) * 6.0f;

	// Distorção das coordenadas (ajuda a criar montanhas malucas)
	float warpX =
		x + noise.GetNoise(x * 0.002f, y * 0.002f) * 150.0f;

	float warpY =
		y + noise.GetNoise(
			x * 0.002f + 1000.0f,
			y * 0.002f + 1000.0f
		) * 150.0f;

	// Montanhas Beta-style
	float mountain =
		fabsf(
			noise.GetNoise(
				warpX * 1.15f,
				warpY * 1.15f
			)
		);
	float bigMountain =
		fabsf(
			noise.GetNoise(
				warpX * 0.8f,
				warpY * 0.8f
			)
		);
	
	if (bigMountain >= 0.3 && mountain >= 0.2) {
		bigMountain *= 12;
	}

	int more = 0;
	if (mountain >= 0.6) {
		more = 5;
	}
	else if(mountain >= 0.5) {
		more = 4;
	}
	else if (mountain >= 0.4) {
		more = 1;
	}

	// Altura máxima das montanhas
	mountain *= 12.0f;

	return
		0
		+ continents
		+ mountain
		+ bigMountain
		+ more
	;
}

vector<blockOfstructureInfo> treeStructure = { 
	{BlockID::oak_log, {0,0,0} }, 
	{BlockID::oak_log, {0,0,1} }, 
	{BlockID::oak_log, {0,0,2} }, 
	{BlockID::oak_log, {0,0,3} }, 
	{BlockID::oak_leaves, {-2,-2,3} }, {BlockID::oak_leaves, {-1,-2,3} }, {BlockID::oak_leaves, { 0,-2,3} }, {BlockID::oak_leaves, { 1,-2,3} }, {BlockID::oak_leaves, { 2,-2,3} }, 
	{BlockID::oak_leaves, {-2,-1,3} }, {BlockID::oak_leaves, {-1,-1,3} }, {BlockID::oak_leaves, { 0,-1,3} }, {BlockID::oak_leaves, { 1,-1,3} }, {BlockID::oak_leaves, { 2,-1,3} },
	{BlockID::oak_leaves, {-2, 0,3} }, {BlockID::oak_leaves, {-1, 0,3} }, /*{BlockID::oak_leaves, { 0, 0,3} },*/ {BlockID::oak_leaves, { 1, 0,3} }, {BlockID::oak_leaves, { 2, 0,3} },
	{BlockID::oak_leaves, {-2, 1,3} }, {BlockID::oak_leaves, {-1, 1,3} }, {BlockID::oak_leaves, { 0, 1,3} }, {BlockID::oak_leaves, { 1, 1,3} }, {BlockID::oak_leaves, { 2, 1,3} },
	{BlockID::oak_leaves, {-2, 2,3} }, {BlockID::oak_leaves, {-1, 2,3} }, {BlockID::oak_leaves, { 0, 2,3} }, {BlockID::oak_leaves, { 1, 2,3} }, {BlockID::oak_leaves, { 2, 2,3} },
	/*{BlockID::oak_leaves, {-2,-2,4} },*/ {BlockID::oak_leaves, {-1,-2,4} }, {BlockID::oak_leaves, { 0,-2,4} }, {BlockID::oak_leaves, { 1,-2,4} }, /*{BlockID::oak_leaves, { 2,-2,4} },*/
	{BlockID::oak_leaves, {-2,-1,4} }, {BlockID::oak_leaves, {-1,-1,4} }, {BlockID::oak_leaves, { 0,-1,4} }, {BlockID::oak_leaves, { 1,-1,4} }, {BlockID::oak_leaves, { 2,-1,4} },
	{BlockID::oak_leaves, {-2, 0,4} }, {BlockID::oak_leaves, {-1, 0,4} }, {BlockID::oak_leaves, { 0, 0,4} }, {BlockID::oak_leaves, { 1, 0,4} }, {BlockID::oak_leaves, { 2, 0,4} },
	{BlockID::oak_leaves, {-2, 1,4} }, {BlockID::oak_leaves, {-1, 1,4} }, {BlockID::oak_leaves, { 0, 1,4} }, {BlockID::oak_leaves, { 1, 1,4} }, {BlockID::oak_leaves, { 2, 1,4} },
	/*{BlockID::oak_leaves, {-2, 2,4} },*/ {BlockID::oak_leaves, {-1, 2,4} }, {BlockID::oak_leaves, { 0, 2,4} }, {BlockID::oak_leaves, { 1, 2,4} }, /*{BlockID::oak_leaves, { 2, 2,4} },*/
	{BlockID::oak_leaves, {-1,-1,5} }, {BlockID::oak_leaves, { 0,-1,5} }, {BlockID::oak_leaves, { 1,-1,5} }, 
	{BlockID::oak_leaves, {-1, 0,5} }, {BlockID::oak_leaves, { 0, 0,5} }, {BlockID::oak_leaves, { 1, 0,5} },
	{BlockID::oak_leaves, {-1, 1,5} }, {BlockID::oak_leaves, { 0, 1,5} }, {BlockID::oak_leaves, { 1, 1,5} },
										{BlockID::oak_leaves, { 0,-1,6} }, 
	{BlockID::oak_leaves, {-1, 0,6} }, {BlockID::oak_leaves, { 0, 0,6} }, {BlockID::oak_leaves, { 1, 0,6} },
										{BlockID::oak_leaves, { 0, 1,6} }, 
};

#include <unordered_set>

struct int3DStructure
{
	int x, y, z;
	bool operator==(const int3DStructure& other) const
	{
		return x == other.x &&
			y == other.y &&
			z == other.z;
	}
};
struct int3DStructureHash
{
	size_t operator()(const int3DStructure& pos) const
	{
		return std::hash<int>()(pos.x) ^
			(std::hash<int>()(pos.y) << 1) ^
			(std::hash<int>()(pos.z) << 2);
	}
};
std::unordered_set<int3DStructure, int3DStructureHash> structures;

void preparingStructureToBePLaced(int3D& chunkCoord, int3D& structureStartPos)
{
	for (const blockOfstructureInfo& bloc : treeStructure)
	{
		blockOfstructureInfo newBlock;
		newBlock.blockId = bloc.blockId;
		newBlock.coords = {
			bloc.coords.x + structureStartPos.x,
			bloc.coords.y + structureStartPos.y,
			bloc.coords.z + structureStartPos.z
		};
		int3D targetChunk = chunkCoord;

		while (newBlock.coords.x < 0            ){newBlock.coords.x += chunkXlength;targetChunk.x--;}
		while (newBlock.coords.x >= chunkXlength){newBlock.coords.x -= chunkXlength;targetChunk.x++;}
		while (newBlock.coords.y < 0            ){newBlock.coords.y += chunkYlength;targetChunk.y--;}
		while (newBlock.coords.y >= chunkYlength){newBlock.coords.y -= chunkYlength;targetChunk.y++;}
		while (newBlock.coords.z < 0            ){newBlock.coords.z += chunkZlength;targetChunk.z--;}
		while (newBlock.coords.z >= chunkZlength){newBlock.coords.z -= chunkZlength;targetChunk.z++;}
		
		Chunk* targetChunkPtr = getChunk(targetChunk.x, targetChunk.y, targetChunk.z);

		if (targetChunkPtr == nullptr)
		{
			ChunkKey key = {targetChunk.x,targetChunk.y,targetChunk.z};
			worldChunks.emplace( key, Chunk{targetChunk.x,targetChunk.y,targetChunk.z} );
			targetChunkPtr = getChunk(targetChunk.x, targetChunk.y, targetChunk.z);
		}
		structures.insert(int3DStructure{ targetChunk.x, targetChunk.y, targetChunk.z});
		targetChunkPtr->structureToBePlaced.push_back(newBlock);
	}
}

BlockID whatBlockToCreate(
	int3D& worldCoord,
	int terrainHeight,
	int3D& chunkCoord,
	int3D& blockPos)
{
	float biome = noise.GetNoise((float)worldCoord.x * 0.8, (float)worldCoord.y * 0.8);
	int grassMinimumDepth = 1;
	if ((worldCoord.x + worldCoord.y) % 2 == 0){
		grassMinimumDepth = 2;
	}
	if (biome < 0.5) {
		if (worldCoord.z == terrainHeight + 1) {

			uint32_t seed =
				static_cast<uint32_t>(worldCoord.x * 73856093) ^
				static_cast<uint32_t>(worldCoord.y * 19349663) ^
				static_cast<uint32_t>(worldCoord.z * 83492791);

			int random = seed % 1000;

			// Tree: 1% chance
			if (random < 1) {
				preparingStructureToBePLaced(chunkCoord, blockPos);
				return BlockID::Air;
			}

			// Flowers: next 19%
			if (random < 200) {
				switch (seed % 7) {
				case 0: return BlockID::flower_allium;
				case 1: return BlockID::flower_blue_orchid;
				case 2: return BlockID::flower_dandelion;
				case 3: return BlockID::flower_houstonia;
				case 4: return BlockID::flower_oxeye_daisy;
				case 5: return BlockID::flower_paeonia;
				case 6: return BlockID::flower_tulip_white;
				}
			}

			return BlockID::Air;
		}
		else if (worldCoord.z == terrainHeight)
		{
			return BlockID::Grass_block;
		}
		else if (worldCoord.z >= terrainHeight - grassMinimumDepth)
		{
			return BlockID::Dirt_block;
		}
		else
		{
			return BlockID::Stone_block;
		}
	}
	else {
		if (worldCoord.z == terrainHeight || worldCoord.z >= terrainHeight - grassMinimumDepth)
		{
			float stoneNoise = biome;
			float randomThing = noise.GetNoise((float)worldCoord.x * 3, (float)worldCoord.y * 3);
			//cout << endl << randomThing;

			if (biome > 0.7f || randomThing > 0.45f) {
				return BlockID::Stone_block;
			}

			return BlockID::gravel;
		}
		else
		{
			return BlockID::Stone_block;
		}

	}
}

void placingTheStructure(Chunk& chunk) {
	// placing structures
	ChunkBlocks& allBlocks = chunk.blocks;
	int3D chunkCoordMultiplyed = { chunk.x * chunkXlength, chunk.y * chunkYlength, chunk.z * chunkZlength};
	for (blockOfstructureInfo& block : chunk.structureToBePlaced) {
		int3D& coordBlock = block.coords;
		auto& theCoord = allBlocks[Index(coordBlock.x, coordBlock.y, coordBlock.z)];
		if (theCoord.id == Air) {
			int3D bruh = { chunkCoordMultiplyed.x + coordBlock.x, chunkCoordMultiplyed.y + coordBlock.y, chunkCoordMultiplyed.z + coordBlock.z };
			theCoord = blockInfo{ block.blockId, getBlockDetermedRotation(blockIdDefinition[block.blockId].rotationDetermination, bruh) };
		}
	}
	chunk.structureToBePlaced = {};
}

void createChunk(int x, int y, int z)
{
	//std::lock_guard<std::mutex> lock(chunksMutex);
	Chunk* existingChunk = getChunk(x, y, z);
	if (existingChunk != nullptr && existingChunk->wasTerrainGenerated == true){ return; } /* if the terrain was generated = > return */

	Chunk* chunk;
	if (existingChunk == nullptr) {
		ChunkKey key = { x, y, z };
	
		auto [it, inserted] = worldChunks.emplace(key, Chunk{ x, y, z }); // no idea how this works

		chunk = &it->second;
	}
	else {
		chunk = existingChunk;
	}

	ChunkBlocks& allBlocks = chunk->blocks;
	int quatityBlocksFound = 0;
	//if (x == y) {
	//	return;
	//}
	if (x == 1 && y == 1) {
		return;
	}
	int coordExtended_X = x * chunkXlength;
	int coordExtended_Y = y * chunkYlength;
	int coordExtended_Z = z * chunkZlength;
	for (int localX = 0; localX < chunkXlength; localX++) 
	{
		int worldX = coordExtended_X + localX;
		for (int localY = 0; localY < chunkYlength; localY++)
		{
			int worldY = coordExtended_Y + localY;

			int terrainHeight =
				//0;
				(int)GetTerrainHeight(
				(float)worldX,
				(float)worldY
			);

			for (int localZ = 0; localZ < chunkZlength; localZ++)
			{
				int worldZ = coordExtended_Z + localZ;

				if (worldZ > terrainHeight + 1) {
					continue;
				}

				int3D myLittlePony = { worldX, worldY, worldZ };
				int3D myLittlePony2 = { x, y, z };
				int3D myLittlePony3 = { localX, localY, localZ };

				BlockID blockId = whatBlockToCreate(myLittlePony, terrainHeight, myLittlePony2, myLittlePony3);
				if (blockId == Stone_block) {
					float coalOre = noise.GetNoise((float)worldX * 6.5f, (float)worldY * 6.5f, (float)worldZ * 6.5f);
					if (coalOre > 0.5f) {
						blockId = coal_ore;
					}
					else {
						float rareByDepth = 0.9;
						if (worldZ < 0 && worldZ > -100) {
							rareByDepth += abs((worldZ) * 0.0015);
						}
						float ironlOre = noise.GetNoise((float)worldX * 8.0f, (float)worldY * 8.0f, (float)worldZ * 7.0f) * rareByDepth;
						if (ironlOre > 0.55f) {
							blockId = iron_ore;
						}
					}
				}
				quatityBlocksFound++;
				allBlocks[Index(localX, localY, localZ)] =
					blockInfo{
						blockId,
						getBlockDetermedRotation(blockIdDefinition[blockId].rotationDetermination, myLittlePony),
					}
				;
			}
		}
	}


	chunk->wasTerrainGenerated = true;
	//chunk.quatityBlocks = quatityBlocksFound;
}
mutex getChunkMutex;
Chunk* getChunk(int x, int y, int z) {
	lock_guard<mutex> lock(getChunkMutex);

	ChunkKey key = { x, y, z };

	auto it = worldChunks.find(key);

	if (it != worldChunks.end()) {
		return &it->second;
	}
	
	return nullptr;
}

const float epsilon = 0.0001f;
int3D getCoordChunk_By_positionTimesChunk(Vector3 position) {

	return int3D{
		(int)floor((position.x + epsilon) / chunkXlength),
		(int)floor((position.y + epsilon) / chunkYlength),
		(int)floor((position.z + epsilon) / chunkZlength)
	};
}


#include <thread>
mutex gottenChunks2Mutex;
bool threading123 = false;

void threadCreateChunksAndMeshes(vector<int3D> createChunks, vector<int3D> createMeshes, int3D centerSnapshot) {
	vector<int3D> fineshedCreating{};
	vector<int3D> fineshedMeshing{};

	int3D toBeginningCoord = { 
		centerSnapshot.x - renderDistanceDiameterHalfed,
		centerSnapshot.y - renderDistanceDiameterHalfed,
		centerSnapshot.z - renderDistanceDiameterHalfed
	};

	for (int3D indexedCoord : createChunks) {
		int3D chunkCoord = { 
			toBeginningCoord.x + indexedCoord.x, 
			toBeginningCoord.y + indexedCoord.y, 
			toBeginningCoord.z + indexedCoord.z 
		};
		createChunk(
			chunkCoord.x,
			chunkCoord.y,
			chunkCoord.z
		);
		fineshedCreating.push_back(int3D{ indexedCoord.x, indexedCoord.y, indexedCoord.z });
	}

	for (int3D gottenChunkCoord : createMeshes) {
		int3D chunkCoord = {
			toBeginningCoord.x + gottenChunkCoord.x,
			toBeginningCoord.y + gottenChunkCoord.y,
			toBeginningCoord.z + gottenChunkCoord.z
		};
		createMeshFaces_ofAChunk(gottenChunkCoord, chunkCoord);

		fineshedMeshing.push_back(int3D{ gottenChunkCoord.x, gottenChunkCoord.y, gottenChunkCoord.z });
	}
	lock_guard<mutex> lock(gottenChunks2Mutex);
	for (int3D coords : fineshedCreating) {
		gottenChunks[coords.x][coords.y][coords.z] = gottenchunksStates::update;
	}
	for (int3D coords : fineshedMeshing) {
		gottenChunks[coords.x][coords.y][coords.z] = gottenchunksStates::done;
	}
	cout << endl << "Fineshed threading.";
	threading123 = false;
}

#include <queue>
queue<Mesh*> uploadMeshToGPU;

int3D oldCenterCoords = {90,90,90};
mutex uploadMeshToGPUMutex1;
void chunk()
{
	if (threading123) {
		return;
	}
	//if (structures.size() != 0) {
	//	cout << endl << "---------=======" << structures.size();
	//}
	for (auto it = structures.begin(); it != structures.end();)
	{
		const int3DStructure& struc = *it;
		Chunk* chunk = getChunk(struc.x, struc.y, struc.z);
		int3D boundsGottenChunk = {
			struc.x - CenterCoords.x + renderDistanceDiameterHalfed,
			struc.y - CenterCoords.y + renderDistanceDiameterHalfed,
			struc.z - CenterCoords.z + renderDistanceDiameterHalfed
		};
		if (boundsGottenChunk.x < 0 || boundsGottenChunk.x >= renderDistanceDiameter ||
			boundsGottenChunk.y < 0 || boundsGottenChunk.y >= renderDistanceDiameter ||
			boundsGottenChunk.z < 0 || boundsGottenChunk.z >= renderDistanceDiameter
			) {
			++it;
			continue;
		}
		if (chunk != nullptr && chunk->wasTerrainGenerated == true)
		{
			//cout << endl << "bruhx: " << boundsGottenChunk.x;
			//cout << endl << "bruhy: " << boundsGottenChunk.y;
			//cout << endl << "bruhz: " << boundsGottenChunk.z;

			gottenChunks[boundsGottenChunk.x][boundsGottenChunk.y][boundsGottenChunk.z] = gottenchunksStates::update;

			placingTheStructure(*chunk);
			it = structures.erase(it);
		}
		else
		{
			++it;
		}
	}
	vector<int3D> toCreateChunks = {};
	vector<int3D> toCreateMeshes = {};
	int dx = CenterCoords.x - oldCenterCoords.x;
	int dy = CenterCoords.y - oldCenterCoords.y;
	int dz = CenterCoords.z - oldCenterCoords.z;

	if (abs(dx) >= renderDistanceDiameter ||
		abs(dy) >= renderDistanceDiameter ||
		abs(dz) >= renderDistanceDiameter)
	{
		toCreateChunks = {};
		toCreateMeshes = {};

		for (auto& x : gottenChunks)
			for (auto& y : x)
				for (auto& z : y)
					z = gottenchunksStates::create;
	}
	else if (dx != 0 || dy != 0 || dz != 0)
	{
		pushingThe3DLists(dx, dy, dz);
	}
	int halfed = renderDistanceDiameterHalfed + 1;

	for (int x = 0; x < renderDistanceDiameter; x++){
		for (int y = 0; y < renderDistanceDiameter; y++){
			for (int z = 0; z < renderDistanceDiameter; z++){
				gottenchunksStates& gottenState = gottenChunks[x][y][z];

				if (gottenState == gottenchunksStates::create)
				{
					toCreateChunks.push_back(int3D{ x, y, z });
					gottenState = gottenchunksStates::loadingCreating;
				}
				if (gottenState == gottenchunksStates::update)
				{
					toCreateMeshes.push_back(int3D{ x, y, z });
					gottenState = gottenchunksStates::loadingUpdate;
				}
			}
		}
	}
	if (toCreateChunks.size() != 0 || toCreateMeshes.size() != 0) {
		cout << endl << "trhead: toCreate-> " << toCreateChunks.size() << " toMesh-> " << toCreateMeshes.size();
		lock_guard<mutex> lock(gottenChunks2Mutex);
		threading123 = true;
		#ifdef __EMSCRIPTEN__
		threadCreateChunksAndMeshes(
			toCreateChunks,
			toCreateMeshes,
			CenterCoords
		);
		#else
		thread t(
			threadCreateChunksAndMeshes,
			toCreateChunks,
			toCreateMeshes,
			CenterCoords
		);
		t.detach();

		//threadCreateChunksAndMeshes(
		//	toCreateChunks,
		//	toCreateMeshes,
		//	CenterCoords
		//);
		#endif
	}
	int timesUploadedNow = 0;
	while (timesUploadedNow < 10) {
		lock_guard<mutex> lock(uploadMeshToGPUMutex1);
		if (uploadMeshToGPU.size() == 0) {
			break;
		}

		UploadMesh(uploadMeshToGPU.front(), false);
		uploadMeshToGPU.pop();
		timesUploadedNow++;
	}

	oldCenterCoords = CenterCoords;
}

int renderDistanceDiameter = 0;
int renderDistanceDiameterHalfed = 0;

gottenChunksUsing gottenChunks;

list<list<list<allMeshes>>> allMeshFacesChunk(
	renderDistanceDiameter,
	list<list<allMeshes>>(
		renderDistanceDiameter,
		list<allMeshes>(
			renderDistanceDiameter,
			allMeshes{ Mesh() }
		)
	)
);

void changeRenderDistanceRadius(int radius) {
	if (threading123) {
		return;
	}
	
	renderDistanceDiameter = (radius * 2) + 1;
	renderDistanceDiameterHalfed = radius;

	gottenChunks = vector<vector<vector<gottenchunksStates>>>(
		renderDistanceDiameter, vector<vector<gottenchunksStates>>(
			renderDistanceDiameter, vector<gottenchunksStates>(
				renderDistanceDiameter, gottenchunksStates::create
			)
		)
	);

	allMeshFacesChunk = list<list<list<allMeshes>>>(
		renderDistanceDiameter,
		list<list<allMeshes>>(
			renderDistanceDiameter,
			list<allMeshes>(
				renderDistanceDiameter,
				allMeshes{ Mesh() }
			)
		)
	);
}