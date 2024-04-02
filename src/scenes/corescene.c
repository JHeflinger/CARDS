#include "corescene.h"
#include "core/logger.h"
#include "raylib.h"

CoreSceneState        g_State              = CORE_NONE;
CoreNetworkObject     g_NetworkObject      = { 0 };
Camera2D              g_Camera             = { 0 };
Vector2               g_PlayerLocation     = { 0 };
Vector2               g_PlayerSize         = { 20.0f, 20.0f };

void DrawCoreScene() {
    BeginDrawing();

    ClearBackground(DARKGRAY);
    
	BeginMode2D(g_Camera);

	Rectangle b1 = {10, 10, 100, 200};
	DrawRectangleRec(b1, BLUE);

	int stepsize = 20;
	int gridsize = 100;
	for (int i = 0; i < gridsize; i++) {
		DrawLine(-1*i*stepsize, -1*gridsize*stepsize, -1*i*stepsize, gridsize*stepsize, LIGHTGRAY);
		DrawLine(i*stepsize, -1*gridsize*stepsize, i*stepsize, gridsize*stepsize, LIGHTGRAY);
		DrawLine(-1*gridsize*stepsize, i*stepsize, gridsize*stepsize, i*stepsize, LIGHTGRAY);
		DrawLine(-1*gridsize*stepsize, -1*i*stepsize, gridsize*stepsize, -1*i*stepsize, LIGHTGRAY);
	}

	Rectangle rec = {g_PlayerLocation.x, g_PlayerLocation.y, g_PlayerSize.x, g_PlayerSize.y};
	DrawRectangleRec(rec, GOLD);

	EndMode2D();

	CoreDevTrace();

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
	// backup initialize connection
	if (g_NetworkObject.m_Descriptor == CORE_IS_UNKNOWN)
		CoreBackupNetworkSetup();

	// initialize camera
    g_Camera.target = (Vector2){ g_PlayerLocation.x + g_PlayerSize.x/2.0f, g_PlayerLocation.y + g_PlayerSize.y/2.0f };
    g_Camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    g_Camera.rotation = 0.0f;
    g_Camera.zoom = 1.0f;

	// change state
	g_State = CORE_MAIN;
}

void MainCoreScene() {
	// basic wasd movement
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

	// update camera to bind to player 
	g_Camera.target = (Vector2){ g_PlayerLocation.x + g_PlayerSize.x/2.0f, g_PlayerLocation.y + g_PlayerSize.y/2.0f };
}

void CoreDevTrace() {
	// fps monitor
	char buffer[1024];
	sprintf(buffer, "FPS: %d", (int)(1.0f/GetFrameTime()));
	DrawText(buffer, 10, 10, 18, RAYWHITE);
}

void CoreBackupNetworkSetup() {
	
}
