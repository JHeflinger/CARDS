#ifndef CORESCENE 
#define CORESCENE 

#include "easynet.h"

typedef enum {
	CORE_NONE = 0,
	CORE_INIT,
	CORE_MAIN
} CoreSceneState;

typedef enum {
	CORE_IS_UNKNOWN = 0,
	CORE_IS_HOST,
	CORE_IS_CLIENT
} CoreNetworkDescriptor;

typedef struct {
	CoreNetworkDescriptor m_Descriptor;
	ezn_Server* m_HostDevice;
	ezn_Client* m_ClientDevice;
} CoreNetworkObject;

void DrawCoreScene();
void UpdateCoreScene();
void InitializeCoreScene();
void MainCoreScene();
void CoreDevTrace();
void CoreBackupNetworkSetup();

#endif
