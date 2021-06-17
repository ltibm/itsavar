#ifndef _MAIN_H
#define _MAIN_H

#include "types.h"
#include "memory.h"

#include "extdll.h"
#include "meta_api.h"
#include "pm_defs.h"
#include "entity_state.h"
#include <windows.h>
#include <sys/types.h>

#include <WinSock.h>
#include "ddoscontrol.h"
#include "packetcontroller.h"
#include <locale.h>


#pragma comment(lib, "Ws2_32.lib" )

#pragma comment( lib, "Psapi.lib" ) 
#pragma comment( lib, "Kernel32.lib" ) 


#include <windows.h>
#include <Psapi.h>
#include <WinBase.h>
#include <io.h>
#include <malloc.h>

#define GET_EAX_POINTER(x) __asm mov x, edx;
#define PAGESIZE 4096


#define MIN_AMOUNT			100.0f

#define FLOAT_CROUCH			30.0f
#define MAX_CLIENTS			32

#define GET_DISTANCE(a,b)		((a - b).Length2D())

#define NUM_FOR_EDICT(e)		((int)(e - nullEdict))
#define EDICT_NUM(e)			((edict_t *)(nullEdict + e))

#ifdef _WIN32
	#define OFFSET_EDICT_CL 203788
#endif
#define GET_ORIG_FUNC(x) CFunc *x; GET_EAX_POINTER(x);


typedef enum
{
	READ_IN_GAME = 0,
	READ_START
} TypeRead;


typedef struct patch_s
{
	void *addr;
	char bytes[5];
	int nSize;
} patch_t;


using namespace std;
size_t ParseBlocks(vector<PacketList>&, char*);
void TrimSpace(char*);
static inline int IsCharSpecial(char);
int load_config();


void print_settings();

int parse_settings(const char *cvar,const char *value,TypeRead iType);

int OnMetaAttach();

void OnMetaDetach();
void SVR_ItSavar();
bool checkActivation();
void EngineFuncLoaded();
void SetCvarsValue();
float clampf(float value, float min, float max);
cvar_t* RegisterSVCvar(char* cvar_name, float cvar_value);
cvar_t* RegisterCvar(char* cvar_name, float cvar_value);
void ClientPutInServer_Post(edict_t *pEdict);

void PM_Move(playermove_t *pmove,int);

void AlertMessage(ALERT_TYPE atype, char *szFmt, ...);
void ServerDeactivate_Post();
void Client_PrintAll(char* Message);

void ServerActivate_Post(edict_t *pEdictList,int edictCount,int clientMax);

const char *pfnGetPlayerAuthId(edict_t *e);

void *Q_memcpy_Handler(void *_Dst,entity_state_t *_Src,uint32_t _Size);


extern bool g_bNotActive;

extern char **global_host_client;

extern patch_t patchData[];

extern DLL_FUNCTIONS *g_pFunctionTable;

extern enginefuncs_t *g_pEnginefuncsTable_Post;

extern enginefuncs_t *gpEnginefuncInterface;
typedef int socklen_t;
typedef size_t(PASCAL *Func_RecvFrom)(int, unsigned char*, size_t, int, const struct sockaddr*, socklen_t*);
size_t PASCAL OnRecvFrom(int socket, unsigned char* message, size_t length, int flags, const struct sockaddr* dest_addr, socklen_t* dest_len);
bool hookRecvFrom(void);
char* ToHex(ULONG decimal);
int parse_config();
int load_variables();
#endif //_MAIN_H

