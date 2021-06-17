#pragma once
#include "ddositem.h"
enum bannedreason
{
	nobanned = 0,
	banned_allpacket = 1,
	banned_differpacket = 2,
};
class ddoscontrol
{

public:
	double e_ban_suresi = 200;
	double e_paket_cache_suresi = 3;
	double e_teksefer_limit = 22;
	int e_toplam_limit = 220;
	bool check_total_flood = true;
	bool check_single_flood = true;
	ddoscontrol();
	~ddoscontrol();
	bool is_ipbanned(char* ipaddr);
	void clear_unactionip();
	bannedreason get_banstatus(char* ipaddr, unsigned char* messages, size_t messagelength);
	ddositem* get_itembyip(char* ipaddr);
	std::vector<ddositem*> ddosiplist;
	void AddIPBan(char*);
	void RemoveAt(int, bool);
	bool RemoveByAddr(char*, bool);
	int GetIdByAddr(const char*);
	int BlockedIPCount();
	void Clear(bool);
	
};

