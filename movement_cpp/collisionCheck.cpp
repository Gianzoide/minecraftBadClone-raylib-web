#include "collisionCheck.h"

using namespace std;
#include <cmath>
#include <string>

struct collisionBox_plus_chunkCoord {
	Vector3 coord;
	collisionBox* collision_Box;
	int3D chunkCoords = {0,0,0};
};

Vector3 whereEntityShouldGo_addingCoords(
	Vector3& entityPos,
	collisionBox& entityCollision,
	Vector3& blockPos,
	collisionBox& blockCollision
) {
	Vector3 result = { 0, 0, 0 };

	Vector3 entitySize = entityCollision.size;
	Vector3 blockSize = blockCollision.size;

	// Compute centers
	Vector3 entityCenter = {
		entityPos.x + entitySize.x * 0.5f,
		entityPos.y + entitySize.y * 0.5f,
		entityPos.z + entitySize.z * 0.5f
	};

	Vector3 blockCenter = {
		blockPos.x + blockSize.x * 0.5f,
		blockPos.y + blockSize.y * 0.5f,
		blockPos.z + blockSize.z * 0.5f
	};

	// Distance between centers
	float dx = entityCenter.x - blockCenter.x;
	float dy = entityCenter.y - blockCenter.y;
	float dz = entityCenter.z - blockCenter.z;

	// Total half sizes
	float px = (entitySize.x + blockSize.x) * 0.5f - abs(dx);
	float py = (entitySize.y + blockSize.y) * 0.5f - abs(dy);
	float pz = (entitySize.z + blockSize.z) * 0.5f - abs(dz);

	// If no overlap, return zero
	if (px <= 0 || py <= 0 || pz <= 0) {
		return result;
	}

	// Find smallest penetration axis
	if (px < py && px < pz) {
		result.x = (dx > 0) ? px : -px;
	}
	else if (py < pz) {
		result.y = (dy > 0) ? py : -py;
	}
	else {
		result.z = (dz > 0) ? pz : -pz;
	}

	return result;
}

vector<int3D> getChunkCoordsThatEntityOccupies (
	Vector3 entityPosInChunk,
	Vector3& entityCollisionSize,
	int3D baseChunk
) {
	vector<int3D> result;

	Vector3 halfSize = Vector3Scale(entityCollisionSize, 0.5f);

	Vector3 min = entityPosInChunk;
	Vector3 max = Vector3Add(entityPosInChunk, entityCollisionSize);

	int minChunkX = floor(min.x / chunkXlength);
	int maxChunkX = floor(max.x / chunkXlength);

	int minChunkY = floor(min.y / chunkYlength);
	int maxChunkY = floor(max.y / chunkYlength);

	int minChunkZ = floor(min.z / chunkZlength);
	int maxChunkZ = floor(max.z / chunkZlength);

	for (int x = minChunkX; x <= maxChunkX; x++) {
		for (int y = minChunkY; y <= maxChunkY; y++) {
			for (int z = minChunkZ; z <= maxChunkZ; z++) {
				result.push_back(int3D{
					baseChunk.x + x,
					baseChunk.y + y,
					baseChunk.z + z
					});
			}
		}
	}

	return result;
}


bool isColliding(Vector3 pos1, Vector3 size1, Vector3 pos2, Vector3 size2) {
	return (
		pos1.x < pos2.x + size2.x &&
		pos1.x + size1.x > pos2.x &&

		pos1.y < pos2.y + size2.y &&
		pos1.y + size1.y > pos2.y &&

		pos1.z < pos2.z + size2.z &&
		pos1.z + size1.z > pos2.z
		);
}

// Returns the flat block indices within ONE chunk whose 1x1x1 cell
// overlaps the entity's collision box, expanded along coordsMovedAction
// so blocks along the swept path aren't skipped.
vector<int> blockThatIsInTheSameCoordAsEntity(
	Vector3 entityPositionInThisChunk,
	Vector3 collicionBoxSize,
	Vector3 coordsMovedAction)
{
	vector<int> result = {};

	Vector3 minPos = entityPositionInThisChunk;
	Vector3 maxPos = Vector3Add(entityPositionInThisChunk, collicionBoxSize);

	// Expand the AABB to cover the movement (swept volume)
	if (coordsMovedAction.x != 0.0f || coordsMovedAction.y != 0.0f || coordsMovedAction.z != 0.0f) {
		Vector3 movedMin = Vector3Add(minPos, coordsMovedAction);
		Vector3 movedMax = Vector3Add(maxPos, coordsMovedAction);

		minPos.x = std::min(minPos.x, movedMin.x);
		minPos.y = std::min(minPos.y, movedMin.y);
		minPos.z = std::min(minPos.z, movedMin.z);

		maxPos.x = std::max(maxPos.x, movedMax.x);
		maxPos.y = std::max(maxPos.y, movedMax.y);
		maxPos.z = std::max(maxPos.z, movedMax.z);
	}

	int minX = (int)std::floor(minPos.x);
	int minY = (int)std::floor(minPos.y);
	int minZ = (int)std::floor(minPos.z);
	int maxX = (int)std::floor(maxPos.x);
	int maxY = (int)std::floor(maxPos.y);
	int maxZ = (int)std::floor(maxPos.z);

	// Clamp to this chunk's bounds — coords outside belong to a
	// neighboring chunk and are handled in that chunk's own pass.
	minX = std::max(minX, 0);
	minY = std::max(minY, 0);
	minZ = std::max(minZ, 0);
	maxX = std::min(maxX, chunkXlength - 1);
	maxY = std::min(maxY, chunkYlength - 1);
	maxZ = std::min(maxZ, chunkZlength - 1);

	for (int x = minX; x <= maxX; x++) {
		for (int y = minY; y <= maxY; y++) {
			for (int z = minZ; z <= maxZ; z++) {
				result.push_back(Index(x, y, z));
			}
		}
	}

	return result;
}

vector<collisionBox_plus_chunkCoord> checkWhatBlocksEntityTouched(bool& anyNullChunks, Vector3& collicionBoxStartPos, Vector3& collicionBoxSize, Vector3& entityPos, Vector3* coordsMovedAction = nullptr) {
	vector<collisionBox_plus_chunkCoord> result = {};
	Vector3 position = Vector3Add(entityPos, collicionBoxStartPos);
	Vector3 positionNotMoved = position;

	if (coordsMovedAction->x != 0.0f || coordsMovedAction->y != 0.0f || coordsMovedAction->z != 0.0f) {
		position = Vector3Add(position, *coordsMovedAction);
	}

	// entity.collisionBoxes.startposition
	// using entity and chunkCoord
	// check if the entity ocupies other chunk and
	// and if so get all chunks it ocupies
	int3D chunkCoord = getCoordChunk_By_positionTimesChunk(position);
	Vector3 entityPositionInChunk = { position.x - (chunkCoord.x * chunkXlength), position.y - (chunkCoord.y * chunkYlength), position.z - (chunkCoord.z * chunkZlength) };

	//cout << "\n entityPositionInChunk:" << entityPositionInChunk.x << "/" << entityPositionInChunk.y << "/" << entityPositionInChunk.z;
 	vector<int3D> ocupiedChunksByEntity = getChunkCoordsThatEntityOccupies(entityPositionInChunk, collicionBoxSize, chunkCoord);
	// for each block in each chunk check if entity touches block
	for (int3D& coords : ocupiedChunksByEntity) {
		Chunk* chunk = getChunk(coords.x, coords.y, coords.z);
		if (chunk == nullptr) {
			anyNullChunks = true;
			return {};
		}

		Vector3 chunkOffset = {
			((float)coords.x * chunkXlength),
			((float)coords.y * chunkYlength),
			((float)coords.z * chunkZlength)
		};
		Vector3 entityPositionInThisChunk = Vector3Subtract(positionNotMoved, chunkOffset);

		vector<int> blocksInCoord = blockThatIsInTheSameCoordAsEntity(entityPositionInThisChunk,collicionBoxSize,*coordsMovedAction);
		for (int coord : blocksInCoord) {
			blockInfo& block = chunk->blocks[coord];

			int3D omgBruh = IndexToXYZ(coord);
			for (collisionBox& theCollision : blockIdDefinition[block.id].collision) {
				Vector3 blockWorldPosition = { (float)omgBruh.x, (float)omgBruh.y, (float)omgBruh.z};
				blockWorldPosition = Vector3Add(blockWorldPosition, theCollision.startposition);
				blockWorldPosition = Vector3Add(blockWorldPosition, chunkOffset);

				bool wasCollidingBefore = isColliding(
					positionNotMoved,
					collicionBoxSize,
					blockWorldPosition,
					theCollision.size
				);

				bool isMovingIntoBlock =
					(coordsMovedAction->x > 0 && position.x < blockWorldPosition.x) ||
					(coordsMovedAction->x < 0 && position.x > blockWorldPosition.x) ||
					(coordsMovedAction->y > 0 && position.y < blockWorldPosition.y) ||
					(coordsMovedAction->y < 0 && position.y > blockWorldPosition.y) ||
					(coordsMovedAction->z > 0 && position.z < blockWorldPosition.z) ||
					(coordsMovedAction->z < 0 && position.z > blockWorldPosition.z);

				if (
					(!wasCollidingBefore || isMovingIntoBlock)
					&& 
					isColliding(
					position, collicionBoxSize,
					blockWorldPosition, theCollision.size)
					)
				{
					result.push_back(collisionBox_plus_chunkCoord{ blockWorldPosition, &theCollision });
				}
			}
		}
	}
	return result;
}

void collisionCheck(Vector3& collicionBoxStartPos, Vector3& collicionBoxSize, Vector3& entityPos, Vector3& movement, Entity* entity) {
	bool anyNullChunks = false;
	Vector3 pos = entityPos;

	// ---------- X AXIS
	if (movement.x != 0.0f) {

		pos.x += movement.x;

		vector<collisionBox_plus_chunkCoord> collisions = checkWhatBlocksEntityTouched(anyNullChunks, collicionBoxStartPos, collicionBoxSize, entityPos, &movement);
		if (anyNullChunks) {
			return;
		}
		Vector3 entityWorldPos = Vector3Add(pos, collicionBoxStartPos);

		float bestPush = 0.0f;
		collisionBox coliBruh = collisionBox{ collicionBoxSize, collicionBoxStartPos };
		for (collisionBox_plus_chunkCoord& c : collisions) {

			Vector3 push = whereEntityShouldGo_addingCoords(
				entityWorldPos,
				coliBruh,
				c.coord,
				*c.collision_Box
			);

			if (fabs(push.x) > fabs(bestPush)) {
				bestPush = push.x;
			}
		}

		pos.x += bestPush;
		if (bestPush != 0.0f) {
			movement.x = 0.0f;
		}
	}

	// ---------- Y AXIS
	if (movement.y != 0.0f) {

		pos.y += movement.y;

		vector<collisionBox_plus_chunkCoord> collisions = checkWhatBlocksEntityTouched(anyNullChunks, collicionBoxStartPos, collicionBoxSize, entityPos, &movement);
		if (anyNullChunks) {
			return;
		}
		Vector3 entityWorldPos = Vector3Add(pos, collicionBoxStartPos);

		float bestPush = 0.0f;
		collisionBox coliBruh = collisionBox{ collicionBoxSize, collicionBoxStartPos };
		for (auto& c : collisions) {

			Vector3 push = whereEntityShouldGo_addingCoords(
				entityWorldPos,
				coliBruh,
				c.coord,
				*c.collision_Box
			);

			if (fabs(push.y) > fabs(bestPush)) {
				bestPush = push.y;
			}
		}

		pos.y += bestPush;
		if (bestPush != 0.0f) {
			movement.y = 0.0f;
		}
	}

	// ---------- Z AXIS
	if (movement.z != 0.0f) {

		pos.z += movement.z;

		vector<collisionBox_plus_chunkCoord> collisions = checkWhatBlocksEntityTouched(anyNullChunks, collicionBoxStartPos, collicionBoxSize, entityPos, &movement);
		if (anyNullChunks) {
			return;
		}
		Vector3 entityWorldPos = Vector3Add(pos, collicionBoxStartPos);

		float bestPush = 0.0f;
		collisionBox coliBruh = collisionBox{ collicionBoxSize, collicionBoxStartPos };
		for (auto& c : collisions) {

			Vector3 push = whereEntityShouldGo_addingCoords(
				entityWorldPos,
				coliBruh,
				c.coord,
				*c.collision_Box
			);

			if (fabs(push.z) > fabs(bestPush)) {
				bestPush = push.z;
			}
		}
		pos.z += bestPush;
		if (bestPush != 0.0f) {
			movement.z = 0.0f;
		}
		if (entity != nullptr) {
			if (bestPush != 0.0f) {
				entity->Velocity.z = 0.0f;
				if (movement.z <= 0 && bestPush >= 0) { // not get jump from celling
					entity->isGrounded = true;
				}
			}
			else {
				entity->isGrounded = false;
			}
		}
	}
	entityPos = pos;
}

void updateCollision(Vector3& collicionBoxStartPos, Vector3& collicionBoxSize, Vector3& entityPos, Vector3& movement, Entity* entity) {

	float maxStepSize = 0.2f;

	float maxMove = fmax(fabs(movement.x),
		fmax(fabs(movement.y), fabs(movement.z)));

	int steps = (int)ceil(maxMove / maxStepSize);

	if (steps < 1) steps = 1;

	Vector3 step = {
		movement.x / steps,
		movement.y / steps,
		movement.z / steps
	};

	for (int i = 0; i < steps; i++) {
		collisionCheck(collicionBoxStartPos, collicionBoxSize, entityPos, step, entity);
	}
}