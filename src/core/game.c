#include "game.h"
#include "scenes/startscene.h"
#include "scenes/corescene.h"
#include "core/logger.h"
#include "raylib.h"

GameState g_CurrentState = TITLE;

void RunGame() {
    InitWindow(800, 450, "CARDS");
    while (!WindowShouldClose()) ManageScenes();
    CloseWindow();
}

void ManageScenes() {
    switch(g_CurrentState) {
        case TITLE:
            DrawStartScene();
            break;
        case CORE:
            DrawCoreScene();
            break;
        default:
            LOG_FATAL("Unhandled scene detected");
            break;
    }
}