
#pragma once

#include <iostream>
#include <cstdlib>
#include <string>
#include <windows.h>
#include <ctime>

using namespace std;

///////////////////////////////////////////////////////////////////////
//控制台颜色设置
HANDLE hConsole;

void print(string out)
{
	int color;
	for(unsigned i = 0; i < out.length(); i++)
	{
		color = rand() % 11 + 4;//随机颜色
		if(color == 7 || color == 8)
			color = 10;
	    SetConsoleTextAttribute(hConsole, color) ;
		cout<<out[i];
	}
}

void println(string out)
{
	print(out);
	cout<<endl;
}

inline void startMyPrinting()
{
	srand(static_cast<unsigned int>(time(0)));//随机数种子
	hConsole = GetStdHandle((STD_OUTPUT_HANDLE)) ;
}

inline void setPrintColor(int color)
{
	hConsole = GetStdHandle((STD_OUTPUT_HANDLE)) ;
	SetConsoleTextAttribute(hConsole, color) ;
}

///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//字符串处理

inline string LTrim(const string& str) 
{ 
	int index = str.find_first_not_of(" \n\r\t");
	if(index == -1)
		return str;
	return str.substr(index); 
} 

inline string RTrim(const string& str) 
{ 
	return str.substr(0, str.find_last_not_of(" \n\r\t") + 1); 
} 

inline string Trim(const string& str) 
{
	return LTrim(RTrim(str)); 
} 

///////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////
//时间处理

string stroftime(const struct tm * ptm)
{
	char chtime[30];
	memset(chtime, 0, sizeof(chtime));
	char *chtemp = new char[10];
	itoa(ptm->tm_year + 1900, chtemp, 10);
	strcat(chtime, chtemp);
	strcat(chtime, "/");
	itoa(ptm->tm_mon + 1, chtemp, 10);
	if(strlen(chtemp) == 1)
		strcat(chtime, "0");
	strcat(chtime, chtemp);
	strcat(chtime, "/");
	itoa(ptm->tm_mday, chtemp, 10);
	if(strlen(chtemp) == 1)
		strcat(chtime, "0");
	strcat(chtime, chtemp);
	
	strcat(chtime, "  ");
	itoa(ptm->tm_hour, chtemp, 10);
	if(strlen(chtemp) ==1)
		strcat(chtime, "0");
	strcat(chtime, chtemp);
	strcat(chtime, ":");
	itoa(ptm->tm_min, chtemp, 10);
	if(strlen(chtemp) == 1)
		strcat(chtime, "0");
	strcat(chtime, chtemp);
	string strtime = string(chtime);
	return strtime;
}

///////////////////////////////////////////////////////////////////////
