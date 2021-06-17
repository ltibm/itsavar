
#include "packetcontroller.h"


bool packetcontroller::is_match(unsigned char * messages, size_t packetlen)
{
	if (packetlen <= 0) return false;
	int sz = this->bList.size();
	size_t rlength = packetlen;
	for (int i = 0; i < sz; i++)
	{
		PacketList plist = bList[i];
		if (plist.chrlen > packetlen) continue;
		if (packetlen > plist.chrlen)
		{
			if (!plist.startswith) continue;
		}
		bool found = true;
		for (int j = 0; j < plist.chrlen; j++)
		{
			if (messages[j] != plist.chr[j])
			{
				found = false;
				break;
			}
		}
		if (found)
		{
			return true;
		}

	}
	return false;
}
void packetcontroller::delete_all()
{
	bList = std::vector<PacketList>();
	bList.clear();
}

packetcontroller::packetcontroller()
{
	
}


packetcontroller::~packetcontroller()
{
	
}
