#include "corescene.h"
#include "core/logger.h"
#include "core/input.h"
#include "utils/timeutils.h"
#include "utils/numbers.h"
#include "data/datamap.h"
#include "data/collisionmap.h"
#include "data/baked.h"
#include "objects/projectile.h"
#include "raylib.h"
#include <math.h>

#define CELLSIZE 20 
#define DEBUG_SINGLEPLAYER
#define IOTA 0.0001f

CoreSceneState        g_State              = CORE_NONE;
CoreNetworkObject     g_NetworkObject      = { 0 };
Camera2D              g_Camera             = { 0 };
Vector2               g_PlayerLocation     = { 0 };
Vector2               g_PlayerSize         = { CELLSIZE, CELLSIZE };
CollisionMap*         g_CollisionMap       = NULL;
ARRLIST_ProjectilePtr g_Projectiles        = { 0 };

// temp?
Vector2               g_EnemyLocation      = { 0 };
uint32_t              g_Ping               = -1;
int                   g_PlayerColliding    = 0;

void DrawCoreScene() {
    BeginDrawing();

    ClearBackground(DARKGRAY);
    
	BeginMode2D(g_Camera);

	Rectangle b1 = {10, 10, 100, 200};
	DrawRectangleRec(b1, BLUE);

	int stepsize = CELLSIZE;
	int gridsize = 100;
	for (int i = 0; i < gridsize; i++) {
		DrawLine(-1*i*stepsize, -1*gridsize*stepsize, -1*i*stepsize, gridsize*stepsize, LIGHTGRAY);
		DrawLine(i*stepsize, -1*gridsize*stepsize, i*stepsize, gridsize*stepsize, LIGHTGRAY);
		DrawLine(-1*gridsize*stepsize, i*stepsize, gridsize*stepsize, i*stepsize, LIGHTGRAY);
		DrawLine(-1*gridsize*stepsize, -1*i*stepsize, gridsize*stepsize, -1*i*stepsize, LIGHTGRAY);
	}

	Rectangle rec = {g_PlayerLocation.x, g_PlayerLocation.y, g_PlayerSize.x, g_PlayerSize.y};
	DrawRectangleRec(rec, GOLD);

	Rectangle rec2 = { g_EnemyLocation.x, g_EnemyLocation.y, g_PlayerSize.x, g_PlayerSize.y};
	DrawRectangleRec(rec2, RED);

	DrawDevObjects();

	DrawProjectiles();

	EndMode2D();

	DrawDevUI();

    EndDrawing();
}

void DrawProjectiles() {
	for (size_t i = 0; i < g_Projectiles.size; i++) {
		Projectile* projectile = ARRLIST_ProjectilePtr_get(&g_Projectiles, i);
		Rectangle rec = { projectile->position.x, projectile->position.y, projectile->size.x, projectile->size.y };
		DrawRectangleRec(rec, BLUE);
	}
}

void DrawDevObjects() {
	if (g_CollisionMap != NULL) {
		for (int y = 0; y < g_CollisionMap->height; y++) {
			for (int x = 0; x < g_CollisionMap->width; x++) {
				if (g_CollisionMap->data[x][y] == 'B') {
					Rectangle hitbox = {x*CELLSIZE + g_CollisionMap->x*CELLSIZE, y*CELLSIZE + g_CollisionMap->y*CELLSIZE, CELLSIZE, CELLSIZE};
					DrawRectangleRec(hitbox, GREEN);
				}
			}
		}
	}
}

void DrawDevUI() {
	// fps monitor
	char buffer[1024];
	sprintf(buffer, "FPS: %d", (int)(1.0f/GetFrameTime()));
	DrawText(buffer, 10, 10, 18, RAYWHITE);
	sprintf(buffer, "PING: %u", g_Ping);
	DrawText(buffer, 10, 30, 18, RAYWHITE);
	DrawText("COLLIDING", 10, 50, 18, g_PlayerColliding == 1 ? RED : GREEN);

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
			#ifndef DEBUG_SINGLEPLAYER
			CheckCoreNetworkService();
			#endif
			MainCoreScene();
			#ifndef DEBUG_SINGLEPLAYER
			UpdateCoreNetworkService();
			#endif
			break;
		default:
			LOG_FATAL("Unhandled core scene state detected");	
	}
}

void UpdateProjectiles() {
	for (size_t i = 0; i < g_Projectiles.size; i++) {
		Projectile* projectile = ARRLIST_ProjectilePtr_get(&g_Projectiles, i);
		if (projectile->enable_flags & ENABLE_PROJECTILE_ON_SPAWN) {
			projectile->enable_flags = projectile->enable_flags ^ ENABLE_PROJECTILE_ON_SPAWN;
			projectile->on_spawn(NULL, 0);
		} 
		if (projectile->enable_flags & ENABLE_PROJECTILE_ON_UPDATE) {
			projectile->on_update(NULL, 0);
		} 
		if (projectile->enable_flags & ENABLE_PROJECTILE_ON_COLLISION) {
			int collided = 0;
			int64_t ocoord_x = (int64_t)floor(projectile->position.x / (float)CELLSIZE);
			int64_t ocoord_y = (int64_t)floor(projectile->position.y / (float)CELLSIZE);
			int64_t ocoord_w = (int64_t)floor((projectile->position.x + projectile->size.x) / (float)CELLSIZE);
			int64_t ocoord_h = (int64_t)floor((projectile->position.y + projectile->size.y) / (float)CELLSIZE);
			for (int64_t x = ocoord_x; x <= ocoord_w && collided == 0; x++) {
				for (int64_t y = ocoord_y; y <= ocoord_h && collided == 0; y++) {
					int64_t cell_x = x - g_CollisionMap->x;
					int64_t cell_y = y - g_CollisionMap->y;
					if (cell_x >= 0 && cell_y >= 0 && cell_x < g_CollisionMap->width && cell_y < g_CollisionMap->height && g_CollisionMap->data[(size_t)cell_x][(size_t)cell_y] == 'B')
						collided = 1;
				}
			}
			if (collided) 
				projectile->on_collision(NULL, 0);
		} 
		if (projectile->enable_flags & ENABLE_PROJECTILE_ON_HIT) {
			LOG_WARN("Projectile on-hit is not implemented yet!"); //TODO
		}
		if (projectile->lifetime <= 0) {
			if (projectile->enable_flags & ENABLE_PROJECTILE_ON_DEATH) {
				projectile->on_death(NULL, 0);
			}
			ARRLIST_ProjectilePtr_remove(&g_Projectiles, i);
			free(projectile);
			i--;
		} else {
			float ft = GetFrameTime();
			projectile->lifetime -= ft;
			projectile->position.x += ft*projectile->velocity.x;
			projectile->position.y += ft*projectile->velocity.y;
		}
	}
}

void InitializeCoreScene() {
	#ifndef DEBUG_SINGLEPLAYER
	// backup initialize connection
	if (g_NetworkObject.m_Descriptor == CORE_IS_UNKNOWN)
		CoreBackupNetworkSetup();
	#endif

	// initialize camera
    g_Camera.target = (Vector2){ g_PlayerLocation.x + g_PlayerSize.x/2.0f, g_PlayerLocation.y + g_PlayerSize.y/2.0f };
    g_Camera.offset = (Vector2){ GetScreenWidth()/2.0f, GetScreenHeight()/2.0f };
    g_Camera.rotation = 0.0f;
    g_Camera.zoom = 1.0f;

	// initialize collision map 
	Datamap* coldata = GenerateDatamap(DEV_MAP);
	g_CollisionMap = GenerateCollisionMap();
	LoadCollisionChunk(g_CollisionMap, coldata);
	FreeDatamap(coldata);
	coldata = GenerateDatamap(DEV_MAP_2);
	LoadCollisionChunk(g_CollisionMap, coldata);
	FreeDatamap(coldata);

	// change state
	g_State = CORE_MAIN;
}

void MainCoreScene() {
	// update input modules
	UpdateSmartInput();

	// fire projectiles
	Vector2 projvelo = { 0 };
	float projspeed = 100.0f;
	switch (PeekIK()) {
		case 'i':
			projvelo.y = -1.0f;
			break;
		case 'k':
			projvelo.y = 1.0f;
			break;
		default:
			projvelo.y = 0.0f;
	}
	switch (PeekJL()) {
		case 'j':
			projvelo.x = -1.0f;
			break;
		case 'l':
			projvelo.x = 1.0f;
			break;
		default:
			projvelo.x = 0.0f;
	}
	if (projvelo.x != 0 || projvelo.y != 0) {
		projvelo.x *= projspeed;
		projvelo.y *= projspeed;
		Projectile* projectile = GenerateDefaultProjectile(g_PlayerLocation, projvelo);
		ARRLIST_ProjectilePtr_add(&g_Projectiles, projectile);
	}

	// update velocity
	float ft = GetFrameTime();
	float speed = CELLSIZE * 10;
	Vector2 vel = { 0 };
	switch (PeekWS()) {
		case 'w':
			vel.y = -1.0f;
			break;
		case 's':
			vel.y = 1.0f;
			break;
		default:
			vel.y = 0;
	}
	switch (PeekAD()) {
		case 'd':
			vel.x = 1.0f;
			break;
		case 'a':
			vel.x = -1.0f;
			break;
		default:
			vel.x = 0;
	}
	if (vel.x != 0 && vel.y != 0) {
		vel.x /= SQRT2;
		vel.y /= SQRT2;
	}

	// process collision checking
	Vector2 rollback = g_PlayerLocation;
	g_PlayerLocation.x += speed*ft*vel.x;
	g_PlayerLocation.y += speed*ft*vel.y;
	g_PlayerColliding = 0;
	Vector2 newloc = g_PlayerLocation;
	for (int x = 0; x < 2; x++) {
		for (int y = 0; y < 2; y++) {
			int64_t coordx = (int64_t)floor(g_PlayerLocation.x / (float)CELLSIZE) - g_CollisionMap->x + x;
			int64_t rollbx = (int64_t)floor(rollback.x / (float)CELLSIZE) - g_CollisionMap->x + x;
			int64_t coordy = (int64_t)floor(g_PlayerLocation.y / (float)CELLSIZE) - g_CollisionMap->y + y;
			int64_t rollby = (int64_t)floor(rollback.y / (float)CELLSIZE) - g_CollisionMap->y + y;
			int xcol = coordx >= 0 && rollby >= 0 && coordx < g_CollisionMap->width && rollby < g_CollisionMap->height && g_CollisionMap->data[(size_t)coordx][(size_t)rollby] == 'B';
			int ycol = rollbx >= 0 && coordy >= 0 && rollbx < g_CollisionMap->width && coordy < g_CollisionMap->height && g_CollisionMap->data[(size_t)rollbx][(size_t)coordy] == 'B';
			int bcol = coordx >= 0 && coordy >= 0 && coordx < g_CollisionMap->width && coordy < g_CollisionMap->height && g_CollisionMap->data[(size_t)coordx][(size_t)coordy] == 'B';
			g_PlayerColliding = g_PlayerColliding | xcol | ycol | bcol;
			if (xcol && vel.x > 0) newloc.x = CELLSIZE*(coordx - x - g_CollisionMap->x) - IOTA;
			if (xcol && vel.x < 0) newloc.x = CELLSIZE*(coordx - x - g_CollisionMap->x + 1) + IOTA;
			if (ycol && vel.y > 0) newloc.y = CELLSIZE*(coordy - y - g_CollisionMap->y) - IOTA;
			if (ycol && vel.y < 0) newloc.y = CELLSIZE*(coordy - y - g_CollisionMap->y + 1) + IOTA;
		}
	}
	g_PlayerLocation = newloc;

	// update projectiles
	UpdateProjectiles();

	// update camera to bind to player 
	g_Camera.target = (Vector2){ g_PlayerLocation.x + g_PlayerSize.x/2.0f, g_PlayerLocation.y + g_PlayerSize.y/2.0f };
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
		if (ezn_server_accept(g_NetworkObject.m_HostDevice, HostAsServer, EZN_FALSE) == EZN_ERROR) {
			LOG_FATAL("Unable to accept connections while hosting");
		}
	}
}

void CheckCoreNetworkService() {
	// temp 
	if (g_NetworkObject.m_ConnectedDevices.size > 0) {
		size_t validation_len;
		EZN_BYTE buff[12];
		if (ezn_ask(ARRLIST_EZN_SOCKET_get(&(g_NetworkObject.m_ConnectedDevices), 0), buff, 12, &validation_len) == EZN_ERROR) {
			LOG_FATAL("error while asking for network packets!");
		}
		if ((int)validation_len > 0) {
			g_EnemyLocation.x = *((float*)buff);
			g_EnemyLocation.y = *((float*)(buff + 4));
			g_Ping = GetUnpreciseEpoch() - *((uint32_t*)(buff + 8));
		}
	} else {
		LOG_FATAL("No server available!");
	}

	switch(g_NetworkObject.m_Descriptor) {
		case CORE_IS_CLIENT:
			break;
		case CORE_IS_HOST:
			break;
		default:
			LOG_FATAL("Invalid network status - cannot update network service");
	}
}

void UpdateCoreNetworkService() {
	// temp 
	if (g_NetworkObject.m_ConnectedDevices.size > 0) {
		size_t validation_len;
		EZN_BYTE buff[12];
		uint32_t currtime = GetUnpreciseEpoch();
		memcpy(buff, &g_PlayerLocation.x, 4);
		memcpy(buff + 4, &g_PlayerLocation.y, 4);
		memcpy(buff + 8, &currtime, 4);
		if (ezn_send(ARRLIST_EZN_SOCKET_get(&(g_NetworkObject.m_ConnectedDevices), 0), buff, 12, &validation_len) == EZN_ERROR) {
			LOG_FATAL("unable to send bytes!");
		}
	} else {
		LOG_FATAL("No server available!");
	}

	switch(g_NetworkObject.m_Descriptor) {
		case CORE_IS_CLIENT:
			break;
		case CORE_IS_HOST:
			break;
		default:
			LOG_FATAL("Invalid network status - cannot update network service");
	}
}

void CleanCoreScene() {
	FreeCollisionMap(g_CollisionMap);
}

EZN_STATUS ConnectAsClient(ezn_Client* client, EZN_SOCKET serversock) {
	LOG_WARN("Implicitly connecting as client via backup network setup");
	g_NetworkObject.m_Descriptor = CORE_IS_CLIENT;
	ARRLIST_EZN_SOCKET_add(&(g_NetworkObject.m_ConnectedDevices), serversock);
	return EZN_NONE;
}

EZN_STATUS HostAsServer(ezn_Server* server, EZN_SOCKET clientsock) {
	ARRLIST_EZN_SOCKET_add(&(g_NetworkObject.m_ConnectedDevices), clientsock);
	return EZN_NONE;
}
