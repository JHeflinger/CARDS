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
			UpdateCoreNetworkService();
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
	if (g_NetworkObject.m_HostDevice == NULL)
		g_NetworkObject.m_HostDevice = calloc(1, sizeof(ezn_Server));
	if (g_NetworkObject.m_ClientDevice == NULL)
		g_NetworkObject.m_ClientDevice = calloc(1, sizeof(ezn_Client));
	uint8_t address[4];
	ezn_set_ipv4_addr(address, 127, 0, 0, 1);
	if (ezn_configure_client(g_NetworkObject.m_ClientDevice, DEFAULT_CORE_PORT, address) == EZN_ERROR) {
		LOG_FATAL("Unable to set up client configuration");
	} else if (ezn_connect_client(g_NetworkObject.m_ClientDevice, ConnectAsClient) == EZN_ERROR) {
		LOG_WARN("Unable to connect to a local server - starting local server instead");
		if (ezn_generate_server(g_NetworkObject.m_HostDevice, DEFAULT_CORE_PORT) == EZN_ERROR)
			LOG_FATAL("Unable to generate server device");
		if (ezn_open_server(g_NetworkObject.m_HostDevice) == EZN_ERROR)
			LOG_FATAL("Unable to open local server");
		LOG_WARN("Implicitly starting server via backup network setup");
		g_NetworkObject.m_Descriptor = CORE_IS_HOST;
		if (ezn_server_accept(g_NetworkObject.m_HostDevice, HostAsServer) == EZN_ERROR) {
			LOG_FATAL("Unable to accept connections while hosting");
		}
	}
}

void UpdateCoreNetworkService() {

}

EZN_STATUS ConnectAsClient(ezn_Client* client, EZN_SOCKET serversock) {
	LOG_WARN("Implicitly connecting as client via backup network setup");
	g_NetworkObject.m_Descriptor = CORE_IS_CLIENT;

	return EZN_NONE;
}

EZN_STATUS HostAsServer(ezn_Server* server, EZN_SOCKET clientsock) {

	return EZN_NONE;
}