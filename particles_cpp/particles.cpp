
#include <iostream>
#include "particles.h"
#include "../movement_cpp/movement.h"
#include "../movement_cpp/collisionCheck.h"
#include "rlgl.h"

#include <random>

float RandomFloatSigned01(float minNum, float maxNum)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	static std::uniform_real_distribution<float> valueDist(minNum, maxNum);
	static std::bernoulli_distribution signDist(0.5);

	float value = valueDist(gen);
	return signDist(gen) ? value : -value;
}

float RandomFloat(float minNum, float maxNum)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(minNum, maxNum);

	return dist(gen);
}

int randomInt(int minNum, int maxNum)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	static std::uniform_int_distribution<int> dist(minNum, maxNum);

	return dist(gen);
}

void DrawBillboardAtlas(
	Camera3D camera, Texture2D atlas, AtlasTile tile, Vector3 position,
	float width, float height, Color tint)
{
	// Calculate vectors perpendicular to camera view
	Vector3 forward = Vector3Normalize(
		Vector3Subtract(camera.position, position));
	Vector3 right = Vector3Normalize(
		Vector3CrossProduct(camera.up, forward));

	// Recompute a true "up" so the quad is fully perpendicular to view direction
	Vector3 up = Vector3Normalize(Vector3CrossProduct(forward, right));

	// Scale by dimensions
	right = Vector3Scale(right, width * 0.5f);
	up = Vector3Scale(up, height * 0.5f);

	// Calculate quad corners
	Vector3 v0 = Vector3Add(Vector3Subtract(position, right), up);
	Vector3 v1 = Vector3Add(Vector3Add(position, right), up);
	Vector3 v2 = Vector3Subtract(Vector3Add(position, right), up);
	Vector3 v3 = Vector3Subtract(Vector3Subtract(position, right), up);

	rlSetTexture(atlas.id);
	rlBegin(RL_QUADS);
	rlColor4ub(tint.r, tint.g, tint.b, tint.a);

	rlTexCoord2f(tile.u0, tile.v0);
	rlVertex3f(v0.x, v0.y, v0.z);
	rlTexCoord2f(tile.u0, tile.v1);
	rlVertex3f(v3.x, v3.y, v3.z);
	rlTexCoord2f(tile.u1, tile.v1);
	rlVertex3f(v2.x, v2.y, v2.z);
	rlTexCoord2f(tile.u1, tile.v0);
	rlVertex3f(v1.x, v1.y, v1.z);

	rlEnd();
	rlSetTexture(0);
}

list<particlesPackage> particlesList;
Vector3 particleSize = { 0.1,0.1,0.1 };
Vector3 particleStartPos = { -0.05, -0.05, -0.05 };
void updateParticles() {
	for (auto packageIt = particlesList.begin(); packageIt != particlesList.end(); ) {
		particlesPackage& package = *packageIt;
		if (package.particlesAlive == 0)
		{
			packageIt = particlesList.erase(packageIt); // erase returns the next iterator
			continue;
		}

		float gravity = package.gravityForce / 3;

		for (particleInfo& particle : package.particles) {
			if (particle.alive == false) {
				continue;
			}
			AtlasTile& particleTexture = particle.texture; //texture coord in atlas
			Vector3& particleForce = particle.directionalForce;
			
			collisionCheck(particleStartPos, particleSize, particle.coord, particleForce);
			particleForce.x *= 0.99f;
			particleForce.y *= 0.99f;
			if (particleForce.z < 0.001 && particleForce.z > -0.001) {
				particleForce.x /= 2;
				particleForce.y /= 2;
			}
			particleForce.z -= gravity;
			if (particleForce.z < -0.1) {
				particleForce.z = -0.1;
			}
			DrawBillboardAtlas(
				camera,
				atlas,
				particle.texture,
				particle.coord,
				particle.squaresize,
				particle.squaresize,
				WHITE
			);
			particle.lifeTime--;
			if (particle.lifeTime <= 0) {
				particle.alive = false;
				package.particlesAlive--;
			}
		}
		++packageIt;
	}
}

int quant = 40;

void createParticles(int3D& chunkCoord, Vector3 blockCoord, BlockID blockId, float gravityForce, float goThroughBlock) {
	vector<particleInfo> particles = {};
	Vector3 chunkCoordToFloat = Vector3Add(blockCoord,  Vector3{ (float)chunkCoord.x * chunkXlength, (float)chunkCoord.y * chunkXlength, (float)chunkCoord.z * chunkXlength });
	chunkCoordToFloat = Vector3Add(chunkCoordToFloat, Vector3{0.5, 0.5, 0.5});
	for (int i = 0; i < quant; i++) {
		Vector3 randomPos = Vector3{ RandomFloatSigned01(0.01, 0.5), RandomFloatSigned01(0.01, 0.5), RandomFloatSigned01(0.01, 0.5) };
		AtlasTile tileCoord = blockIdDefinition[blockId].texture[randomInt(0, 5)]; //original
		//shrink it to be 4x4
		float randX_PixelPos = PIXEL_SIZE * RandomFloat(0,12);
		float randY_PixelPos = PIXEL_SIZE * RandomFloat(0,12);
		tileCoord.u1 -= PIXEL_SIZE * 12;
		tileCoord.v1 -= PIXEL_SIZE * 12;
		tileCoord = AtlasTile{ 
			tileCoord.u0 + randX_PixelPos, 
			tileCoord.v0 + randY_PixelPos, 
			tileCoord.u1 + randX_PixelPos, 
			tileCoord.v1 + randY_PixelPos 
		};
		particles.push_back(
			particleInfo{
				Vector3Add(chunkCoordToFloat, randomPos),
				Vector3Divide(randomPos, {20,20,10}),
				tileCoord,
				30 + (int)(50 * RandomFloatSigned01(1.0, 1.2)),
				true,
				0.2f * RandomFloat(0.5, 1)
			}
		);
	}
	particlesList.push_back(
		particlesPackage{
			chunkCoord,
			gravityForce,
			goThroughBlock,
			particles,
			quant
		}
	);
}