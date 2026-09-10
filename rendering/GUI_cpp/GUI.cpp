
#include <iostream>
#include "GUI.h"
#include "hotBar.h"
#include <raylib.h>

#include "../../Entity_cpp/Entity.h"

#include <vector>

using namespace std;

vector<Texture2D> texturesGUI;

void getFromFolderGUI_Textures() {

	texturesGUI.push_back(LoadTexture("./textures/GUI/hotbar.png"));
	texturesGUI.push_back(LoadTexture("./textures/GUI/hotbar_selection.png")); 
	texturesGUI.push_back(LoadTexture("./textures/GUI/inventory.png")); 

}
int WindowSizeX = 1000;
int WindowSizeY = 600;
bool inventoryopened = false;
bool f3Activated = true;


void GUI_Render() {
	if (inventoryopened == true) {
		if (IsKeyPressed(KEY_E)) {
			inventoryopened = false;
			HideCursor();
			DisableCursor();
		}
	}
	else {
		if (IsKeyPressed(KEY_E)) {
			inventoryopened = true;
			ShowCursor();
			EnableCursor();
		}
	}

	DrawCrosshair();
	drawHotbar();
	if (IsKeyPressed(KEY_F3)) {
		f3Activated = !f3Activated;
	}
	if (f3Activated == true) {
		DrawText(TextFormat("x: %.2f", main_player->positionTimesChunk.x), 10, 10, 20, BLACK);
		DrawText(TextFormat("y: %.2f", main_player->positionTimesChunk.y), 10, 30, 20, BLACK);
		DrawText(TextFormat("z: %.2f", main_player->positionTimesChunk.z), 10, 50, 20, BLACK);
		DrawText(TextFormat("isGrounded: %s", main_player->isGrounded ? "true" : "false"), 10, 70, 20, BLACK);
		string gamemodeString = "";
		switch (main_player->gamemode)
		{
		case PlayerGamemode::survivel:
			gamemodeString = "survivel"; break;
		case PlayerGamemode::creative:
			gamemodeString = "creative"; break;
		case PlayerGamemode::spectator:
			gamemodeString = "spectator"; break;
		}
		DrawText(("gamemode: " + gamemodeString).c_str(), 10, 90, 20, BLACK);
		DrawText(TextFormat("renderDistance: %i", renderDistanceDiameterHalfed), 10, 110, 20, BLACK);

		DrawText(TextFormat("FPS: %i", GetFPS()), 10, 130, 20, BLACK);
	}
}