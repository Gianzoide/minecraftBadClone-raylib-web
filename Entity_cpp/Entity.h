#include <iostream>

#include "../chunk_cpp/chunk.h"

#include <raylib.h> 
#include <raymath.h>
#include <vector>
#include <list>
#include <memory>

using namespace std;

#pragma once


enum Projection {
    _3D,
    _2D,
};
enum PlayerGamemode {
    creative,
    survivel,
    spectator
};

struct ItemInfo {
    int whereSlot = -1;
    int currentQuantity = 0;
    int maxQuantity = 64;
    string nameOG = "";
    string currentName = "";
    BlockID blockId = BlockID::Air;
    Projection projection = _3D;
};  

class Entity {
public:
    Vector3 positionTimesChunk;
    collisionBox collision_Box;
    int /*place holder*/ skin = 0;
    Vector3 Velocity = {0.0, 0.0, 0.0};
    bool isGrounded = false;
    float reachabilityDistance = 3.0;
    Vector3 cameraVisionposition;
    list<ItemInfo> inventory = {};
    int hotbarSlotBeeningUsed = 0;
    PlayerGamemode gamemode = creative;

    Entity(Vector3 pos, collisionBox col, int s, float reach, Vector3 camVision) {
        positionTimesChunk = pos;
        collision_Box = col;
        skin = s;
        reachabilityDistance = reach;
        cameraVisionposition = camVision;
    }

    ItemInfo getThingPlayerHolding() {
        for (ItemInfo& item : inventory) {
            if (item.whereSlot == hotbarSlotBeeningUsed) {
                return item;
            }
        }
        return ItemInfo{};
    }
    ItemInfo* getItemInInventory(int slotPos) {
        for (ItemInfo& item : inventory) {
            if (item.whereSlot == slotPos) {
                return &item;
            }
        }
        return nullptr;
    }
    void removeItemFromInventory(int slotPos)
    {
        for (auto it = inventory.begin(); it != inventory.end(); ++it)
        {
            if (it->whereSlot == slotPos)
            {
                inventory.erase(it);
                return;
            }
        }
    }
};

extern list<unique_ptr<Entity>> entities;

extern Entity* main_player;