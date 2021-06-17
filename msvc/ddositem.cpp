#include "ddositem.h"



int ddositem::request_index(unsigned char * request_list, int request_length)
{
	if (request_length <= 0) return -1;
	int sz = this->request_list.size();

	int rlength = request_length;
	if (rlength > 25) rlength = 25;
	for (int i = 0; i < sz; i++)
	{
		bool found = true;
		if (this->request_length[i] != rlength) continue;
		for (int j = 0; j < rlength; j++)
		{
			if (request_list[j] !=  this->request_list[i].chre[j])
			{
				found = false;
				break;
			}
		}
		if (found) return i;

	}
	return -1;
}

void ddositem::add_request(unsigned char * request_list, int request_length)
{
	if (request_length <= 0) return;
	size_t rls = this->request_length.size();
	if (rls >= 25)
	{
		for (size_t i = 0; i < 5; i++)
		{
			this->request_length.erase(this->request_length.begin());
			this->request_list.erase(this->request_list.begin());
			this->request_count.erase(this->request_count.begin());
		}
	}
	VChar vc;
	int ilen = request_length;
	if (ilen > 25)
	{
		ilen = 25;
	}
	memcpy(vc.chre, request_list, ilen);
	this->request_length.push_back(ilen);
	this->request_list.push_back(vc);
	this->request_count.push_back(1);
}

void ddositem::delete_alldata()
{
	this->request_length.clear();
	this->request_list.clear();
	this->request_count.clear();
}

ddositem::ddositem()
{
	this->flagged_removed = false;
}


ddositem::~ddositem()
{
	this->delete_alldata();
}


