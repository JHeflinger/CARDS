#include "corescene.h"
#include "raylib.h"

void DrawCoreScene() {
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawText("This is the CORE scene!", 190, 200, 20, GOLD);
    
    EndDrawing();
}