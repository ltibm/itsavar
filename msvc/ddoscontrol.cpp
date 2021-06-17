#include "ddoscontrol.h"
#include <vector>
#include <ctime>


ddoscontrol::ddoscontrol()
{

}


ddoscontrol::~ddoscontrol()
{
}
bool ddoscontrol::is_ipbanned(char * ipaddr)
{
	int vsize = this->ddosiplist.size();
	for (int i = 0; i < vsize; i++)
	{
		ddositem* di = this->ddosiplist[i];
		if (!di) continue;
		if (strcmpi(di->ip_address, ipaddr) == 0)
		{
			return di->is_banned;
		}
	}
	return false;
}
void ddoscontrol::RemoveAt(int index, bool flagonly = true)
{
	if (index < 0 || index >= this->ddosiplist.size()) return;
	if (flagonly)
	{
		ddositem* di = this->ddosiplist[index];
		if (di)
		{
			di->flagged_removed = true;
		}
	}
	else
	{
		this->ddosiplist.erase(this->ddosiplist.begin() + index);
	}

}
bool ddoscontrol::RemoveByAddr(char* addr, bool flagonly = true)
{
	int id = this->GetIdByAddr(addr);
	if (id < 0) return false;
	this->RemoveAt(id, flagonly);
	return true;
}
int ddoscontrol::GetIdByAddr(const char* addr)
{
	int vsize = this->ddosiplist.size();
	for (int i = 0; i < vsize; i++)
	{
		ddositem* di = this->ddosiplist[i];
		if (strcmpi(di->ip_address, addr) == 0)
		{
			return i;
		}
	}
	return NULL;

}
void ddoscontrol::clear_unactionip()
{
	int vsize = this->ddosiplist.size();
	time_t now = std::time(0);
	for (int i = 0; i < vsize; i++)
	{
		ddositem* di = this->ddosiplist[i];
		double itime = 0.0;
		bool silbunu = false;
		if (di->is_banned || di->flagged_removed)
		{
			if (di->flagged_removed) silbunu = true;
			else
			{
				itime = difftime(now, di->bannedtime);
				if (itime >= e_ban_suresi)
				{
					silbunu = true;
				}
			}

		}
		else
		{
			itime = difftime(now, di->lastconnection);
			if (itime >= e_paket_cache_suresi)
			{
				silbunu = true;
			}
		}
		if (silbunu)
		{
			di->delete_alldata();
			this->ddosiplist.erase(this->ddosiplist.begin() + i);
			i--;
			vsize--;
			continue;
		}
		
	}
}
int ddoscontrol::BlockedIPCount()
{
	int total = 0;
	for (size_t i = 0; i < this->ddosiplist.size(); i++)
	{
		ddositem* di = this->ddosiplist[i];
		if (!di || di->flagged_removed || !di->is_banned) continue;
		total++;
	}
	return total;
}

void ddoscontrol::AddIPBan(char* ipaddr)
{
	ddositem* existsitem = this->get_itembyip(ipaddr);
	time_t now = std::time(0);
	if (existsitem)
	{
		existsitem->flagged_removed = false;
		existsitem->is_banned = true;
		existsitem->lastconnection = now;
		existsitem->bannedtime = now;
		return;
	}
	ddositem* cur = new ddositem();
	cur->ip_address = ipaddr;
	cur->bannedtime = now;
	cur->is_banned = true;
	cur->lastconnection = now;
	this->ddosiplist.push_back(cur);
}
bannedreason ddoscontrol::get_banstatus(char * ipaddr, unsigned char * messages, size_t messagelength)
{
	ddositem* di = get_itembyip(ipaddr);
	time_t now = std::time(0);
	int ikonum = 0;
	double itime = 0.0;
	if (!di)
	{
		di = new ddositem();
		di->ip_address = new char[25];
		strcpy(di->ip_address, ipaddr);
		di->lastconnection = now;
		di->total_ddoscount = 1;
		di->is_banned = false;
		di->add_request(messages, messagelength);
		this->ddosiplist.push_back(di);
		return nobanned;
	}
	if (di->flagged_removed) return nobanned;
	if (di->is_banned)
	{
		itime = difftime(now, di->bannedtime);
		if (itime >= e_ban_suresi)
		{
			return nobanned;
		}
	}
	else
	{
		itime = difftime(now, di->lastconnection);
		if (itime >= e_paket_cache_suresi)
		{
			return nobanned;
		}
		else
		{
			
			di->total_ddoscount++;
			if (check_total_flood && di->total_ddoscount >= e_toplam_limit)
			{
				di->bannedtime = now;
				di->is_banned = true;
				di->total_ddoscount = 0;
				di->delete_alldata();
				return banned_allpacket;
			}
			else
			{
				if (check_single_flood)
				{
					int ibytindex = di->request_index(messages, messagelength);
					//printf("\n bytindex:%i, len: %i\n", ibytindex, messagelength);
					if (ibytindex == -1)
					{
						di->add_request(messages, messagelength);
					}
					else
					{
						int itotal = di->request_count[ibytindex];
						itotal++;
						if (itotal >= e_teksefer_limit)
						{
							di->bannedtime = now;
							di->is_banned = true;
							di->total_ddoscount = 0;
							di->delete_alldata();
							return banned_differpacket;
						}
						di->request_count[ibytindex] = itotal;
					}
				}

			}

		}
	}
	return nobanned;
}
ddositem* ddoscontrol::get_itembyip(char * ipaddr)
{
	int vsize = this->ddosiplist.size();
	for (int i = 0; i < vsize; i++)
	{
		ddositem* di = this->ddosiplist[i];
		if (strcmpi(di->ip_address, ipaddr) == 0)
		{
			return di;
		}
	}
	return NULL;
}
void ddoscontrol::Clear(bool flagonly = true)
{
	if (flagonly)
	{
		for (size_t i = 0; i < this->ddosiplist.size(); i++)
		{
			ddositem* di = this->ddosiplist[0];
			if (!di) continue;
			di->flagged_removed = true;
		}
	}
	else
	{
		this->ddosiplist.clear();
	}
}
