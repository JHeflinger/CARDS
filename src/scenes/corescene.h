#ifndef CORESCENE 
#define CORESCENE 

#include "utils/datastructs.h"
#include "easynet.h"

#define DEFAULT_CORE_PORT 69421

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
	ARRLIST_EZN_SOCKET m_ConnectedDevices;
} CoreNetworkObject;

void DrawCoreScene();
void UpdateCoreScene();
void InitializeCoreScene();
void MainCoreScene();
void CoreDevTrace();
void CoreBackupNetworkSetup();
void UpdateCoreNetworkService();

EZN_STATUS ConnectAsClient(ezn_Client* client, EZN_SOCKET serversock);
EZN_STATUS HostAsServer(ezn_Server* server, EZN_SOCKET clientsock);

#endif