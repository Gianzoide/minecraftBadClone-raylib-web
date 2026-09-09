
#include "blocks.h"

FacesToRender squareBluePrintFacesToRender = {
    vector{ SquareFace{{Vector3{1,0,0}, Vector3{1,1,0}, Vector3{1,1,1}, Vector3{1,0,1}}, square} },// XPositive
    vector{ SquareFace{{Vector3{0,1,0}, Vector3{0,0,0}, Vector3{0,0,1}, Vector3{0,1,1}}, square} },// XNegative
    vector{ SquareFace{{Vector3{1,1,0}, Vector3{0,1,0}, Vector3{0,1,1}, Vector3{1,1,1}}, square} },// YPositive
    vector{ SquareFace{{Vector3{0,0,0}, Vector3{1,0,0}, Vector3{1,0,1}, Vector3{0,0,1}}, square} },// YNegative
    vector{ SquareFace{{Vector3{0,0,1}, Vector3{1,0,1}, Vector3{1,1,1}, Vector3{0,1,1}}, square} },// top
    vector{ SquareFace{{Vector3{1,0,0}, Vector3{0,0,0}, Vector3{0,1,0}, Vector3{1,1,0}}, square} },// bottom
};
FacesToRender stairBluePrintFacesToRender = {
    vector{//XPositive
        SquareFace{{Vector3{1.0,0.0,0.0}, Vector3{1.0,1.0,0.0}, Vector3{1.0,1.0,0.5}, Vector3{1.0,0.0,0.5}}, BottomHalfSquare},
        SquareFace{{Vector3{1.0,0.0,0.5}, Vector3{1.0,0.5,0.5}, Vector3{1.0,0.5,1.0}, Vector3{1.0,0.0,1.0}}, oneForthTopLeftSquare}
    },
    vector{//XNegative
        SquareFace{{Vector3{0.0,1.0,0.0}, Vector3{0.0,0.0,0.0}, Vector3{0.0,0.0,0.5}, Vector3{0.0,1.0,0.5}}, BottomHalfSquare},
        SquareFace{{Vector3{0.0,0.5,0.5}, Vector3{0.0,0.0,0.5}, Vector3{0.0,0.0,1.0}, Vector3{0.0,0.5,1.0}}, oneForthTopRigthSquare}
    },
    vector{//YPositive
        SquareFace{{Vector3{1.0,1.0,0.0}, Vector3{0.0,1.0,0.0}, Vector3{0.0,1.0,0.5}, Vector3{1.0,1.0,0.5}}, BottomHalfSquare},
        SquareFace{{Vector3{1.0,0.5,0.5}, Vector3{0.0,0.5,0.5}, Vector3{0.0,0.5,1.0}, Vector3{1.0,0.5,1.0}}, alwaysRender},
    },
    vector{//YNegative
        SquareFace{{Vector3{0.0,0.0,0.0}, Vector3{1.0,0.0,0.0}, Vector3{1.0,0.0,1.0}, Vector3{0.0,0.0,1.0}}, square}
    },
    vector{//top
        SquareFace{{Vector3{1.0,0.5,1.0}, Vector3{0.0,0.5,1.0}, Vector3{0.0,0.0,1.0}, Vector3{1.0,0.0,1.0}}, BottomHalfSquare},
        SquareFace{{Vector3{1.0,1.0,0.5}, Vector3{0.0,1.0,0.5}, Vector3{0.0,0.5,0.5}, Vector3{1.0,0.5,0.5}}, alwaysRender},
    },
    vector{//bottom
        SquareFace{{Vector3{1.0,1.0,0.0}, Vector3{1.0,0.0,0.0}, Vector3{0.0,0.0,0.0}, Vector3{0.0,1.0,0.0}}, square}
    },
};

FacesToRender halfBlockBluePrintFacesToRender = {
    vector{ SquareFace{{Vector3{1,0,0}, Vector3{1,1,0}, Vector3{1,1,0.5}, Vector3{1,0,0.5}}, BottomHalfSquare} },// XPositive
    vector{ SquareFace{{Vector3{0,1,0}, Vector3{0,0,0}, Vector3{0,0,0.5}, Vector3{0,1,0.5}}, BottomHalfSquare} },// XNegative
    vector{ SquareFace{{Vector3{1,1,0}, Vector3{0,1,0}, Vector3{0,1,0.5}, Vector3{1,1,0.5}}, BottomHalfSquare} },// YPositive
    vector{ SquareFace{{Vector3{0,0,0}, Vector3{1,0,0}, Vector3{1,0,0.5}, Vector3{0,0,0.5}}, BottomHalfSquare} },// YNegative
    vector{ SquareFace{{Vector3{0,0,0.5}, Vector3{1,0,0.5}, Vector3{1,1,0.5}, Vector3{0,1,0.5}}, alwaysRender} },// top
    vector{ SquareFace{{Vector3{1,0,0}, Vector3{0,0,0}, Vector3{0,1,0}, Vector3{1,1,0}}, square} },              // bottom
};
float spacelilbro = 0.0001;
FacesToRender fireBluePrintFacesToRender = {
    vector{ SquareFace{{Vector3{1,0,0}, Vector3{1,1,0}, Vector3{1,1,1.3125}, Vector3{1,0,1.3125}}, alwaysRender}, SquareFace{{Vector3{spacelilbro,0,0}, Vector3{spacelilbro,1,0}, Vector3{spacelilbro,1,1.3125}, Vector3{spacelilbro,0,1.3125}}, alwaysRender},
    SquareFace{{Vector3{0.8,0,0}, Vector3{0.8,1,0}, Vector3{0.2,1,1.3125}, Vector3{0.2,0,1.3125}}, alwaysRender}/*diagonal*/},// XPositive
    vector{ SquareFace{{Vector3{0,1,0}, Vector3{0,0,0}, Vector3{0,0,1.3125}, Vector3{0,1,1.3125}}, alwaysRender}, SquareFace{{Vector3{1 - spacelilbro,1,0}, Vector3{1 - spacelilbro,0,0}, Vector3{1 - spacelilbro,0,1.3125}, Vector3{1 - spacelilbro,1,1.3125}}, alwaysRender},
    SquareFace{{Vector3{0.2,1,0}, Vector3{0.2,0,0}, Vector3{0.8,0,1.3125}, Vector3{0.8,1,1.3125}}, alwaysRender}/*diagonal*/},// XNegative
    vector{ SquareFace{{Vector3{1,1,0}, Vector3{0,1,0}, Vector3{0,1,1.3125}, Vector3{1,1,1.3125}}, alwaysRender}, SquareFace{{Vector3{1,spacelilbro,0}, Vector3{0,spacelilbro,0}, Vector3{0,spacelilbro,1.3125}, Vector3{1,spacelilbro,1.3125}}, alwaysRender},
    SquareFace{{Vector3{1,0.8,0}, Vector3{0,0.8,0}, Vector3{0,0.2,1.3125}, Vector3{1,0.2,1.3125}}, alwaysRender}/*diagonal*/},// YPositive
    vector{ SquareFace{{Vector3{0,0,0}, Vector3{1,0,0}, Vector3{1,0,1.3125}, Vector3{0,0,1.3125}}, alwaysRender}, SquareFace{{Vector3{0,1 - spacelilbro,0}, Vector3{1,1 - spacelilbro,0}, Vector3{1,1 - spacelilbro,1.3125}, Vector3{0,1 - spacelilbro,1.3125}}, alwaysRender},
    SquareFace{{Vector3{0,0.2,0}, Vector3{1,0.2,0}, Vector3{1,0.8,1.3125}, Vector3{0,0.8,1.3125}}, alwaysRender}},// YNegative
    { },// top
    { },// bottom
};
FacesToRender flowersBluePrintFacesToRender = {
    vector{ SquareFace{{Vector3{0,0,0}, Vector3{1,1,0}, Vector3{1,1,1}, Vector3{0,0,1}}, alwaysRender} },// XPositive
    vector{ SquareFace{{Vector3{1,1,0}, Vector3{0,0,0}, Vector3{0,0,1}, Vector3{1,1,1}}, alwaysRender} },// XNegative
    vector{ SquareFace{{Vector3{1,0,0}, Vector3{0,1,0}, Vector3{0,1,1}, Vector3{1,0,1}}, alwaysRender} },// YPositive
    vector{ SquareFace{{Vector3{0,1,0}, Vector3{1,0,0}, Vector3{1,0,1}, Vector3{0,1,1}}, alwaysRender} },// YNegative
    { },// top
    { },// bottom
};

vector<FacesToRender> ChooseshapeToRender = {
    squareBluePrintFacesToRender, //index 0
    halfBlockBluePrintFacesToRender,
    stairBluePrintFacesToRender,
    fireBluePrintFacesToRender,
    flowersBluePrintFacesToRender,
};

EnumArray<collisionEnum, collisionBoxes, (size_t)collisionEnum::COUNT> collision = { 
    collisionBoxes{}, // nothing
    collisionBoxes{ collisionBox{1.0, 1.0, 1.0} }, // block
    collisionBoxes{ collisionBox{1.0, 1.0, 0.5} }, // halfBlock
    collisionBoxes{ collisionBox{1.0, 1.0, 0.5}, collisionBox{1.0, 0.5, 1.0} }, // stair
    collisionBoxes{ collisionBox{{0.35, 0.35, 0.70}, {0.325, 0.325, 0.0 }}  }, // flower
};

texturesAtlas textureAtlas;
array<blockyTexture, (size_t)blockTextureEnum::COUNT> blockTexture;
array<BlockDefinition, (size_t)BlockID::COUNT> blockIdDefinition;

//vector<Texture2D> textures;
Texture2D atlas;
Texture2D breakingAnimation;
Shader cutoutShader;

void apply100TransparentShaders(Image& image, Texture2D& source2DTexture) {
    
    ImageFormat(&image, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    Color* pixels = (Color*)image.data;
    int count = image.width * image.height;
    for (int i = 0; i < count; i++)
    {
        if (pixels[i].a < 255)
            pixels[i].a = 0;
    }
    source2DTexture = LoadTextureFromImage(image);
    UnloadImage(image);
}

void getBlocksTextures()
{
    //setting the block faces textures
    texturesAtlas& TA = textureAtlas;
    blockTexture[(size_t)blockTextureEnum::stone] = {TA.stonePNG, TA.stonePNG, TA.stonePNG, TA.stonePNG, TA.stonePNG, TA.stonePNG};
    blockTexture[(size_t)blockTextureEnum::cobble_stone] = {TA.cobblestonePNG, TA.cobblestonePNG, TA.cobblestonePNG, TA.cobblestonePNG, TA.cobblestonePNG, TA.cobblestonePNG };
    blockTexture[(size_t)blockTextureEnum::dirt] = {TA.dirtPNG, TA.dirtPNG, TA.dirtPNG, TA.dirtPNG, TA.dirtPNG, TA.dirtPNG };
    blockTexture[(size_t)blockTextureEnum::grass_block] = {TA.grass_block_sidePNG, TA.grass_block_sidePNG, TA.grass_block_sidePNG, TA.grass_block_sidePNG, TA.grass_block_topPNG, TA.dirtPNG };
    blockTexture[(size_t)blockTextureEnum::glass] = {TA.glassPNG, TA.glassPNG, TA.glassPNG, TA.glassPNG, TA.glassPNG, TA.glassPNG };
    blockTexture[(size_t)blockTextureEnum::fire] = {TA.fireGIF, TA.fireGIF, TA.fireGIF, TA.fireGIF, TA.fireGIF, TA.fireGIF };
    blockTexture[(size_t)blockTextureEnum::flower_allium] = {TA.flower_alliumPNG, TA.flower_alliumPNG, TA.flower_alliumPNG, TA.flower_alliumPNG, TA.flower_alliumPNG, TA.flower_alliumPNG };
    blockTexture[(size_t)blockTextureEnum::flower_blue_orchid] = {TA.flower_blue_orchidPNG, TA.flower_blue_orchidPNG, TA.flower_blue_orchidPNG, TA.flower_blue_orchidPNG, TA.flower_blue_orchidPNG, TA.flower_blue_orchidPNG };
    blockTexture[(size_t)blockTextureEnum::flower_dandelion] = {TA.flower_dandelionPNG, TA.flower_dandelionPNG, TA.flower_dandelionPNG, TA.flower_dandelionPNG, TA.flower_dandelionPNG, TA.flower_dandelionPNG };
    blockTexture[(size_t)blockTextureEnum::flower_houstonia] = {TA.flower_houstoniaPNG, TA.flower_houstoniaPNG, TA.flower_houstoniaPNG, TA.flower_houstoniaPNG, TA.flower_houstoniaPNG, TA.flower_houstoniaPNG };
    blockTexture[(size_t)blockTextureEnum::flower_oxeye_daisy] = {TA.flower_oxeye_daisyPNG, TA.flower_oxeye_daisyPNG, TA.flower_oxeye_daisyPNG, TA.flower_oxeye_daisyPNG, TA.flower_oxeye_daisyPNG, TA.flower_oxeye_daisyPNG };
    blockTexture[(size_t)blockTextureEnum::flower_paeonia] = {TA.flower_paeoniaPNG, TA.flower_paeoniaPNG, TA.flower_paeoniaPNG, TA.flower_paeoniaPNG, TA.flower_paeoniaPNG, TA.flower_paeoniaPNG };
    blockTexture[(size_t)blockTextureEnum::flower_tulip_white] = {TA.flower_tulip_whitePNG, TA.flower_tulip_whitePNG, TA.flower_tulip_whitePNG, TA.flower_tulip_whitePNG, TA.flower_tulip_whitePNG, TA.flower_tulip_whitePNG };
    blockTexture[(size_t)blockTextureEnum::oak_log] = {TA.oak_logPNG, TA.oak_logPNG, TA.oak_logPNG, TA.oak_logPNG, TA.oak_log_topPNG, TA.oak_log_topPNG };
    blockTexture[(size_t)blockTextureEnum::oak_leaves] = {TA.oak_leavesPNG, TA.oak_leavesPNG, TA.oak_leavesPNG, TA.oak_leavesPNG, TA.oak_leavesPNG, TA.oak_leavesPNG };
    blockTexture[(size_t)blockTextureEnum::gravel] = {TA.gravelPNG, TA.gravelPNG, TA.gravelPNG, TA.gravelPNG, TA.gravelPNG, TA.gravelPNG };
    blockTexture[(size_t)blockTextureEnum::iron_ore] = {TA.iron_orePNG, TA.iron_orePNG, TA.iron_orePNG, TA.iron_orePNG, TA.iron_orePNG, TA.iron_orePNG };
    blockTexture[(size_t)blockTextureEnum::coal_ore] = {TA.coal_orePNG, TA.coal_orePNG, TA.coal_orePNG, TA.coal_orePNG, TA.coal_orePNG, TA.coal_orePNG };

    blockIdDefinition[(size_t)BlockID::Air] = {};
    blockIdDefinition[(size_t)BlockID::Stone_block] = {
        blockTexture[(size_t)blockTextureEnum::stone], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::XYZFixed,tranparencyType::justOpaque
    };
    blockIdDefinition[(size_t)BlockID::CobbleStore_block] = { 
        blockTexture[(size_t)blockTextureEnum::cobble_stone], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::justOpaque 
    };
    blockIdDefinition[(size_t)BlockID::Dirt_block] = {
        blockTexture[(size_t)blockTextureEnum::dirt], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::ZFixed_XYPreDetermed, tranparencyType::justOpaque
    };
    blockIdDefinition[(size_t)BlockID::Grass_block] = {
        blockTexture[(size_t)blockTextureEnum::grass_block], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::ZFixed_XYPreDetermed, tranparencyType::justOpaque
    };
    blockIdDefinition[(size_t)BlockID::Glass_block] = {
        blockTexture[(size_t)blockTextureEnum::glass], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, true,
        determinationRotationBlock::XYZFixed,tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::oak_log] = {
        blockTexture[(size_t)blockTextureEnum::oak_log], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::ZFixed, tranparencyType::justOpaque
    };
    blockIdDefinition[(size_t)BlockID::oak_leaves] = {
        blockTexture[(size_t)blockTextureEnum::oak_leaves], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, true,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::gravel] = {
        blockTexture[(size_t)blockTextureEnum::gravel], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::ZFixed_XYPreDetermed, tranparencyType::justOpaque
    };

    //ores
    blockIdDefinition[(size_t)BlockID::iron_ore] = {
        blockTexture[(size_t)blockTextureEnum::iron_ore], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::justOpaque
    };
    blockIdDefinition[(size_t)BlockID::coal_ore] = {
        blockTexture[(size_t)blockTextureEnum::coal_ore], collision[collisionEnum::cube],
        Shape::cube, collision[collisionEnum::cube], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::justOpaque
    };

    // diferent blocks
    blockIdDefinition[(size_t)BlockID::fire_block] = {
        blockTexture[(size_t)blockTextureEnum::fire], collision[collisionEnum::nothing],
        Shape::fire, collision[collisionEnum::halfBlock], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::flower_allium] = {
        blockTexture[(size_t)blockTextureEnum::flower_allium], collision[collisionEnum::nothing],
        Shape::flower, collision[collisionEnum::flower], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::flower_blue_orchid] = {
        blockTexture[(size_t)blockTextureEnum::flower_blue_orchid], collision[collisionEnum::nothing],
        Shape::flower, collision[collisionEnum::flower], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::flower_dandelion] = {
        blockTexture[(size_t)blockTextureEnum::flower_dandelion], collision[collisionEnum::nothing],
        Shape::flower, collision[collisionEnum::flower], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::flower_houstonia] = {
        blockTexture[(size_t)blockTextureEnum::flower_houstonia], collision[collisionEnum::nothing],
        Shape::flower, collision[collisionEnum::flower], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::flower_oxeye_daisy] = {
        blockTexture[(size_t)blockTextureEnum::flower_oxeye_daisy], collision[collisionEnum::nothing],
        Shape::flower, collision[collisionEnum::flower], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::flower_paeonia] = {
        blockTexture[(size_t)blockTextureEnum::flower_paeonia], collision[collisionEnum::nothing],
        Shape::flower, collision[collisionEnum::flower], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };
    blockIdDefinition[(size_t)BlockID::flower_tulip_white] = {
        blockTexture[(size_t)blockTextureEnum::flower_tulip_white], collision[collisionEnum::nothing],
        Shape::flower, collision[collisionEnum::flower], 1000, 0, false,
        determinationRotationBlock::XYZFixed, tranparencyType::transparent
    };

    Image breakingAnimationImage = LoadImage("./textures/destroy_stage.png");
    apply100TransparentShaders(breakingAnimationImage, breakingAnimation);

    Image atlasImage = LoadImage("textures/atlas_0.png");
    apply100TransparentShaders(atlasImage, atlas);


#ifndef PLATFORM_WEB
    SetTextureFilter(atlas, TEXTURE_FILTER_ANISOTROPIC_16X);
#endif

    cutoutShader = LoadShader(
        0,
        "./shaders/cutout.fs"
    );

    //blocksTextures
    //textures.push_back(LoadTexture("textures/atlas_0.png"));
    //textures.push_back(LoadTexture("textures/dirt.png"));
    //textures.push_back(LoadTexture("textures/stone.png"));
    //textures.push_back(LoadTexture("textures/cobblestone.png"));
    //textures.push_back(LoadTexture("textures/grass_block_side.png"));
    //textures.push_back(LoadTexture("textures/grass_block_top.png"));
    //textures.push_back(LoadTexture("./textures/glass.png")); //index 5
}

static uint32_t hash3D(int x, int y, int z)
{
    uint32_t h = 0;

    h ^= (uint32_t)x * 0x85ebca6b;
    h ^= (uint32_t)y * 0xc2b2ae35;
    h ^= (uint32_t)z * 0x27d4eb2f;

    // MurmurHash3 finalizer
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;

    return h;
}

Rotation getBlockDetermedRotation(determinationRotationBlock determed, int3D& blockCoord, int3D wantedRotation) {
    uint32_t h = hash3D(blockCoord.x, blockCoord.y, blockCoord.z);

    int3D preDetermed;
    preDetermed.x = static_cast<int>(h & 3u);
    preDetermed.y = static_cast<int>((h >> 2) & 3u);
    preDetermed.z = static_cast<int>((h >> 4) & 3u);

    switch (determed)
    {
    case determinationRotationBlock::notPreDetermed:
        return makeRotation(wantedRotation.x, wantedRotation.y, wantedRotation.z);
    case determinationRotationBlock::XYZFixed:
        return 0;
    case determinationRotationBlock::ZFixed:
        return makeRotation(wantedRotation.x, wantedRotation.y, preDetermed.z);
    case determinationRotationBlock::ZFixed_XYPreDetermed:
        return makeRotation(0, 0, preDetermed.z);
    default:
        return 0;
    }
}

void copyPixels(Image& image,
    int srcX, int srcY,
    int dstX, int dstY,
    int width, int height)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            Color pixel = GetImageColor(image, srcX + x, srcY + y);

            ImageDrawPixel(
                &image,
                dstX + x,
                dstY + y,
                pixel
            );
        }
    }
}

int totalFireSprites = 32;
int fireAnimation = 0;
bool bruhFire = false;
Image atlasImage = LoadImage("textures/atlas_0.png");
void atlasAnimation() {
    if (bruhFire) {
        copyPixels(
            atlasImage,
            0, (1 + fireAnimation) * TILE_SIZE,       
            2 * TILE_SIZE, 1 * TILE_SIZE,
            TILE_SIZE, TILE_SIZE
        );
    }
    else {
        copyPixels(
            atlasImage,
            1 * TILE_SIZE, (1 + fireAnimation) * TILE_SIZE,
            2 * TILE_SIZE, 1 * TILE_SIZE,
            TILE_SIZE, TILE_SIZE
        );
    }
    UpdateTexture(atlas, atlasImage.data);
    fireAnimation++;
    if (fireAnimation == totalFireSprites) {
        fireAnimation = 0;
        bruhFire = !bruhFire;
    }
}