#include "stdafx.h"
#include "Portscan.h"

#include <iostream>
#include <process.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

typedef struct
{
	int portstart;
	int portend;
}P;
char* IP;
HANDLE hMutex;//互斥访问量
DWORD WINAPI ThreadProc(LPVOID lpParam);

Portscan::Portscan()
{
	cout << "*****Port Scan*****" << endl;
	cout << "Port scan initialize......" << endl;
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "WSAStartup failed" << endl;
		exit(0);
	}
}


Portscan::~Portscan()
{
	WSACleanup();
}

bool Portscan::IsvalidIP(char *ip)
{
	string sip = ip;
	//查找有没有点
	int pos = (int)sip.find_first_of(".");
	if (pos == 0)
		return false;
	//分割地址
	string s1 = sip.substr(0, pos);
	sip = sip.substr(pos + 1, sip.length() - pos);
	if (!judge(s1))
		return false;

	//S2
	pos = (int)sip.find_first_of(".");
	if (pos == 0)
		return false;
	string s2 = sip.substr(0, pos);
	sip = sip.substr(pos + 1, sip.length() - pos);
	if (!judge(s2))
		return false;

	//S3
	pos = (int)sip.find_first_of(".");
	if (pos == 0)
		return false;
	string s3 = sip.substr(0, pos);
	sip = sip.substr(pos + 1, sip.length() - pos);
	if (!judge(s3))
		return false;

	//S4
	pos = (int)sip.find_first_of(".");
	if (pos == 0)
		return false;
	string s4 = sip.substr(0, pos);
	sip = sip.substr(pos + 1, sip.length() - pos);
	if (!judge(s4))
		return false;
	return true;
}

bool Portscan::judge(string s)
{
	if (s.length() > 3)
		return false;
	//判断是否全是数字
	for (int i = 0; i < s.length(); i++)
	{
		int c = s.c_str()[i];
		if (!isdigit(c))
			return false;
	}
	//判断是否是再1-255；
	if (atoi(s.c_str()) < 1 || atoi(s.c_str())>255)
		return false;
	return true;
}



void Portscan::scan(char* ip, Port *port)
{
	IP = ip;
	HANDLE handle[200];//创建线程句柄
	int thread_num = 50;
	int scan_len = (port->port_end - port->port_start) / thread_num;
	
	for (int i = 0; i < thread_num; i++)
	{
		P *NP = new P;
		NP->portstart = scan_len*i;
		if (i<thread_num-1)
		{
			NP->portend = NP->portstart + scan_len-1;
		}
		else
		{
			NP->portend = port->port_end;
		}
		handle[i] = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc, NP, 0, NULL);
	}
	Sleep(40);//等待所有进程完毕
	WaitForMultipleObjects(thread_num, handle, TRUE, INFINITE);
	CloseHandle(hMutex);
}


DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	P * port = (P*)lpParam;
	SOCKET con = NULL;
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.S_un.S_addr = inet_addr(IP);
	for (int i = port->portstart; i <= port->portend; i++)
	{
		if (con==NULL)
		{
			con = socket(AF_INET, SOCK_STREAM, 0);
			if (con == INVALID_SOCKET)
			{
				cout << "creat socket faild！" << endl;
				return 0;
			}
		}
		sin.sin_port = htons(i);
		// 请求一个互斥量的访问权
		WaitForSingleObject(hMutex, 5000);

		int re = connect(con, (sockaddr *)&sin, sizeof(sockaddr));
		if (!(re==SOCKET_ERROR))
		{
			cout << IP << ":" << i << " is opened" << endl;
			closesocket(con);
			con = NULL;
		}
		else
		{
			cout << IP << ":" << i << " no opened" << endl;
		}
		// 释放一个互斥量的访问权
		ReleaseMutex(hMutex);
	}
	return 0;
}