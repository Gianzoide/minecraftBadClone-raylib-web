#pragma once

#include <iostream>
#include <raylib.h>
#include "raymath.h"

#include "../camera_cpp/camera.h"
#include "../Entity_cpp/Entity.h"

void updateMovement(Camera3D& camera, Entity* main_player);

extern float yaw;
extern float pitch;
extern bool normalMode ;
extern Vector2 mouseDelta;
extern float sensitivity;
extern Vector3 DirectionForward;
extern Vector3 directionRight;

void updateForwardAndOthers();