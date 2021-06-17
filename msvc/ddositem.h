#pragma once
#include <vector>
#include <ctime>
class ddositem
{

public:
	struct VChar {
		unsigned char chre[255];
	};
	char* ip_address;
	std::vector<VChar> request_list;
	std::vector<int> request_length;
	std::vector<int> request_count;
	int request_index(unsigned char* request_list, int request_length);
	void add_request(unsigned char* request_list, int request_length);
	void delete_alldata();
	time_t lastconnection;
	time_t bannedtime;
	int total_ddoscount;
	bool is_banned;
	bool flagged_removed;
	ddositem();
	~ddositem();
};

