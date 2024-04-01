#pragma once

typedef enum {
	CORE_NONE,
	CORE_INIT,
	CORE_MAIN
} CoreSceneState;

void DrawCoreScene();
void UpdateCoreScene();
void InitializeCoreScene();
