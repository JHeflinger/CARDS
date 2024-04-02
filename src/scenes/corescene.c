#include "corescene.h"
#include "core/logger.h"
#include "raylib.h"

CoreSceneState g_State            = CORE_NONE;
Camera2D       g_Camera           = { 0 };
Vector2        g_PlayerLocation   = { 0 };
Vector2        g_PlayerSize       = { 20.0f, 20.0f };

void DrawCoreScene() {
    BeginDrawing();

    ClearBackground(DARKGRAY);
    
	BeginMode2D(g_Camera);

	Rectangle rec = {g_PlayerLocation.x, g_PlayerLocation.y, g_PlayerSize.x, g_PlayerSize.y};
	DrawRectangleRec(rec, GOLD);

	EndMode2D();
    
    EndDrawing();
}

void UpdateCoreScene() {
	// handle scene states
	switch(g_State) {
		case CORE_NONE:
			g_State = CORE_INIT;
			break;
		case CORE_INIT:
			InitializeCoreScene();
			break;
		case CORE_MAIN:
			MainCoreScene();
			break;
		default:
			LOG_FATAL("Unhandled core scene state detected");	
	}
}

void InitializeCoreScene() {
	// initialize camera
    g_Camera.target = (Vector2){ g_PlayerLocation.x + g_PlayerSize.x/2.0f, g_PlayerLocation.y + g_PlayerSize.y/2.0f };
    g_Camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    g_Camera.rotation = 0.0f;
    g_Camera.zoom = 1.0f;

	// change state
	g_State = CORE_MAIN;
}

void MainCoreScene() {
	float ft = GetFrameTime();
	float speed = 100;
	if (IsKeyDown(KEY_W)) {
		g_PlayerLocation.y -= speed*ft;
	}
	if (IsKeyDown(KEY_A)) {
		g_PlayerLocation.x -= speed*ft;
	}
	if (IsKeyDown(KEY_S)) {
		g_PlayerLocation.y += speed*ft;
	}
	if (IsKeyDown(KEY_D)) {
		g_PlayerLocation.x += speed*ft;
	}
}