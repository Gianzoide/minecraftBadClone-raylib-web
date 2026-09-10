#include <iostream>

using namespace std;

#include "camera.h"
#include "../movement_cpp/movement.h"
#include "../usefullFunc_cpp/usefullFunc.h"

Camera3D camera;

void createCamera() {
    camera = { 0 };
    camera.position = { 0.0f, 0.0f, 0.0f };
    camera.target = { 0.0f, 0.0f, 0.0f };
    camera.up = { 0.0f, 0.0f, 1.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    DisableCursor();
}

void updateCamera() {

    Vector3 finalCameraPosition = Vector3Add(
        main_player->positionTimesChunk,
        main_player->cameraVisionposition
    );

    Vector3 direction = Vector3Add(finalCameraPosition, DirectionForward);
   
    camera.position = finalCameraPosition;
    camera.target = direction;
    camera.projection = CAMERA_PERSPECTIVE;
}