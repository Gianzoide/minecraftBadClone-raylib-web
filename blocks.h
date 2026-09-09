
#pragma once
#include "chunk_cpp/chunk.h"
#include <vector>
#include <array>

template <typename E>
constexpr size_t toIndex(E e) {
    return static_cast<size_t>(e);
}

template <typename Enum, typename T, size_t Size>
struct EnumArray {
    std::array<T, Size> data;

    T& operator[](Enum e) {
        return data[toIndex(e)];
    }

    const T& operator[](Enum e) const {
        return data[toIndex(e)];
    }
};

constexpr AtlasTile MakeTile(int x, int y)
{
    return {
        x * UV_SIZE,
        y * UV_SIZE,
        (x + 1) * UV_SIZE,
        (y + 1) * UV_SIZE
    };
}

enum class blockTextureEnum{
    stone,
    cobble_stone,
    dirt,
    grass_block,
    glass,
    fire,
    flower_allium,
    flower_blue_orchid,
    flower_dandelion,
    flower_houstonia,
    flower_oxeye_daisy,
    flower_paeonia,
    flower_tulip_white,
    oak_log,
    oak_leaves,
    gravel,
    iron_ore,
    coal_ore,
    COUNT
};

struct texturesAtlas {
    AtlasTile cobblestonePNG = MakeTile(0, 0);
    AtlasTile dirtPNG = MakeTile(1, 0);
    AtlasTile glassPNG = MakeTile(2, 0);
    AtlasTile grass_block_sidePNG = MakeTile(3, 0);
    AtlasTile grass_block_topPNG = MakeTile(4, 0);
    AtlasTile stonePNG = MakeTile(5, 0);
    AtlasTile fireGIF = MakeTile(2, 1);
    AtlasTile flower_alliumPNG = MakeTile(2, 2);
    AtlasTile flower_blue_orchidPNG = MakeTile(2, 3);
    AtlasTile flower_dandelionPNG = MakeTile(2, 4);
    AtlasTile flower_houstoniaPNG = MakeTile(2, 5);
    AtlasTile flower_oxeye_daisyPNG = MakeTile(3, 2);
    AtlasTile flower_paeoniaPNG = MakeTile(3, 3);
    AtlasTile flower_tulip_whitePNG = MakeTile(3, 4);
    AtlasTile oak_logPNG = MakeTile(6, 0);
    AtlasTile oak_log_topPNG = MakeTile(7, 0);
    AtlasTile oak_leavesPNG = MakeTile(8, 0);
    AtlasTile gravelPNG = MakeTile(9, 0);
    AtlasTile iron_orePNG = MakeTile(10, 0);
    AtlasTile coal_orePNG = MakeTile(11, 0);
};

extern texturesAtlas textureAtlas;
extern Texture2D breakingAnimation;

extern EnumArray<collisionEnum, collisionBoxes, (size_t)collisionEnum::COUNT> collision;


extern array<blockyTexture, (size_t)blockTextureEnum::COUNT> blockTexture;

extern void getBlocksTextures();

//extern vector<Texture2D> textures;



extern Texture2D atlas;

enum shapeFace {
    alwaysRender,
    square,
    BottomHalfSquare,
    TopHalfSquare,
    RightHalfSquare,
    leftHalfSquare,
    oneForthTopRigthSquare,
    oneForthTopLeftSquare,
    oneForthBottomRigthSquare,
    oneForthBottomLeftSquare,
};

struct SquareFace {
    array <Vector3, 4> face;
    shapeFace shape = alwaysRender;
};

//struct FacesToRender {
//    vector <SquareFace> top = {};
//    vector <SquareFace> bottom = {};
//    vector <SquareFace> YPositive = {};
//    vector <SquareFace> YNegative = {};
//    vector <SquareFace> XPositive = {};
//    vector <SquareFace> XNegative = {};
//};

using FacesToRender = array<vector <SquareFace>, 6>;


extern vector<FacesToRender> ChooseshapeToRender;

Rotation getBlockDetermedRotation(determinationRotationBlock determed, int3D& blockCoord, int3D wantedRotation = {});

extern Shader cutoutShader;

void atlasAnimation();