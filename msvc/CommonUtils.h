#pragma once
#include <ctype.h>
class Utils
{
};
bool IsAllNumeric(const char* text)
{
	for (char c = *text; c; c = *++text) {
		if (!isdigit(c)) return false;
	}
	return true;
}
char* TimeToString(time_t time, char* fmt)
{
	tm* tm = localtime(&time);
	char* tf = new char[50];
	strftime(tf, 49, "%d.%m.%Y %H:%M:%S", tm);
	return tf;
}
char* TimeToString(const time_t* time, char* fmt)
{	
	tm* tm = localtime(time);
	char* tf = new char[50];
	strftime(tf, 49, "%d.%m.%Y %H:%M:%S", tm);
	return tf;
}

