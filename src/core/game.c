#include "game.h"
#include "scenes/startscene.h"
#include "scenes/corescene.h"
#include "core/logger.h"
#include "raylib.h"

GameState g_CurrentState = CORE;

void RunGame() {
	SetConfigFlags(FLAG_VSYNC_HINT);
    InitWindow(800, 450, "CARDS");
    while (!WindowShouldClose()) ManageScenes();
    CloseWindow();
}

void ManageScenes() {
    switch(g_CurrentState) {
        case TITLE:
			UpdateStartScene();
            DrawStartScene();
            break;
        case CORE:
			UpdateCoreScene();
            DrawCoreScene();
            break;
        default:
            LOG_FATAL("Unhandled scene detected");
            break;
    }
}
