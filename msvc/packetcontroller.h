#pragma once
#include <vector>
struct PacketList
{
	unsigned char chr[100];
	int chrlen;
	bool startswith;
};
class packetcontroller
{

public:

	std::vector<PacketList> bList;
	bool is_match(unsigned char* messages, size_t packetlen);
	size_t i_size = 0;
	void delete_all();
	packetcontroller();
	~packetcontroller();
};

