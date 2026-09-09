
#include "main.h"
using namespace std;

#include <list>
#include <memory>
#include <string>

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>
#endif


//void SetWindowSize(int width, int height) {
//
//}

#ifdef PLATFORM_WEB

double width, height;

//emscripten_get_element_css_size("#canvas", &width, &height);

//SetWindowSize((int)width, (int)height);

#endif

bool started = true;
static float atlasAnimationTimer = 0.0f;

void UpdateDrawFrame()
{
    #ifdef PLATFORM_WEB

        double width, height;

        emscripten_get_element_css_size("#canvas", &width, &height);
        WindowSizeX = (int)width;
        WindowSizeY = (int)height;
        SetWindowSize(WindowSizeX, WindowSizeY);

    #endif
    Vector3 playerPos = main_player->positionTimesChunk;
    if (!threading123) {
        CenterCoords = getCoordChunk_By_positionTimesChunk(playerPos);
    }

    BeginDrawing();

    ClearBackground(RAYWHITE);

    if (!started)
    {
        DrawText("Clique para jogar", 100, 100, 40, WHITE);

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            DisableCursor();
            started = true;
        }
    }
    else
    {
        BeginMode3D(camera);
        updateCamera();

        chunk();

        if (inventoryopened == false) {
            updateForwardAndOthers();
        }

        updateMovement(camera, main_player);

        if (inventoryopened == false) {
            hoveringBlock(*main_player);
        }

        updateParticles();

        renderBlocks();
        EndMode3D();

        GUI_Render();

        if (IsKeyPressed(KEY_F4)) {
            switch (main_player->gamemode)
            {
            case PlayerGamemode::creative: main_player->gamemode = survivel; break;
            case PlayerGamemode::survivel: main_player->gamemode = spectator; break;
            case PlayerGamemode::spectator: main_player->gamemode = creative; break;
            }
        }
        if (IsKeyDown(KEY_F5)) {
            if (IsKeyPressed(KEY_MINUS)) {
                if (renderDistanceDiameterHalfed != 0) {
                    changeRenderDistanceRadius(renderDistanceDiameterHalfed - 1);
                }
            }
            else if(IsKeyPressed(KEY_EQUAL)){
                changeRenderDistanceRadius(renderDistanceDiameterHalfed + 1);
            }
        }

        atlasAnimationTimer += GetFrameTime();

        if (atlasAnimationTimer >= 1.0f / 20.0f)
        {
            atlasAnimation();
            atlasAnimationTimer -= 1.0f / 20.0f;
        }
    }

    EndDrawing();
}

int main() {
    //SetTraceLogLevel(LOG_DEBUG);
    InitWindow(WindowSizeX, WindowSizeY, "Cave Game");
    SetTargetFPS(60);
    changeRenderDistanceRadius(3);  


    //rlEnableBackfaceCulling();
    rlDisableBackfaceCulling();

    createCamera();
    //textures
    getBlocksTextures();
    getFromFolderGUI_Textures();
    initRendering();
    atlasAnimation();
    buildDirectionLookup();
    setNoise();
    entities.push_back(make_unique<Entity>(
        Vector3{ (float)0.0 * chunkXlength, (float)0 * chunkYlength, (float)0.8 * chunkZlength },// position
        collisionBox{ /*size*/{0.6,0.6,1.8}, /*start coords*/{0.2,0.2,0}},    // collision box
        0,                                                                    // skin
        5.0f,                                                                 // reach
        Vector3{ 0.5f, 0.5f, 1.5f }                                           // camera vision
    ));
    main_player = entities.front().get();

    /*
    struct ItemInfo {
    int whereSlot = NULL;
    int currentQuantity = 0;
    int maxQuantity = 64;
    string nameOG = "";
    string currentName = "";
    blockWhenPlacing blockWhenPlacing = {};
    */
    main_player->inventory.push_back(ItemInfo{
        0,
        2,
        64, 
        "nig",
        "nig",
        BlockID::Grass_block
    });
    main_player->inventory.push_back(ItemInfo{
        1,
        99,
        64,
        "nig",
        "nig",
        BlockID::CobbleStore_block
    });
    main_player->inventory.push_back(ItemInfo{
        2,
        1,
        64,
        "nig",
        "nig",
       BlockID::Dirt_block
    });
    main_player->inventory.push_back(ItemInfo{
        3,
        1,
        64,
        "nig",
        "nig",
        BlockID::Stone_block
    });
    main_player->inventory.push_back(ItemInfo{4,1,64,"nig","nig",BlockID::Glass_block});
    main_player->inventory.push_back(ItemInfo{5,1,64,"nig","nig",BlockID::fire_block});
    //main_player->inventoryHotbar.push_back(ItemInfo{
    //    8,
    //    0,
    //    64,
    //    "nig",
    //    "nig",
    //    blockWhenPlacing{shape::stair, blockTexture[blockTextureEnum::grass_block], collision[collisionEnum::stair]}
    //    });

    //main_player->inventory.push_back(ItemInfo{ 9 + 0,  33, 64, "nig", "nig", BlockID::Glass_block , Projection::_2D});
    //main_player->inventory.push_back(ItemInfo{ 9 + 1,  3, 64, "nig", "nig", BlockID::Dirt_block, Projection::_2D });

    main_player->inventory.push_back(ItemInfo{ 9 + 0,  3, 64, "nig", "nig", BlockID::Glass_block});
    main_player->inventory.push_back(ItemInfo{ 9 + 1,  3, 64, "nig", "nig", BlockID::oak_log });
    main_player->inventory.push_back(ItemInfo{ 9 + 2,  3, 64, "nig", "nig", BlockID::oak_leaves });
    main_player->inventory.push_back(ItemInfo{ 9 + 3,  3, 64, "nig", "nig", BlockID::gravel });
    main_player->inventory.push_back(ItemInfo{ 9 + 4,  3, 64, "nig", "nig", BlockID::iron_ore });
    main_player->inventory.push_back(ItemInfo{ 9 + 5,  3, 64, "nig", "nig", BlockID::coal_ore });

    main_player->inventory.push_back(ItemInfo{ 18 + 0,  3, 64, "nig", "nig", BlockID::flower_allium});
    main_player->inventory.push_back(ItemInfo{ 18 + 1,  3, 64, "nig", "nig", BlockID::flower_blue_orchid});
    main_player->inventory.push_back(ItemInfo{ 18 + 2,  3, 64, "nig", "nig", BlockID::flower_dandelion});
    main_player->inventory.push_back(ItemInfo{ 18 + 3,  3, 64, "nig", "nig", BlockID::flower_houstonia });
    main_player->inventory.push_back(ItemInfo{ 18 + 4,  3, 64, "nig", "nig", BlockID::flower_oxeye_daisy });
    main_player->inventory.push_back(ItemInfo{ 18 + 5,  3, 64, "nig", "nig", BlockID::flower_paeonia });
    main_player->inventory.push_back(ItemInfo{ 18 + 6,  3, 64, "nig", "nig", BlockID::flower_tulip_white });

                    
    #ifdef PLATFORM_WEB

        emscripten_set_main_loop(UpdateDrawFrame, 0, 1);

    #else

        while (!WindowShouldClose())
        {
            UpdateDrawFrame();
            //cout << endl << main_player->positionTimesChunk.z;
        }

    #endif

    //for (auto& texture : textures) {
    //    UnloadTexture(texture);
    //    cout << endl << "texture id:" << texture.id << endl;
    //}

    CloseWindow();
    return 0;
}