

#include "movement.h"
#include "collisionCheck.h"
#include "../mouse_cpp/mouseDirectionSeeing.h"
#include "../usefullFunc_cpp/usefullFunc.h"
#include "../rendering/GUI_cpp/GUI.h"
using namespace std;

float yaw = 0.0f;
float pitch = 0.0f;

Vector2 mouseDelta;
float sensitivity = 0.003f;
Vector3 DirectionForward{};
Vector3 forwardFlat{};
Vector3 directionRight{};

void updateForwardAndOthers() {
    // Mouse look
    mouseDelta = GetMouseDelta();

    yaw -= mouseDelta.x * sensitivity;
    pitch -= mouseDelta.y * sensitivity;

    // Clamp pitch (avoid flipping)
    if (pitch > 1.5f) pitch = 1.5f;
    if (pitch < -1.5f) pitch = -1.5f;

    //direction vector
    DirectionForward = {};
    DirectionForward.x = cosf(pitch) * cosf(yaw);
    DirectionForward.y = cosf(pitch) * sinf(yaw);
    DirectionForward.z = sinf(pitch);

    DirectionForward = Vector3Normalize(DirectionForward);

    forwardFlat = { DirectionForward.x, DirectionForward.y, 0.0 };
    forwardFlat = Vector3Normalize(forwardFlat);
    directionRight = Vector3Normalize(Vector3CrossProduct(forwardFlat, camera.up));
}

void updateMovement(Camera3D& camera, Entity* main_player) {
    
    // keyboard movement
    float dt = GetFrameTime();

    // SETTINGS
    float baseSpeed = 5.0f;
    float sprintBonus = IsKeyDown(KEY_CAPS_LOCK) ? 20.0f : 0.0f;
    float gravity = 29.0f;
    float jumpStrength = 8.0f;
    PlayerGamemode playersMode = main_player->gamemode;

    // HORIZONTAL INPUT
    Vector3 movementDelta = { 0.0f, 0.0f, 0.0f };

    // APPLY GRAVITY
    if (playersMode == survivel || playersMode == creative) {
        main_player->Velocity.z -= gravity * dt;
    }
    else if(playersMode == spectator){
        if (inventoryopened == false) {
            if (IsKeyDown(KEY_LEFT_SHIFT)) movementDelta = Vector3Subtract(movementDelta, camera.up);
            if (IsKeyDown(KEY_SPACE)) movementDelta = Vector3Add(movementDelta, camera.up);
        }
    }

    if (IsKeyDown(KEY_W)) movementDelta = Vector3Add(movementDelta, forwardFlat);
    if (IsKeyDown(KEY_S)) movementDelta = Vector3Subtract(movementDelta, forwardFlat);
    if (IsKeyDown(KEY_D)) movementDelta = Vector3Add(movementDelta, directionRight);
    if (IsKeyDown(KEY_A)) movementDelta = Vector3Subtract(movementDelta, directionRight);

    // normalize horizontal movement
    if (Vector3Length(movementDelta) > 0.0f) {
        movementDelta = Vector3Normalize(movementDelta);
        movementDelta = Vector3Scale(movementDelta, (baseSpeed + sprintBonus) * dt);
    }

    // jump
    if (inventoryopened == false && IsKeyDown(KEY_SPACE) && main_player->isGrounded) {
        main_player->Velocity.z = jumpStrength + sprintBonus;
    }

    // APPLY VERTICAL VELOCITY
    if (playersMode == survivel || playersMode == creative) {
        movementDelta.z = main_player->Velocity.z * dt;
    }
    if (inventoryopened == true) {
        movementDelta = {0, 0, movementDelta.z };
    }

    // APPLY MOVEMENT (WITH COLLISION)
    if (playersMode == survivel || playersMode == creative) {
        updateCollision(main_player->collision_Box.startposition, main_player->collision_Box.size, main_player->positionTimesChunk, movementDelta, main_player);
    }
    else if (playersMode == spectator) {
        main_player->positionTimesChunk = Vector3Add(main_player->positionTimesChunk, movementDelta);
    }

    if (inventoryopened == true) {
        return;
    }

/*    // STOP FALLING WHEN HITTING GROUND
    // (detect if vertical movement was blocked)
    if (movementDelta.z < 0.0f) {
        // try moving down again slightly
        Vector3 test = { 0, 0, -0.01f };
        Vector3 before = main_player->positionTimesChunk;

        collision(*main_player, test);

        if (main_player->positionTimesChunk.z == before.z) {
            main_player->Z_Velocity = 0.0f; // hit ground
        }

        main_player->positionTimesChunk = before;
    }*/

    int hotbarMovement = -GetMouseWheelMove();
    if (main_player->hotbarSlotBeeningUsed + hotbarMovement < 0) {
        main_player->hotbarSlotBeeningUsed = 8;
    }else 
    if (main_player->hotbarSlotBeeningUsed + hotbarMovement > 8) {
        main_player->hotbarSlotBeeningUsed = 0;
    }
    else {
        main_player->hotbarSlotBeeningUsed += hotbarMovement;
    }

    if (IsKeyPressed(KEY_ONE)) {
        main_player->hotbarSlotBeeningUsed = 0;
    }
    if (IsKeyPressed(KEY_TWO)) {
        main_player->hotbarSlotBeeningUsed = 1;
    }
    if (IsKeyPressed(KEY_THREE)) {
        main_player->hotbarSlotBeeningUsed = 2;
    }
    if (IsKeyPressed(KEY_FOUR)) {
        main_player->hotbarSlotBeeningUsed = 3;
    }
    if (IsKeyPressed(KEY_FIVE)) {
        main_player->hotbarSlotBeeningUsed = 4;
    }
    if (IsKeyPressed(KEY_SIX)) {
        main_player->hotbarSlotBeeningUsed = 5;
    }
    if (IsKeyPressed(KEY_SEVEN)) {
        main_player->hotbarSlotBeeningUsed = 6;
    }
    if (IsKeyPressed(KEY_EIGHT)) {
        main_player->hotbarSlotBeeningUsed = 7;
    }
    if (IsKeyPressed(KEY_NINE)) {
        main_player->hotbarSlotBeeningUsed = 8;
    }


}