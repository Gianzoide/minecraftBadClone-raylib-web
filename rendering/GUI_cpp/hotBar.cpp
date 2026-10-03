#include <iostream>

#include <raylib.h>
#include <rlgl.h>
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
float divideInHand = WindowSizeY / 8;

// Camera usada somente para a mão do jogador (os ícones de UI agora são 2D)
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

// ============================================================
//  ÍCONES 2D (cubo isométrico) - independem da resolução
// ============================================================

// Câmera isométrica olhando de (+X, +Y, +Z), com Z para cima.
// Faces visíveis: X+ (direita), Y+ (esquerda) e topo.
// Um cubo unitário vira um hexágono de largura 2 e altura 2 (unidades projetadas).
static inline Vector2 IsoProject(const Vector3& v) {
    return { v.x - v.y, (v.x + v.y) * 0.5f - v.z };
}

// UV local (0..1) de um vértice dentro da face. Os índices seguem a ordem
// de FacesToRender: 0=X+, 1=X-, 2=Y+, 3=Y-, 4=topo, 5=base.
// Calculado pela posição do vértice, então escadas e lajes pegam só a parte
// certa da textura (metade, quarto...).
static inline Vector2 FaceLocalUV(int dir, const Vector3& v) {
    switch (dir) {
    case 0:  return { v.y,        1.0f - v.z };
    case 1:  return { 1.0f - v.y, 1.0f - v.z };
    case 2:  return { 1.0f - v.x, 1.0f - v.z };
    case 3:  return { v.x,        1.0f - v.z };
    case 4:  return { v.x,        1.0f - v.y };
    default: return { 1.0f - v.x, 1.0f - v.y };
    }
}

struct IsoFace {
    Vector2 p[4];   // posição projetada (unidades isométricas)
    Vector2 uv[4];  // coordenadas no atlas
    Color color;
    float depth;    // maior = mais perto da câmera
    int dir;
};

// Desenha o bloco usando as faces do próprio Shape (ChooseshapeToRender).
// pos = canto superior esquerdo da caixa size x size.
void DrawIsoCube(BlockID id, Vector2 pos, float size, Texture2D& theAtlas)
{
    auto& def = blockIdDefinition[id];
    Shape shape = def.shapeTexture;
    const FacesToRender& faces = ChooseshapeToRender[(int)shape];

    // fogo e flores são planos visíveis dos dois lados
    bool doubleSided = (shape == Shape::fire || shape == Shape::flower);

    static vector<IsoFace> list;
    list.clear();

    float minX = 1e9f, maxX = -1e9f, minY = 1e9f, maxY = -1e9f;

    for (int dir = 0; dir < 6; dir++) {
        // blocos sólidos: só X+, Y+ e topo aparecem
        if (!doubleSided && dir != 0 && dir != 2 && dir != 4) continue;

        Color shade = WHITE;
        if (!doubleSided) {
            if (dir == 2) shade = Color{ 200,200,200,255 }; // esquerda
            if (dir == 0) shade = Color{ 150,150,150,255 }; // direita
        }

        const AtlasTile& tile = def.texture[dir];

        for (const SquareFace& f : faces[dir]) {
            IsoFace o;
            o.color = shade;
            o.dir = dir;
            o.depth = 0;

            Vector2 luv[4];
            bool inRange = true;
            for (int i = 0; i < 4; i++) {
                const Vector3& v = f.face[i];
                luv[i] = FaceLocalUV(dir, v);
                if (luv[i].x < -0.001f || luv[i].x > 1.001f || luv[i].y < -0.001f || luv[i].y > 1.001f)
                    inRange = false;
                o.depth += v.x + v.y + v.z;
                o.p[i] = IsoProject(v);
                minX = min(minX, o.p[i].x); maxX = max(maxX, o.p[i].x);
                minY = min(minY, o.p[i].y); maxY = max(maxY, o.p[i].y);
            }
            // faces maiores que 1 bloco (ex: fogo): usa a textura inteira esticada
            if (!inRange) {
                luv[0] = { 0,1 }; luv[1] = { 1,1 }; luv[2] = { 1,0 }; luv[3] = { 0,0 };
            }
            for (int i = 0; i < 4; i++) {
                o.uv[i] = { tile.u0 + luv[i].x * (tile.u1 - tile.u0),
                            tile.v0 + luv[i].y * (tile.v1 - tile.v0) };
            }

            // garante o mesmo sentido de giro do DrawTexturePro (culling do raylib)
            float area = 0;
            for (int i = 0; i < 4; i++) {
                int j = (i + 1) % 4;
                area += o.p[i].x * o.p[j].y - o.p[j].x * o.p[i].y;
            }
            if (area >= 0) {
                swap(o.p[1], o.p[3]);
                swap(o.uv[1], o.uv[3]);
            }
            list.push_back(o);
        }
    }
    if (list.empty()) return;

    // pintor: de trás para frente
    stable_sort(list.begin(), list.end(), [](const IsoFace& a, const IsoFace& b) {
        return a.depth / 4.0f < b.depth / 4.0f;
        });

    // formas mais altas/largas que um cubo (fogo) são reduzidas e centralizadas
    float spanX = maxX - minX;
    float spanY = maxY - minY;
    float k = min(1.0f, min(2.0f / spanX, 2.0f / spanY));
    Vector2 offset = { 0, 0 };
    if (k < 1.0f) offset = { -(minX + maxX) * 0.5f, -(minY + maxY) * 0.5f };

    float h = size * 0.5f;
    Vector2 center = { pos.x + h, pos.y + h };

    rlSetTexture(theAtlas.id);
    rlBegin(RL_QUADS);
    for (const IsoFace& o : list) {
        rlColor4ub(o.color.r, o.color.g, o.color.b, o.color.a);
        for (int i = 0; i < 4; i++) {
            rlTexCoord2f(o.uv[i].x, o.uv[i].y);
            rlVertex2f(center.x + (o.p[i].x + offset.x) * k * h,
                center.y + (o.p[i].y + offset.y) * k * h);
        }
    }
    rlEnd();
    rlSetTexture(0);
}

// ============================================================
//  Ícones com supersampling (render grande -> redução suave)
// ============================================================
// Por que isso: num ícone de ~30px a textura de 16px fica espremida. Na projeção
// isométrica o topo é achatado na vertical (cada texel ocupa ~0.4px de altura),
// então com amostragem direta o topo perde informação e serrilha.
// Aqui cada bloco é desenhado numa textura grande (ICON_SUPERSAMPLE x o tamanho
// na tela) com filtro POINT, e depois reduzida com mipmap, que faz a média dos
// texels. Resultado: topo e laterais com a mesma qualidade, sem serrilhado.
constexpr int ICON_SUPERSAMPLE = 8;   // 4 = mais leve, 8 = mais liso, 16 = máximo

static RenderTexture2D iconCache[(size_t)BlockID::COUNT];
static bool iconReady[(size_t)BlockID::COUNT] = {};
static int iconRes = 0;

static int NextPow2(int v) { int p = 1; while (p < v) p <<= 1; return p; }

static void SetAtlasFilterPoint() { SetTextureFilter(atlas, TEXTURE_FILTER_POINT); }
static void RestoreAtlasFilter() {
#ifndef PLATFORM_WEB
    SetTextureFilter(atlas, TEXTURE_FILTER_ANISOTROPIC_16X);
#endif
}

void UnloadItemIcons()
{
    for (size_t i = 0; i < (size_t)BlockID::COUNT; i++) {
        if (iconReady[i]) UnloadRenderTexture(iconCache[i]);
        iconReady[i] = false;
    }
}

// Opcional (os ícones também são criados sob demanda). Mantida para compatibilidade.
void PreRenderItemIcons() {}

static void BuildIcon(BlockID id, int res)
{
    size_t i = (size_t)id;

    rlDrawRenderBatchActive();
    SetAtlasFilterPoint();

    iconCache[i] = LoadRenderTexture(res, res);
    BeginTextureMode(iconCache[i]);
    ClearBackground(BLANK);
    DrawIsoCube(id, { 0, 0 }, (float)res, atlas);
    EndTextureMode();

    RestoreAtlasFilter();

    //GenTextureMipmaps(&iconCache[i].texture);
    SetTextureFilter(iconCache[i].texture, TEXTURE_FILTER_TRILINEAR);
    iconReady[i] = true;
}

static void DrawCachedIcon(BlockID id, Vector2 pos, float size)
{
    RenderTexture2D& rt = iconCache[(size_t)id];
    Rectangle src = { 0, 0, (float)rt.texture.width, -(float)rt.texture.height }; // RT vem de cabeça para baixo
    Rectangle dst = { roundf(pos.x), roundf(pos.y), roundf(size), roundf(size) };

    // o mipmap mistura cor com transparente -> alpha pré-multiplicado evita borda escura
    BeginBlendMode(BLEND_ALPHA_PREMULTIPLY);
    DrawTexturePro(rt.texture, src, dst, { 0, 0 }, 0.0f, WHITE);
    EndBlendMode();
}

// Fogo é animado (o atlas muda todo frame), então não usa cache.
static void DrawIsoCubeLive(BlockID id, Vector2 pos, float size)
{
    rlDrawRenderBatchActive();
    SetAtlasFilterPoint();
    DrawIsoCube(id, { roundf(pos.x), roundf(pos.y) }, roundf(size), atlas);
    rlDrawRenderBatchActive();
    RestoreAtlasFilter();
}

static void DrawIsoIcon(BlockID id, Vector2 pos, float size)
{
    if (blockIdDefinition[id].shapeTexture == Shape::fire) {
        DrawIsoCubeLive(id, pos, size);
        return;
    }

    // resolução do cache acompanha o `scale` da UI (recria se ele mudar)
    int wanted = NextPow2((int)ceilf(TILE_SIZE * scale * ICON_SUPERSAMPLE));
    if (wanted > 1024) wanted = 1024;
    if (wanted != iconRes) {
        UnloadItemIcons();
        iconRes = wanted;
    }

    if (!iconReady[(size_t)id]) BuildIcon(id, iconRes);
    DrawCachedIcon(id, pos, size);
}

// Item plano (sprite 2D) do atlas
static void DrawFlatIcon(BlockID id, Vector2 pos, float size, Texture2D& theAtlas)
{
    AtlasTile tile = blockIdDefinition[id].texture[0];
    Rectangle source = {
        tile.u0 * theAtlas.width,
        tile.v0 * theAtlas.height,
        (tile.u1 - tile.u0) * theAtlas.width,
        (tile.v1 - tile.v0) * theAtlas.height
    };
    Rectangle dest = { pos.x, pos.y, size, size };
    DrawTexturePro(theAtlas, source, dest, { 0, 0 }, 0.0f, WHITE);
}

// Desenha qualquer ícone (3D -> cubo isométrico, 2D -> sprite)
void DrawItemIcon(BlockID id, Projection projection, Vector2 pos, float size)
{
    if (projection == Projection::_3D) {
        DrawIsoIcon(id, pos, size);
    }
    else {
        DrawFlatIcon(id, pos, size, atlas);
    }
}

// ============================================================
//  Posição de slot dentro do inventário aberto
// ============================================================
// additionalPos = coluna (0..8). itemsInTheMainSlot = true para as 3 fileiras de cima.
// posInWindow diferente de zero = usa essa posição direto.
void DrawItemInInventory(BlockID blockId, int additionalPos, bool itemsInTheMainSlot = false,
    Vector2 posInWindow = {}, Projection projection = Projection::_2D)
{
    Vector2 inventoryPos = {
        (GetScreenWidth() / 2.0f) - (texturesGUI[2].width * scale / 2.0f),
        (GetScreenHeight() / 2.0f) - (texturesGUI[2].height * scale / 2.0f)
    };

    int slotCount = 9;
    float slotWidth = ((texturesGUI[2].width - 16) * scale) / slotCount;

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

    DrawItemIcon(blockId, projection, itemPos, 16.0f * scale);
}

// ============================================================
//  Mão do jogador (continua em 3D)
// ============================================================
Vector3 animationMotionEnd = { 345,280,50 };
Vector3 animationMotionBackToBegining = { 367,367,60 };
Vector3 animationMotionValue = animationMotionBackToBegining;

Vector3 animationDiff = {
    (animationMotionBackToBegining.x - animationMotionEnd.x) / 8,
    (animationMotionBackToBegining.y - animationMotionEnd.y) / 8,
    (animationMotionBackToBegining.z - animationMotionEnd.z) / 8
};
bool goingBack = true;

void drawItemInHand(BlockID blockId, Shape blockShape, Texture2D& theAtlas) {
    bool Left_Clicked = false;
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) Left_Clicked = true;
    float resizeIdk = 600 / (float)WindowSizeY;
    Vector3 pos3D = { 0,0.02,-0.018 };
    pos3D.y += bruhMoment;
    pos3D.y *= resizeIdk;
    pos3D.z += (bruhZMomentZZZ * resizeIdk) - bruhZMomentZZZ;
    int index = 0;

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
    else if (goingBack == true) {
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

// ============================================================
//  Item preso no mouse
// ============================================================
bool mouseHoldingItem = false; // false quando o jogador não está segurando nada

void movingItemInInventory(Vector2 mousePos) {
    float size = 16.0f * scale;
    for (const ItemInfo& item : main_player->inventory) {
        if (item.whereSlot != -999) continue;
        // centraliza o ícone no cursor
        DrawItemIcon(item.blockId, item.projection, { mousePos.x - size / 2, mousePos.y - size / 2 }, size);
        break;
    }
}

// ============================================================
//  Texto de quantidade
// ============================================================
static int DigitsExtra(int q) {
    int morethan9 = 0;
    if (q > 9 || q < 0) {
        morethan9 = 1;
        if (q > 99 || q < -9) morethan9 = 2;
    }
    return morethan9;
}

// ============================================================
//  Hotbar + inventário
// ============================================================
void drawHotbar() {
    // Mão (única parte que ainda usa a câmera 3D) - desenhada antes da hotbar
    BeginMode3D(hotbarCamera);
    for (const ItemInfo& item : main_player->inventory)
    {
        if (item.projection != Projection::_3D) continue;
        if (item.whereSlot == main_player->hotbarSlotBeeningUsed) {
            BlockID blockId = item.blockId;
            drawItemInHand(blockId, blockIdDefinition[blockId].shapeTexture, atlas);
        }
    }
    EndMode3D();

    Vector2 hotbarPos = {
        (GetScreenWidth() / 2.0f) - (texturesGUI[0].width * scale / 2.0f),
        (GetScreenHeight() * spaceFromBottom) - (texturesGUI[0].height * scale)
    };
    DrawTextureEx(texturesGUI[0], hotbarPos, 0.0f, scale, WHITE);

    int slotCount = 9;
    float slotWidth = (texturesGUI[0].width * scale) / slotCount;

    // Slot selecionado
    Vector2 selectedPos = {
        hotbarPos.x - (1.8f * scale) + (slotWidth * main_player->hotbarSlotBeeningUsed),
        hotbarPos.y - (1.0f * scale)
    };
    DrawTextureEx(texturesGUI[1], selectedPos, 0.0f, scale, WHITE);

    float itemslotScale = 0.9f;

    // Itens da hotbar (3D e 2D no mesmo loop)
    for (const ItemInfo& item : main_player->inventory)
    {
        if (item.whereSlot == -999 || item.whereSlot > 8) continue;

        Vector2 itemPos = {
            hotbarPos.x - (1.8f * scale) + (0.99f * slotWidth * item.whereSlot) + slotWidth / 4,
            hotbarPos.y - (1.0f * scale) + slotWidth / 4
        };
        DrawItemIcon(item.blockId, item.projection, itemPos, TILE_SIZE * scale * itemslotScale);
    }

    // Quantidade dos itens na hotbar
    for (ItemInfo& item : main_player->inventory)
    {
        if (item.whereSlot == -999 || item.whereSlot > 8) continue;
        int quantitySlot = item.currentQuantity;
        int morethan9 = DigitsExtra(quantitySlot);

        Vector2 itemPos = {
            (hotbarPos.x - (15 * morethan9)) - (1.8f * scale) + (0.99f * slotWidth * (item.whereSlot + 0.5f)) + slotWidth / 4,
            (hotbarPos.y + (TILE_SIZE * scale) - 25) - (1.0f * scale) + slotWidth / 4
        };
        if (quantitySlot <= 0) {
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

        // Quadrado branco no slot onde o mouse está
        Vector2 mousePos = GetMousePosition();
        mousePos = { mousePos.x, WindowSizeY - mousePos.y };
        Vector2 InventoryMousePos = Vector2Subtract(mousePos, inventoryPos);
        InventoryMousePos = { InventoryMousePos.x - (7 * scale), InventoryMousePos.y - (7 * scale) };
        float invSlotWidth = ((inventoryIMG.width - 16) * scale) / 9;
        int maxslotX = 8;
        int maxslotY = 3;

        if (InventoryMousePos.x > 0 && InventoryMousePos.x < (inventoryIMG.width * scale) - (7 * scale * 2) &&
            InventoryMousePos.y > 0 && InventoryMousePos.y < (inventoryIMG.height * scale) - (7 * scale * 2))
        {
            int slotX = (int)(InventoryMousePos.x / invSlotWidth);
            int slotY = (int)(InventoryMousePos.y / invSlotWidth);
            if (slotY > 0) {
                slotY = (int)((InventoryMousePos.y - (4 * scale)) / invSlotWidth);
            }

            if (slotX <= maxslotX && slotY <= maxslotY) {
                int slotPos = slotY * 9 + slotX;
                ItemInfo* playerItemClicking = main_player->getItemInInventory(slotPos);

                if (slotPos <= 8) {
                    DrawItemInInventory(BlockID::Air, slotPos);
                }
                else {
                    DrawItemInInventory(BlockID::Air, slotPos - 9, true);
                }

                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseHoldingItem == false) { // mouse vazio
                    if (playerItemClicking != nullptr) { // clicou em um item
                        mouseHoldingItem = true;
                        playerItemClicking->whereSlot = -999;
                    }
                }
                else if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && mouseHoldingItem == true) { // mouse segurando algo
                    ItemInfo* heldItem = main_player->getItemInInventory(-999);
                    if (heldItem != nullptr) {
                        if (playerItemClicking == nullptr) { // clicou em slot vazio
                            mouseHoldingItem = false;
                            heldItem->whereSlot = slotPos;
                        }
                        else { // clicou em um item
                            if (playerItemClicking->blockId == heldItem->blockId) {
                                playerItemClicking->currentQuantity += heldItem->currentQuantity;
                                main_player->removeItemFromInventory(-999);
                                mouseHoldingItem = false;
                            }
                            else {
                                playerItemClicking->whereSlot = -999;
                                heldItem->whereSlot = slotPos;
                                mouseHoldingItem = true;
                            }
                        }
                    }
                }
            }
        }

        // Itens do inventário (3D e 2D no mesmo loop)
        for (const ItemInfo& item : main_player->inventory) {
            if (item.whereSlot == -999) continue;
            if (item.whereSlot > 8) {
                DrawItemInInventory(item.blockId, item.whereSlot - 9, true, {}, item.projection);
            }
            else {
                DrawItemInInventory(item.blockId, item.whereSlot, false, {}, item.projection);
            }
        }

        // Item preso no mouse
        movingItemInInventory(GetMousePosition());

        // Quantidade dos itens no inventário
        for (ItemInfo& item : main_player->inventory)
        {
            bool isMinus999 = (item.whereSlot == -999);
            int quantitySlot = item.currentQuantity;
            int morethan9 = DigitsExtra(quantitySlot);

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
            if (isMinus999 == true) { // item seguindo o mouse
                itemPos = GetMousePosition();
                itemPos.x += (8 * scale) + (0.6 * 1.015) + 1 - (15 * morethan9);
                itemPos.y += ((TILE_SIZE * scale) - 20);
            }
            else {
                itemPos = Vector2{
                    (float)((inventoryPos.x - (15 * morethan9)) + (7 * scale) + (invSlotWidth * (additionalPos + 0.6) * 1.015) + 1),
                    (float)((inventoryPos.y + (TILE_SIZE * scale) - 20) - (TILE_SIZE * scale) - (7 * scale) + (texturesGUI[2].height * scale) - (invSlotWidth * additionalYPos))
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