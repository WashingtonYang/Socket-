#pragma once
#include "stdafx.h"
#include <iostream>
#include <process.h>
#include <winsock2.h>
#include <string>
#include<time.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

class Portscan
{
private:
	typedef struct
	{
		int port_start;
		int port_end;
	}Port;
public:
	Portscan();
	~Portscan();
	void scan(char* ip,Port *port);
	bool IsvalidIP(char* ip);
	bool judge(string s);
	char *ip;
	Port *p = new Port;
};

