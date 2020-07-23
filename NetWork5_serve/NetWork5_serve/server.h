#include <winsock2.h>
#include<iostream>
#include <string>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#pragma once
class server
{
public:
	SOCKET Listen;
	SOCKET Accept;
	SYSTEM_INFO SystemInfo;
	int workThread;
	HANDLE CompletionPort;
	DWORD ThreadID;
	sockaddr_in sAddr;
	int retVal;
	server();
	~server();
	int init();
};

