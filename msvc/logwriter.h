#pragma once
#include <ctime>
#include <fstream>
class logwriter
{
public:
	static char* get_current_daystr();
	static char* get_current_timestr();
	char* logfilefolder;
	void writelog(char* logstring, char*);
	void closelogfile();
	logwriter();
	~logwriter();
private:
	void checklogisopen();
	char* lognamestr;
	FILE* logfile;
};

