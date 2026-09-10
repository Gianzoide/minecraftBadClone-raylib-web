
#pragma once

#include <iostream>

#include "../Entity_cpp/Entity.h"
#include "../chunk_cpp/chunk.h"
#include <raymath.h>

void updateCollision(Vector3& collicionBoxStartPos, Vector3& collicionBoxSize, Vector3& entityPos, Vector3& movement, Entity* entity = nullptr);

void collisionCheck(Vector3& collicionBoxStartPos, Vector3& collicionBoxSize, Vector3& entityPos, Vector3& movement, Entity* entity = nullptr);