

#include "main.h"
#include "logwriter.h"

#include <vector>
#include "detours.h"
#include "CommonUtils.h"
#include "ini.h"
#include "Settings.h"
#define CONFIG_FILE "itsavar.ini"
#define CFG_FILE  "itsavar.cfg"
#define IsValidPev(pEntity) (!FNullEnt(pEntity) && pEntity->pvPrivateData)  
#define PREFIX "[Lt. ItSavar] "

std::vector<char*> allowedIp;
Settings* MySettings = new Settings();
packetcontroller* bannedpacket;
packetcontroller* allowedpacket;
packetcontroller* controlpacket;
static edict_t* maxEdict;
static edict_t* nullEdict;
static edict_t* startEdict;
Func_RecvFrom RecvFromOriginal = NULL;
bool g_bNotActive = false;
char** global_host_client;


patch_t patchData[] =
{
	{ (void*)NULL, {}, 0 },
	{ (void*)NULL, {}, 0 },
	{ (void*)NULL, {}, 0 }
};

ddoscontrol* ddoscontrolitem;
time_t son_tarama_zamani;
time_t son_banned_packtime;
cvar_t* cvar_ban_time;
cvar_t* cvar_single_flood;
cvar_t* cvar_max_flood;
cvar_t* cvar_flood_time;
cvar_t* cvar_secure_type;
cvar_t* cvar_check_total;
cvar_t* cvar_check_single;
cvar_t* cvar_log;
cvar_t* cvar_log_packets;
cvar_t* cvar_warn_players;
cvar_t* cvar_warn_server;
cvar_t* cvar_cleartime;
cvar_t* cvar_warn_bannedpacket;
cvar_t* cvar_ban_bannedpacketip;
cvar_t* cvar_enabled_allowed;
logwriter* log_writer;

static char mPluginPath[256];
static char mConfigPath[256];
static char mCfgPath[256];

bool isfinish = false;
bool isloaded = false;
bool block = false;
int secure_type = 1;
int userMsgId = -1;
bool cvarvalue_enable_allowedip = true;
double cvarvalue_cleartime = 10;
bool cvarvalue_log = true;
bool cvarvalue_log_packets = true;
bool cvarvalue_warnserver = true;
bool cvarvalue_warnplayers = true;
bool cvarvalue_warnbannedpacket = true;
bool cvarvalue_banpacketip = true;
void OnMetaDetach()
{
	isfinish = true;
	/*for(int j = 0; j < 3; j++)
	{
		if(patchData[j].addr)
			mem_memcpy(patchData[j].addr,patchData[j].bytes,patchData[j].nSize);
	}*/
	if (log_writer) log_writer->closelogfile();
}
//std::thread  CIPContainerControl;
int totalblockedAttack = 0;
int OnMetaAttach()
{
	//setlocale(LC_ALL, "Turkish.utf8");
	isfinish = false;
	secure_type = 1;
	bool result = hookRecvFrom();
	load_variables();
	char* iniPath = new char[255];
	sprintf(iniPath, "%s%s", mPluginPath, CONFIG_FILE);
	mINI::INIFile iniFile(iniPath);
	mINI::INIStructure ini;
	if (iniFile._read(ini))
	{
		if (ini.has("main"))
		{
			auto& main = ini["main"];
			if (main.has("Language"))
			{
				MySettings->SetLanguage(main.get("Language").c_str());
			}
			if (main.has("DateFormat"))
			{
				std::string df = main.get("DateFormat");

				MySettings->DateFormat = new char[df.size() + 1];
				strcpy(MySettings->DateFormat, df.c_str());
			}
		}
		printf("%s%s", PREFIX, MySettings->GetText("CONFIG_LOADED"));
	}
	else printf("%s%s", PREFIX, MySettings->GetText("CONFIG_FAIL"));
	char* modulName = MySettings->GetText("MODULE_NAME");
	printf("\n%s%s\n", PREFIX, modulName);
	printf("%s%s.\n\n", PREFIX, result ? MySettings->GetTextFmt("MODULE_LOADED", modulName) : MySettings->GetTextFmt("MODULE_LOAD_FAIL", modulName));

	if (result)
	{
		ddoscontrolitem = new ddoscontrol();
		ddoscontrolitem->check_single_flood = true;
		ddoscontrolitem->check_total_flood = true;
		bannedpacket = new packetcontroller();
		allowedpacket = new packetcontroller();
		controlpacket = new packetcontroller();
		load_config();
		log_writer = new logwriter();
		sprintf(log_writer->logfilefolder, "%s%s/", mPluginPath, "logs");
		REG_SVR_COMMAND("itsavar", SVR_ItSavar);
		isloaded = true;
		char* c = new char[255];

		sprintf(c, "exec \"%s\"\n", mCfgPath);
	
		g_engfuncs.pfnServerCommand(c);
		//g_engfuncs.pfnServerExecute();
		//userMsgId = REG_USER_MSG("SayText", -1);
	}
	son_tarama_zamani = std::time(0);
	return result;
}
bool hookRecvFrom(void)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	HMODULE h = GetModuleHandle("ws2_32.dll");
	RecvFromOriginal = (Func_RecvFrom)GetProcAddress(h, "recvfrom");
	LONG error = DetourAttach(&(PVOID&)RecvFromOriginal, OnRecvFrom);
	if (error) return false;
	DetourTransactionCommit();
	return true;
}
char* ToHex(ULONG decimal)
{
	char* chr = new char[16];
	memset(chr, '\0', sizeof(chr) - 1);
	if (decimal < 1) return "0";
	USHORT totale = 15;
	UCHAR hex = '\0';
	USHORT total = 0;
	while (decimal > 0)
	{
		hex = (unsigned char)(decimal % 16);
		UCHAR hexchar;
		if (hex < 10)
			hexchar = hex + 48;
		else
			hexchar = hex + 55;

		decimal /= 16;
		chr[totale] = hexchar;
		total++;
		totale--;
	}
	char* chr2 = new char[total];
	memset(chr2, '\0', sizeof(chr2) - 1);
	for (size_t i = 0; i < total; i++)
	{
		chr2[i] = chr[totale + i + 1];
	}
	delete chr;
	return chr2;
}
void EngineFuncLoaded()
{
	cvar_ban_time = RegisterCvar("itsavar_ban_time", 200);
	cvar_flood_time = RegisterCvar("itsavar_flood_time", 3);
	cvar_single_flood = RegisterCvar("itsavar_flood_single", 22);
	cvar_max_flood = RegisterCvar("itsavar_flood_max", 300);
	cvar_secure_type = RegisterCvar("itsavar_securetype", 1);
	cvar_check_single = RegisterCvar("itsavar_check_single", 1);
	cvar_check_total = RegisterCvar("itsavar_check_total", 1);
	cvar_warn_players = RegisterCvar("itsavar_warn_players", 1);
	cvar_warn_server = RegisterCvar("itsavar_warn_server", 1);
	cvar_warn_bannedpacket = RegisterCvar("itsavar_warn_bannedpacket", 1);
	cvar_ban_bannedpacketip = RegisterCvar("itsavar_ban_bannedpacketip", 1);
	cvar_log = RegisterCvar("itsavar_log", 1);
	cvar_log_packets = RegisterCvar("itsavar_log_packets", 1);
	cvar_cleartime = RegisterCvar("itsavar_ip_cleartime", 10);
	cvar_enabled_allowed = RegisterCvar("itsavar_enabled_allowed_ip", 1);

	RegisterSVCvar("itsavar_version", 1.2f);
	SetCvarsValue();


}
time_t lastTime = 0;
void SetCvarsValue()
{
	if (!isloaded) return;
	time_t time = std::time(0);
	if (lastTime > 0 && time - lastTime <= 1) return;
	ddoscontrolitem->e_ban_suresi = clampf(cvar_ban_time->value, 30, 3600);
	ddoscontrolitem->e_paket_cache_suresi = clampf(cvar_flood_time->value, 1, 30);
	ddoscontrolitem->e_teksefer_limit = clampf(cvar_single_flood->value, 3, 300);
	ddoscontrolitem->e_toplam_limit = clampf(cvar_max_flood->value, 5, 3000);
	ddoscontrolitem->check_single_flood = cvar_check_single->value;
	ddoscontrolitem->check_total_flood = cvar_check_total->value;
	secure_type = clampf(cvar_secure_type->value, 0, 2);
	cvarvalue_enable_allowedip = cvar_enabled_allowed->value > 0;
	cvarvalue_cleartime = cvar_cleartime->value;
	cvarvalue_log = cvar_log->value > 0;
	cvarvalue_log_packets = cvar_log_packets->value > 0;
	cvarvalue_warnserver = cvar_warn_server->value > 0;
	cvarvalue_warnplayers = cvar_warn_players->value > 0;
	cvarvalue_warnbannedpacket = cvar_warn_bannedpacket->value > 0;
	cvarvalue_banpacketip = cvar_ban_bannedpacketip->value > 0;
	lastTime = time;

}
float clampf(float value, float min, float max)
{
	if (value < min) return min;
	if (value > max) return max;
	return value;
}
cvar_t* RegisterSVCvar(char* cvar_name, float cvar_value)
{
	cvar_t& cvar = cvar_t();
	cvar.name = new char[25];
	cvar.value = cvar_value;
	cvar.string = new char[10];
	cvar.flags = FCVAR_SERVER;
	sprintf(cvar.string, "%f", cvar_value);
	strcpy(cvar.name, cvar_name);
	//g_engfuncs.pfnCVarRegister(cvar);
	g_engfuncs.pfnCVarRegister(&cvar);
	return g_engfuncs.pfnCVarGetPointer(cvar_name);
}
cvar_t* RegisterCvar(char* cvar_name, float cvar_value)
{

	cvar_t& cvar = cvar_t();
	cvar.name = new char[25];
	cvar.value = cvar_value;
	cvar.string = new char[10];
	sprintf(cvar.string, "%f", cvar_value);
	strcpy(cvar.name, cvar_name);
	//g_engfuncs.pfnCVarRegister(cvar);
	g_engfuncs.pfnCVarRegister(&cvar);

	return g_engfuncs.pfnCVarGetPointer(cvar_name);
}

size_t PASCAL OnRecvFrom(int socket, unsigned char* message, size_t length, int flags, const struct sockaddr* dest_addr, socklen_t* dest_len)
{
	unsigned char* origMessage = (unsigned char*)message;
	size_t ret_length = RecvFromOriginal(socket, message, length, flags, dest_addr, dest_len);
	SetCvarsValue();
	if (secure_type == 0 || !isloaded)
	{
		return ret_length;
	}

	if (ret_length <= 0) return ret_length;
	struct sockaddr_in* addr_in = (struct sockaddr_in*)dest_addr;
	char* iipaddrs = inet_ntoa(addr_in->sin_addr);
	if (cvarvalue_enable_allowedip)
	{
		for (size_t i = 0; i < allowedIp.size(); i++)
		{
			if (strcmpi(allowedIp[i], iipaddrs) == 0) return ret_length;
		}
	}
	if (allowedpacket->is_match(origMessage, ret_length))
	{
		return ret_length;
	}
	bool ipbanned = ddoscontrolitem->is_ipbanned(iipaddrs);
	bool stopit = false;
	bool checkit = false;
	std::time_t now = std::time(0);
	if (!ipbanned && bannedpacket->is_match(origMessage, ret_length))
	{
		if (cvarvalue_banpacketip)
		{
			ddoscontrolitem->AddIPBan(iipaddrs);
		}
		if (cvarvalue_warnbannedpacket)
		{
			double efark = difftime(now, son_banned_packtime);
			if (cvarvalue_banpacketip || efark > 200)
			{
				char* chr = new char[255];
				if (cvarvalue_warnserver)
				{
					printf("\n%s%s\n", PREFIX, MySettings->GetTextFmt("IP_PACKET_BLOCK", iipaddrs));
				}
				sprintf(chr, "%s%s", PREFIX, MySettings->GetTextFmt("IP_PACKET_BLOCK", iipaddrs));
				if (cvarvalue_log)
				{
					if (log_writer) log_writer->writelog(chr, TimeToString(&now, MySettings->DateFormat));
				}
				if (cvarvalue_warnplayers)
				{
					Client_PrintAll(chr);
				}
				delete chr;
			}
		}
		totalblockedAttack++;
		son_banned_packtime = now;
		stopit = true;
	}
	double efark = difftime(now, son_tarama_zamani);
	if (efark >= cvarvalue_cleartime)
	{
		son_tarama_zamani = now;
		ddoscontrolitem->clear_unactionip();
	}
	if (!stopit && secure_type == 2)
	{
		return ret_length;
	}
	if (controlpacket->is_match(origMessage, ret_length))
	{
		checkit = true;
	}
	if (!stopit && checkit)
	{
		if (ipbanned)
		{
			stopit = true;
		}
		else
		{
			bannedreason banstatus = ddoscontrolitem->get_banstatus(iipaddrs, origMessage, ret_length);
			if (banstatus != nobanned)
			{
				int stip = 0;
				if (cvarvalue_warnserver)
				{
					if (banstatus == banned_allpacket)
					{
						printf("\n%s%s\n", PREFIX, MySettings->GetTextFmt("IP_PACKET_BLOCK_DIFF", iipaddrs));
					}
					else
					{
						stip = 1;
						//This ip adress temporary blocked due its sends same packet.
						printf("\n%s%s\n", PREFIX, MySettings->GetTextFmt("IP_PACKET_BLOCK_SAME", iipaddrs));

					}
				}
				totalblockedAttack++;
				char* chr = new char[255];
				sprintf(chr, "%s%s", PREFIX, MySettings->GetTextFmt("IP_PACKET_BLOCK_DOUBT", stip, iipaddrs));
				if (cvarvalue_log)
				{
					if (log_writer) log_writer->writelog(chr, TimeToString(&now, MySettings->DateFormat));
				}
				if (cvarvalue_log && cvarvalue_log_packets)
				{
					if (stip == 1 && ret_length > 0)
					{
						char* chrex = new char[255];
						int tprint = sprintf(chrex, "%s: ", MySettings->GetText("PACKET"));
						size_t retl2 = ret_length;
						if (retl2 > 100) retl2 = 100;
						for (size_t i = 0; i < retl2; i++)
						{

							tprint += sprintf(chrex + tprint, "0x%s ", ToHex((int)origMessage[i]));
						}
						if (ret_length > retl2)
						{
							tprint += sprintf(chrex + tprint, "+");
						}
						if (log_writer) log_writer->writelog(chrex, TimeToString(&now, MySettings->DateFormat));
						delete chrex;
					}
				}

				if (cvarvalue_warnplayers)
				{
					Client_PrintAll(chr);
				}
				delete chr;
				stopit = true;

			}
		}
	}

	if (stopit)
	{
		memset(message, 0, 1);
		return 1;
	}
	return ret_length;
}
static uint16_t FixedUnsigned16(float fValue, float fScale)
{
	int output = (int)(fValue * fScale);

	if (output < 0)
		output = 0;

	if (output > 0xFFFF)
		output = 0xFFFF;

	return (uint16_t)output;
}
void SVR_ItSavar()
{
	if (CMD_ARGC() < 2)
	{
		print_settings();
		return;
	}
	printf("\n");
	const char* argv = CMD_ARGV(1);
	const char* value = CMD_ARGV(2);
	if (strcmpi(argv, "list") == 0)
	{
		ddoscontrolitem->clear_unactionip();
		int blocked = ddoscontrolitem->BlockedIPCount();
		if (blocked == 0)
		{
			printf("%s%s\n", PREFIX, MySettings->GetText("CMD_LIST_NOIP"));
		}
		else
		{
			printf("%s%s", PREFIX, MySettings->GetTextFmt("CMD_LIST_HEADER", blocked));
			int total = 1;
			for (size_t i = 0; i < ddoscontrolitem->ddosiplist.size(); i++)
			{
				ddositem* item = ddoscontrolitem->ddosiplist[i];
				if (!item || !item->is_banned || item->flagged_removed) continue;
				printf("\n%s", MySettings->GetTextFmt("CMD_LIST_CONTENT", total++, item->ip_address, TimeToString(&item->bannedtime, MySettings->DateFormat), i));
			}
		}
	}
	else if (strcmpi(argv, "clear") == 0)
	{
		ddoscontrolitem->Clear(true);
		printf("%s%s", PREFIX, MySettings->GetText("CMD_CLEAR_OK"));
	}
	else if (strcmpi(argv, "remove") == 0)
	{
		if (CMD_ARGC() < 3 || *value == '\0')
		{
			printf("%s%s: list index|ip_address\n", PREFIX, MySettings->GetText("USAGE"));
			return;
		}
		int index = -1;
		if (strlen(value) < 5)
		{

			if (IsAllNumeric(value))
			{
				index = atoi(value);
			}
		}
		else
		{
			index = ddoscontrolitem->GetIdByAddr(value);
		}
		if (index >= ddoscontrolitem->ddosiplist.size()) index = -1;
		if (index <= -1)
		{
			printf("%s%s", PREFIX, MySettings->GetText("CMD_REMOVE_INVALID"));
		}
		else

		{
			ddoscontrolitem->RemoveAt(index, true);
			printf("%s%s", PREFIX, MySettings->GetText("CMD_REMOVE_OK"));
		}

	}
	else if (strcmpi(argv, "info") == 0)
	{
		printf("%s Anti DDOS %s: v1.2", PREFIX, MySettings->GetText("CMD_INFO_VERSION"));
		printf("\n%s", MySettings->GetTextFmt("CMD_INFO_TOTAL_BLOCK", totalblockedAttack));
		printf("\n%s", MySettings->GetTextFmt("CMD_INFO_CUR_BLOCK", ddoscontrolitem->BlockedIPCount()));

		printf("\n%s", MySettings->GetTextFmt("CMD_INFO_ALLOWED_IPCOUNT", allowedIp.size()));
		printf("\n%s", MySettings->GetTextFmt("CMD_INFO_LOADED_BANNED", bannedpacket->bList.size()));
		printf("\n%s", MySettings->GetTextFmt("CMD_INFO_LOADED_ALLOWED", allowedpacket->bList.size()));
		printf("\n%s", MySettings->GetTextFmt("CMD_INFO_LOADED_CHECKS", controlpacket->bList.size()));
	}
	printf("\n");

}
void* Q_memcpy_Handler(void* _Dst, entity_state_t* _Src, uint32_t _Size)
{
	return memcpy(_Dst, _Src, _Size);
}
void PM_Move(playermove_t* pmove, int server)
{
	RETURN_META(MRES_IGNORED);
}
const char* pfnGetPlayerAuthId(edict_t* e)
{
	RETURN_META_VALUE(MRES_IGNORED, NULL);
}
bool sv_activated = false;
void ServerDeactivate_Post()
{
	sv_activated = false;
	RETURN_META(MRES_IGNORED);
}
void Client_PrintAll(char* Message)
{

	//int gmsgidSayText = REG_USER_MSG("SayText", -1);
	if (userMsgId <= 0)
	{
		userMsgId = GET_USER_MSG_ID(PLID, "SayText", 0);
		if (userMsgId <= 0) return;
	}
	int mlen = strlen(Message);
	for (int i = 1; i <= gpGlobals->maxClients; ++i)
	{

		edict_t* pPlayer = INDEXENT(i);
		if (!IsValidPev(pPlayer))
			continue;
		gpEnginefuncInterface->pfnMessageBegin(MSG_ONE, userMsgId, NULL, pPlayer);
		gpEnginefuncInterface->pfnWriteByte(0);
		gpEnginefuncInterface->pfnWriteString(Message);
		gpEnginefuncInterface->pfnMessageEnd();
		/*	g_engfuncs.pfnClientPrintf(pPlayer, (PRINT_TYPE) 3, Message);*/


	}
}
void ServerActivate_Post(edict_t* pEdictList, int edictCount, int clientMax)
{
	sv_activated = true;
	g_bNotActive = false;
	parse_config();
	/*cvar_t* cvar = new cvar_t();

	cvar->name = new char[10];
	CVAR_REGISTER(cvar);*/
	nullEdict = pEdictList;
	startEdict = pEdictList + 1;
	maxEdict = pEdictList + clientMax;
	totalblockedAttack = 0;
	char* c = new char[255];

	sprintf(c, "exec \"%s\"", mCfgPath);
	g_engfuncs.pfnServerCommand(c);
	g_engfuncs.pfnServerExecute();


	RETURN_META(MRES_IGNORED);
}
void ClientPutInServer_Post(edict_t* pEdict)
{
	if (!pEdict->pvPrivateData)
	{
		RETURN_META(MRES_IGNORED);
	}
	RETURN_META(MRES_IGNORED);
}
void AlertMessage(ALERT_TYPE atype, char* fmt, ...)
{
	RETURN_META(MRES_IGNORED);
}
bool checkActivation()
{
	return true;
}


size_t ParseLines(std::vector<char*>& tvector, char* tfilename)
{

	FILE* fp;
	fp = fopen(tfilename, "rt");

	if (!fp)
	{
		return 0;
	}

	char buf[256];
	size_t totalitem = 0;
	while (!feof(fp))
	{
		if (!fgets(buf, sizeof(buf) - 1, fp))
			break;
		if (buf[0] == ';') continue;
		TrimSpace(buf);
		int slen = strlen(buf);
		if (slen <= 1) continue;
		char* buf2 = new char[256];
		strcpy(buf2, buf);
		tvector.push_back(buf2);
	}
	fclose(fp);
	return totalitem;
}
size_t ParseBlocks(std::vector<PacketList>& tvector, char* tfilename)
{

	FILE* fp;
	fp = fopen(tfilename, "rt");

	if (!fp)
	{
		return 0;
	}

	char buf[256];
	size_t totalitem = 0;
	while (!feof(fp))
	{
		if (!fgets(buf, sizeof(buf) - 1, fp))
			break;
		if (buf[0] == ';') continue;
		PacketList pl;
		pl.startswith = false;
		pl.chrlen = 0;
		TrimSpace(buf);
		int slen = strlen(buf);
		if (slen <= 1) continue;
		if (buf[slen - 1] == '+')
		{
			buf[slen - 1] = '\0';
			slen--;
			pl.startswith = true;
		}
		char* token = strtok(buf, " ");
		int totallen = 0;

		while (token)
		{

			pl.chr[totallen] = (unsigned char)strtoul(token, NULL, 16);
			totallen++;
			if (totallen >= 100)
			{
				break;
			}
			token = strtok(NULL, " ");
		}
		pl.chrlen = totallen;
		if (totallen <= 0) continue;
		totalitem++;
		tvector.push_back(pl);
	}
	fclose(fp);
	return totalitem;
}
void TrimSpace(char* pneedle)
{
	char* phaystack = pneedle;
	char* pbuf = pneedle;

	while (IsCharSpecial(*pbuf))
		++pbuf;

	while (*pbuf)
		*phaystack++ = *pbuf++;

	*phaystack = '\0';

	while (phaystack > pneedle && *--phaystack && IsCharSpecial(*phaystack))
		*phaystack = '\0';
}
static inline int IsCharSpecial(char j)
{
	return (j == ' ' || j == '"' || j == ';' || j == '\t' || j == '\r' || j == '\n');
}



static inline int clamp(int value, int _min, int _max)
{
	return value < _min ? _min : (value > _max ? _max : value);
}
static inline float clamp(double value, float _min, float _max)
{
	return value < _min ? _min : (value > _max ? _max : value);
}
int parse_settings(const char* setting, const char* value, TypeRead iType)
{
	return 1;
}
void print_settings()
{
	printf("\n-->%s%s\n", PREFIX, MySettings->GetText("COMMANDS"));
	printf("->info: %s\n", MySettings->GetText("CMD_INFO"));
	printf("->list: %s\n", MySettings->GetText("CMD_LIST"));
	printf("->clear: %s\n", MySettings->GetText("CMD_CLEAR"));
	printf("->remove index|ip:%s\n", MySettings->GetText("CMD_REMOVE"));
}
static int parse_config()
{
	bannedpacket->delete_all();
	allowedpacket->delete_all();
	controlpacket->delete_all();
	bannedpacket->i_size = 0;
	allowedpacket->i_size = 0;
	char* bannedloc = new char[255];
	char* allowedloc = new char[255];
	char* controlloc = new char[255];

	sprintf(bannedloc, "%s%s.ini", mPluginPath, "bannedpackets");
	sprintf(allowedloc, "%s%s.ini", mPluginPath, "allowedpackets");
	sprintf(controlloc, "%s%s.ini", mPluginPath, "controlpackets");

	//bannedpacket->bList = std::vector<PacketList>();
	//allowedpacket->bList = std::vector<PacketList>();

	bannedpacket->i_size = ParseBlocks(bannedpacket->bList, bannedloc);
	allowedpacket->i_size = ParseBlocks(allowedpacket->bList, allowedloc);
	controlpacket->i_size = ParseBlocks(controlpacket->bList, controlloc);
	char* allowFN = new char[256];
	sprintf(allowFN, "%s%s", mPluginPath, "allowedip.ini");
	ParseLines(allowedIp, allowFN);

	printf("\n%s", MySettings->GetTextFmt("CMD_INFO_ALLOWED_IPCOUNT", allowedIp.size()));
	printf("\n%s", MySettings->GetTextFmt("CMD_INFO_LOADED_BANNED", bannedpacket->bList.size()));
	printf("\n%s", MySettings->GetTextFmt("CMD_INFO_LOADED_ALLOWED", allowedpacket->bList.size()));
	printf("\n%s\n", MySettings->GetTextFmt("CMD_INFO_LOADED_CHECKS", controlpacket->bList.size()));





	return 1;

}
int load_config_maps()
{
	g_bNotActive = false;
	parse_config();
	return 1;
}
int load_variables()
{
	char* pos;
	strcpy(mConfigPath, GET_PLUGIN_PATH(PLID));

	pos = strrchr(mConfigPath, '/');

	if (pos == NULL || *pos == '\0')
	{
		return 0;
	}

	*(pos + 1) = '\0';

	strncpy(mPluginPath, mConfigPath, sizeof(mPluginPath) - 1);
	strcat(mConfigPath, CONFIG_FILE);
	sprintf(mCfgPath, "%s", "addons/itsavar/itsavar.cfg");
	return 1;
}
int load_config()
{

	//default settings
	parse_config();
	return 1;
}

