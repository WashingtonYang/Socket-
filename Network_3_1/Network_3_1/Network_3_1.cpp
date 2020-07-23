// Network_3_1.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Portscan.h"
#include <string>
#include <stdio.h>
#include <time.h>
#include <iostream>
using namespace std;

typedef struct
{
	int port_start;
	int port_end;
}Port;

char* TCHAR2char(const TCHAR* STR)
{
	//返回字符串的长度
	int size = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, FALSE);
	//申请一个多字节的字符串变量
	char* str = new char[sizeof(char) * size];
	//将STR转成str
	WideCharToMultiByte(CP_ACP, 0, STR, -1, str, size, NULL, FALSE);
	return str;
}

int _tmain(int argc, _TCHAR* argv[])
{
	cout << argv[1] << endl;
	string msg, tempIP;
	char ip[100];
	Port *p = new Port;
	const char *t;
	if (argc != 1)
	{
		Portscan scan;
		tempIP = TCHAR2char(argv[2]);
		for (int i = 0; i < tempIP.length(); i++)
		{
			*(ip + i) = tempIP[i];
		}
		ip[tempIP.length()] = '\0';
		msg = TCHAR2char(argv[3]);
		p->port_start= atoi(msg.c_str());
		for (t = msg.c_str(); *(t++) != '-';);
			p->port_end = atoi(t);
		cout << ip << p->port_start<<p->port_end<<endl;
		scan.p->port_start = p->port_start;
		scan.p->port_end = p->port_end;
		scan.scan(ip, scan.p);
	}
	else
	{
		cout << "usemethod";
		cout << "Network_3_1.exe scan xxx.xxx.xxx.xxx 1-65535" << endl;
	}
	//system("pause");
	return 0;
}

