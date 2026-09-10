/*
worst code ever made btw
*/
#include <iostream>

#include <raylib.h>
#include <algorithm>

#include "GUI.h"
#include "hotBar.h"
#include "../redering.h"
#include "../../blocks.h"

#include "../../Entity_cpp/Entity.h"

using namespace std;

void DrawCrosshair()
{
    int cx = GetScreenWidth() / 2;
    int cy = GetScreenHeight() / 2;

    int size = 10;      // half length of lines
    int thickness = 2;

    DrawRectangle(cx - thickness / 2, cy - size, thickness, size * 2, BLACK); // vertical
    DrawRectangle(cx - size, cy - thickness / 2, size * 2, thickness, BLACK); // horizontal
}

float scale = 2.0f;
float spaceFromBottom = 0.975f;
float bruh = 0;
float divide = WindowSizeY;
float divideInHand = WindowSizeY / 8;


// Camera used only for rendering hotbar 3D items
Camera3D hotbarCamera = {
    { 0.1f, 0.0f, 0.0f },   // position
    { 0.0f, 0.0f, 0.0f },   // target
    { 0.0f, 0.0f, 1.5f },   // up
    20.0f,
    CAMERA_PERSPECTIVE
};

Vector3 centered3D = { -0.5, -0.5, -0.5 };
float bruhMoment = 0.00235;
float bruhZMomentZZZ = 0.0015;


void DrawItem3D(BlockID blockId,
    Shape blockShape,
    Texture2D& theAtlas,
    int additionalPos,
    bool inventoryOpen = false,
    bool itemsInTheMainSlot = false,
    Vector2 posInWindow = {}
    )
{   
    float resizeIdk = 600 / (float)WindowSizeY;

    float additionalYPos = 1;
    if (itemsInTheMainSlot == true) {
        additionalYPos = 2.71;
        while (additionalPos >= 9) {
            additionalPos -= 9 ;
            additionalYPos += 1.4;
        }
    }
    Vector3 pos3D;
    float itemResizeForInventory = 1.0f;
    float bruhMomentBruh;
    if (inventoryOpen == false) {
        pos3D = {0,-0.0094,-0.0155};
        bruhMomentBruh = bruhMoment ;
    }
    else {
        pos3D = { 0,-0.0085,-0.0079 };
        itemResizeForInventory = 1.2f;
        bruhMomentBruh = 0.00212;
    }

    if (posInWindow == Vector2{}) {
        pos3D.y += bruhMomentBruh * additionalPos;
        pos3D.y *= resizeIdk;
        pos3D.z += (bruhZMomentZZZ * additionalYPos * resizeIdk) - bruhZMomentZZZ;

    }else{
        pos3D.y *= -(posInWindow.x - (WindowSizeX / 2)) / 145;
        pos3D.z *= (posInWindow.y - (WindowSizeY / 2)) / 140;
        //cout << endl << "y: " << pos3D.y << " /z: " << pos3D.z;
    }
    int index = 0;
    for (vector<SquareFace>& faces : ChooseshapeToRender[(int)blockShape])
    {
        for (SquareFace& theFace : faces) {
            array <Vector3, 4> verticies = theFace.face;
            //block.texture[theFace.shape];
            AtlasTile tile = blockIdDefinition[blockId].texture[index];
            DrawItemFace(
                Vector3Add( rotate_Vector3( Vector3Divide (Vector3Add( verticies[0], centered3D), {divide * itemResizeForInventory, divide * itemResizeForInventory, divide * itemResizeForInventory}), { 375,375,45 + bruh }) , pos3D),
                Vector3Add( rotate_Vector3( Vector3Divide (Vector3Add( verticies[1], centered3D), {divide * itemResizeForInventory, divide * itemResizeForInventory, divide * itemResizeForInventory}), { 375,375,45 + bruh }) , pos3D),
                Vector3Add( rotate_Vector3( Vector3Divide (Vector3Add( verticies[2], centered3D), {divide * itemResizeForInventory, divide * itemResizeForInventory, divide * itemResizeForInventory}), { 375,375,45 + bruh }) , pos3D),
                Vector3Add( rotate_Vector3( Vector3Divide (Vector3Add( verticies[3], centered3D), {divide * itemResizeForInventory, divide * itemResizeForInventory, divide * itemResizeForInventory}), { 375,375,45 + bruh }) , pos3D),
                tile,
                theAtlas
            );
        }
        index++;
    }
}
void Draw2DItemInInventory(BlockID blockId, int additionalPos, bool itemsInTheMainSlot = false, Vector2 posInWindow = {})
{
    auto& block = blockIdDefinition[blockId];
    AtlasTile tile = block.texture[0];

    // Mesma referência de posição usada pro sprite grande do inventário
    Vector2 inventoryPos = {
        (GetScreenWidth() / 2.0f) - (texturesGUI[2].width * scale / 2.0f),
        (GetScreenHeight() / 2.0f) - (texturesGUI[2].height * scale / 2.0f)
    };

    int slotCount = 9;
    float slotWidth = ((texturesGUI[2].width - 16) * scale) / slotCount;
    float itemslotScale = 0.9f;

    float additionalYPos = 0.05;
    if (itemsInTheMainSlot == true) {
        additionalYPos = 1.3;
        while (additionalPos >= 9) {
            additionalPos -= 9;
            additionalYPos += 1.0;
        }
    }
    Vector2 itemPos;
    if (posInWindow == Vector2{}) {
        itemPos = Vector2{
            (float)(inventoryPos.x + (7 * scale) + (slotWidth * additionalPos * 1.015) + 1),
            (float)(inventoryPos.y - (TILE_SIZE * scale) - (7 * scale) + (texturesGUI[2].height * scale) - (slotWidth * additionalYPos))
        };
    }
    else {
        itemPos = posInWindow;
    }

    Rectangle source = {
        tile.u0 * atlas.width,
        tile.v0 * atlas.height,
        (tile.u1 - tile.u0) * atlas.width,
        (tile.v1 - tile.v0) * atlas.height
    };

    Rectangle dest = {
        itemPos.x,
        itemPos.y,
        16.0f * scale ,
        16.0f * scale 
    };

    DrawTexturePro(atlas, source, dest, { 0, 0 }, 0.0f, WHITE);
}
Vector3 animationMotionEnd = { 345,280,50 };
Vector3 animationMotionBackToBegining = { 367,367,60 };
Vector3 animationMotionValue = animationMotionBackToBegining;
//Vector3 movingAnimEnd = {0,0,0};

Vector3 animationDiff = { 
    (animationMotionBackToBegining.x - animationMotionEnd.x) / 8,
    (animationMotionBackToBegining.y - animationMotionEnd.y) / 8, 
    (animationMotionBackToBegining.z - animationMotionEnd.z) / 8 
};
bool goingBack = true;

void drawItemInHand(BlockID blockId, Shape blockShape,Texture2D& theAtlas) {
    bool rightClicked = false;
    bool Left_Clicked = false;
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) rightClicked = true;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) Left_Clicked = true;
    float resizeIdk = 600 / (float)WindowSizeY;
    Vector3 pos3D = { 0,0.02,-0.018 };
    pos3D.y += bruhMoment;
    pos3D.y *= resizeIdk;
    pos3D.z += (bruhZMomentZZZ * resizeIdk) - bruhZMomentZZZ;
    int index = 0;
    //animationMotionValue.z += 0.1;

    if (goingBack == false) {
        if (animationMotionValue.y > animationMotionEnd.y) {
            animationMotionValue -= animationDiff;
            pos3D.y *= 0.9;
            pos3D.z *= 0.9;
        }
        else {
            animationMotionValue = animationMotionEnd;
            goingBack = true;
        }
    }
    else if(goingBack == true){
        if (animationMotionValue.y < animationMotionBackToBegining.y) {
            animationMotionValue += animationDiff;
            pos3D.y *= 0.9;
            pos3D.z *= 0.9;
        }
        else {
            animationMotionValue = animationMotionBackToBegining;
            if (Left_Clicked) {
                if (inventoryopened == false) {
                    goingBack = false;
                }
            }
        }
    }
    if (!Left_Clicked) {
        goingBack = true;
    }
 
    for (vector<SquareFace>& faces : ChooseshapeToRender[(int)blockShape])
    {
        for (SquareFace& theFace : faces) {
            array <Vector3, 4> verticies = theFace.face;
            //block.texture[theFace.shape];
            AtlasTile tile = blockIdDefinition[blockId].texture[index];
            BeginShaderMode(cutoutShader);
            DrawItemFace(
                Vector3Add(rotate_Vector3(Vector3Divide(Vector3Add(verticies[0], centered3D), { divideInHand, divideInHand, divideInHand }), { animationMotionValue }), pos3D),
                Vector3Add(rotate_Vector3(Vector3Divide(Vector3Add(verticies[1], centered3D), { divideInHand, divideInHand, divideInHand }), { animationMotionValue }), pos3D),
                Vector3Add(rotate_Vector3(Vector3Divide(Vector3Add(verticies[2], centered3D), { divideInHand, divideInHand, divideInHand }), { animationMotionValue }), pos3D),
                Vector3Add(rotate_Vector3(Vector3Divide(Vector3Add(verticies[3], centered3D), { divideInHand, divideInHand, divideInHand }), { animationMotionValue }), pos3D),
                tile,
                theAtlas
            );
            EndShaderMode();
        }
        index++;
    }
}

bool mouseHoldingItem = false; // false for when player is holding nothing 
void movingItemInInventory(Vector2 mousePos) {

    BeginMode3D(hotbarCamera);
    for (const ItemInfo& item : main_player->inventory) {
        if (item.projection != Projection::_3D) continue;
        if (item.whereSlot != -999) continue;
        BlockID blockId = item.blockId;
        DrawItem3D(blockId, blockIdDefinition[blockId].shapeTexture, atlas, item.whereSlot, true, false, mousePos);
        break;
    }
    EndMode3D();
    for (const ItemInfo& item : main_player->inventory) {
        if (item.projection != Projection::_2D) continue;
        if (item.whereSlot != -999) continue;
        Draw2DItemInInventory(item.blockId, item.whereSlot, false, mousePos);
        break;
    }
        
}

void drawHotbar() {
    bruh += 1.0f; //important for item 3d rotation

    BeginMode3D(hotbarCamera);
    for (const ItemInfo& item : main_player->inventory) // this so the hand is behind Hotbar
    {
        if (item.projection != Projection::_3D) {
            continue;
        }

        BlockID blockId = item.blockId;

        if (item.whereSlot == main_player->hotbarSlotBeeningUsed) {
            drawItemInHand(blockId, blockIdDefinition[blockId].shapeTexture, atlas);
        }
    }
    EndMode3D();

    Vector2 hotbarPos = {
        (GetScreenWidth() / 2.0f) - (texturesGUI[0].width * scale / 2.0f),
        (GetScreenHeight() * spaceFromBottom) - (texturesGUI[0].height * scale)
    };
    DrawTextureEx(texturesGUI[0], hotbarPos, 0.0f, scale, WHITE);

    // --- Slot size (based on hotbar layout) ---
    int slotCount = 9;
    float slotWidth = (texturesGUI[0].width * scale) / slotCount;

    // --- Selected slot position ---
    Vector2 selectedPos = {
        hotbarPos.x - (1.8f * scale) + (slotWidth * main_player->hotbarSlotBeeningUsed),
        hotbarPos.y - (1.0f * scale)
    };

    // Draw selected slot
    DrawTextureEx(texturesGUI[1], selectedPos, 0.0f, scale, WHITE);

    float itemslotScale = 0.9f;

    BeginMode3D(hotbarCamera);
    for (const ItemInfo& item : main_player->inventory)
    {
        if (item.projection != Projection::_3D) continue;
        if (item.whereSlot == -999) continue;
        if (item.whereSlot > 8) continue;
        BlockID blockId = item.blockId;

        DrawItem3D(blockId, blockIdDefinition[blockId].shapeTexture, atlas, item.whereSlot);
    }
    EndMode3D();

    // Draw 2D items in hotbar
    for (const ItemInfo& item : main_player->inventory)
    {
        if (item.projection != Projection::_2D) continue;
        if (item.whereSlot == -999) continue;
        if (item.whereSlot > 8) continue;

        auto& block = blockIdDefinition[item.blockId];

        AtlasTile tile = block.texture[0];

        Vector2 itemPos = {
            hotbarPos.x - (1.8f * scale) + (0.99f * slotWidth * item.whereSlot) + slotWidth / 4,
            hotbarPos.y - (1.0f * scale) + slotWidth / 4
        };

        Rectangle source = { tile.u0 * atlas.width,tile.v0 * atlas.height,(tile.u1 - tile.u0) * atlas.width,(tile.v1 - tile.v0) * atlas.height };
        Rectangle dest = { itemPos.x,itemPos.y,TILE_SIZE * scale * itemslotScale,TILE_SIZE * scale * itemslotScale };
        DrawTexturePro(atlas, source, dest, { 0,0 }, 0.0f, WHITE);

    }

    // show quantity of item in every slot in hotbar
    for (ItemInfo& item : main_player->inventory)
    {
        if (item.whereSlot == -999) continue;
        if (item.whereSlot > 8) continue;
        int morethan9 = 0;
        int quantitySlot = item.currentQuantity;
        if (quantitySlot > 9 || quantitySlot < 0) {
            morethan9 = 1;
            if (quantitySlot > 99 || quantitySlot < -9) {
                morethan9 = 2;
            }
        }
        Vector2 itemPos = {
            (hotbarPos.x - (15 * morethan9)) - (1.8f * scale) + (0.99f * slotWidth * (item.whereSlot + 0.5f)) + slotWidth / 4,
            (hotbarPos.y + (TILE_SIZE * scale) - 25) - (1.0f * scale) + slotWidth / 4
        };
        if(quantitySlot <= 0) {
            DrawText(TextFormat("%d", quantitySlot), itemPos.x, itemPos.y, 12.5 * scale, RED);
        }
        else if (quantitySlot != 1) {
            DrawText(TextFormat("%d", quantitySlot), itemPos.x, itemPos.y, 12.5 * scale, WHITE);
        }
    }

    if (inventoryopened == true) {
        Texture2D inventoryIMG = texturesGUI[2];
        Vector2 inventoryPos = {
            (GetScreenWidth() / 2.0f) - (texturesGUI[2].width * scale / 2.0f),
            (GetScreenHeight() / 2.0f) - (texturesGUI[2].height * scale / 2.0f)
        };
        DrawTextureEx(inventoryIMG, inventoryPos, 0.0f, scale, WHITE);

        // drawing white square where mouse is on whitch slot in the inventory
        Vector2 mousePos = GetMousePosition();
        mousePos = { mousePos.x, WindowSizeY - mousePos.y};
        Vector2 InventoryMousePos = Vector2Subtract(mousePos, inventoryPos);
        InventoryMousePos = { InventoryMousePos.x - (7 * scale), InventoryMousePos.y - (7 * scale) };
        float slotWidth = ((inventoryIMG.width - 16) * scale) / 9;
        int maxslotX = 8;
        int maxslotY = 3;
        if (InventoryMousePos.x > 0 && InventoryMousePos.x < (inventoryIMG.width * scale) - (7 * scale * 2) &&
            InventoryMousePos.y > 0 && InventoryMousePos.y < (inventoryIMG.height * scale) - (7 * scale * 2)
            ) {
            //see what slot position the mouse is placed
            int slotX = (int)(InventoryMousePos.x / slotWidth);
            int slotY = (int)(InventoryMousePos.y / slotWidth);
            if (slotY > 0) {
                slotY = (int)((InventoryMousePos.y - (4 * scale)) / slotWidth);
            }
            if (slotX > maxslotX || slotY > maxslotY) {
                goto endthisIf;
            }
            int slotPos = slotY * 9 + slotX;
            ItemInfo* playerItemClicking = main_player->getItemInInventory(slotPos);

            if (slotPos <= 8) {
                Draw2DItemInInventory(BlockID::Air, slotPos);
            }
            else {
                Draw2DItemInInventory(BlockID::Air, slotPos - 9, true);
            }
            
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseHoldingItem == false) { // when mouse is holding nothing
                if (playerItemClicking != nullptr) { // when mouse actually clicked a item 
                    mouseHoldingItem = true;
                    playerItemClicking->whereSlot = -999;
                }
            }
            else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseHoldingItem == true) { // when mouse is holding something
                if (playerItemClicking == nullptr) {// when mouse clicked a empty slot 
                    mouseHoldingItem = false;
                   
                    playerItemClicking = main_player->getItemInInventory(-999);
                    playerItemClicking->whereSlot = slotPos;
                }
                else { // when mouse actually clicked a item 
                    
                    ItemInfo* playerItemClicking2 = main_player->getItemInInventory(-999);
                    if (playerItemClicking->blockId == playerItemClicking2->blockId){
                        playerItemClicking->currentQuantity += playerItemClicking2->currentQuantity;
                        main_player->removeItemFromInventory(-999);
                        mouseHoldingItem = false;
                    }
                    else {
                        playerItemClicking->whereSlot = -999;
                        playerItemClicking2->whereSlot = slotPos;
                        mouseHoldingItem = true;
                    }
                }
            }
        }
    endthisIf:
        
        // 3D items
        BeginMode3D(hotbarCamera);
        for (const ItemInfo& item : main_player->inventory) {
            if (item.projection != Projection::_3D) continue;
            if (item.whereSlot == -999) continue; 
            BlockID blockId = item.blockId;
            if (item.whereSlot > 8) {
                DrawItem3D(blockId, blockIdDefinition[blockId].shapeTexture, atlas, item.whereSlot - 9, true, true);
            }
            else {
                DrawItem3D(blockId, blockIdDefinition[blockId].shapeTexture, atlas, item.whereSlot, true);
            }
        }
        EndMode3D();

        // 2D items
        for (const ItemInfo& item : main_player->inventory) {
            if (item.projection != Projection::_2D) continue;
            if (item.whereSlot == -999) continue;
            if (item.whereSlot > 8) {
                Draw2DItemInInventory(item.blockId, item.whereSlot - 9, true);
            }
            else {
                Draw2DItemInInventory(item.blockId, item.whereSlot, false);
            }
        }
        
        movingItemInInventory(GetMousePosition());

        // show quantity of item in every slot in inventory
        if(inventoryopened == true){
            for (ItemInfo& item : main_player->inventory)
            {
                bool isMinus999 = false;
                if (item.whereSlot == -999) isMinus999 = true;
                int morethan9 = 0;
                int quantitySlot = item.currentQuantity;
                if (quantitySlot > 9 || quantitySlot < 0) {
                    morethan9 = 1;
                    if (quantitySlot > 99 || quantitySlot < -9) {
                        morethan9 = 2;
                    }
                }
                int additionalPos = item.whereSlot;
                float additionalYPos = 0.05;
                if (item.whereSlot > 8) {
                    additionalYPos = 0.3;
                    while (additionalPos >= 9) {
                        additionalPos -= 9;
                        additionalYPos += 1.0;
                    }
                }
                Vector2 itemPos;

                if (isMinus999 == true) { // for item following mouse
                    itemPos = GetMousePosition();
                    itemPos.x += (8 * scale) + (0.6 * 1.015) + 1 - (15 * morethan9);
                    itemPos.y += ((TILE_SIZE * scale) - 20) ;
                }
                else {
                    itemPos = Vector2{
                        (float)((inventoryPos.x - (15 * morethan9)) + (7 * scale) + (slotWidth * (additionalPos + 0.6) * 1.015) + 1),
                        (float)((inventoryPos.y + (TILE_SIZE * scale) - 20) - (TILE_SIZE * scale) - (7 * scale) + (texturesGUI[2].height * scale) - (slotWidth * additionalYPos))
                    };
                }
                if (quantitySlot <= 0) {
                    DrawText(TextFormat("%d", quantitySlot), itemPos.x, itemPos.y, 12 * scale, RED);
                }
                else if (quantitySlot != 1) {
                    DrawText(TextFormat("%d", quantitySlot), itemPos.x, itemPos.y, 12 * scale, WHITE);
                }
            }
        }

    }
}