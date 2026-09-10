using namespace std;
#include "../chunk_cpp/chunk.h"
#include "../camera_cpp/camera.h"
#include "../blocks.h"

#include <list>

#pragma once


void updateParticles();

void createParticles(int3D& chunkCoord, Vector3 blockCoord, BlockID blockId, float gravityForce = 0.01, float goThroughBlock = 0);

struct particleInfo {
	Vector3 coord;
	Vector3 directionalForce;
	AtlasTile texture;
	int lifeTime;
	bool alive = true;
	float squaresize;
};

struct particlesPackage {
	int3D chunkCoord;
	float gravityForce;
	float goThroughBlock; // 0 == no / highter than 0 then its resistem of phasing the block
	vector<particleInfo> particles;
	int particlesAlive;
};

extern list<particlesPackage> particlesList;
