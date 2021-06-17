#include "logwriter.h"
#include <string>

char * logwriter::get_current_daystr()
{
	std::time_t etime = std::time(0);
	tm* t = localtime(&etime);
	char* chr = new char[50];
	//sprintf(chr, "%02i-%02i-%04i", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900);

	sprintf(chr, "%04i-%02i-%02i", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
	return chr;
}



char * logwriter::get_current_timestr()
{
	std::time_t etime = std::time(0);
	tm* t = localtime(&etime);
	char* chr = new char[50];
	sprintf(chr, "%02i.%02i.%04i %02i:%02i:%02i", t->tm_mday, t->tm_mon + 1, t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
	return chr;
}



void logwriter::writelog(char * logstring, char * timetext)
{
	checklogisopen();
	if (!logfile) return;
	//char* currentdatetime = get_current_timestr();
	//char* currentdatetime = TimeToString(std::time(0), timeformat);
	fprintf(logfile, "%s --> %s\n", timetext, logstring);
	fflush(logfile);
}

void logwriter::closelogfile()
{
	if (logfile)
	{
		fflush(logfile);
		fclose(logfile);
	}
}

logwriter::logwriter()
{
	logfilefolder = new char[100];
	lognamestr = new char[255];
	logfile = nullptr;
}


logwriter::~logwriter()
{
}

void logwriter::checklogisopen()
{
	char* curretndate = get_current_daystr();
	if (logfile)
	{
		if (lognamestr != NULL && strcmp(lognamestr, curretndate) != 0)
		{
			fflush(logfile);
			fclose(logfile);
			logfile = NULL;
		}
	}		
	if (!logfile)
	{
		
		char* filename = new char[255];
		sprintf(filename, "%s%s_itsavar.log", logfilefolder, curretndate);
		lognamestr = new char[255];
		strcpy(lognamestr, filename);
		logfile = fopen(filename, "at");
	}
}
 