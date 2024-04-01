#include "corescene.h"
#include "core/logger.h"
#include "raylib.h"

CoreSceneState g_State    = CORE_NONE;
Camera2D       g_Camera   = { 0 };

void DrawCoreScene() {
    BeginDrawing();

    ClearBackground(DARKGRAY);
    DrawText("This is the CORE scene!", 190, 200, 20, GOLD);
    
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
			break;
		default:
			LOG_FATAL("Unhandled core scene state detected");	
	}
}

void InitializeCoreScene() {
	// initialize camera
    g_Camera.target = (Vector2){ 0.0f, 0.0f };
    g_Camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    g_Camera.rotation = 0.0f;
    g_Camera.zoom = 1.0f;
}
