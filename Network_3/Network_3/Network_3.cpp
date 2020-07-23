// Network_3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdio.h>
#include<iostream>
#include<WinSock2.h>
#include <string>
#include<time.h>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI ThreadProc(LPVOID lpParam);
void scan(char* ip);
bool IsvalidIP(char* ip);
bool judge(string s);

int PortEnd, PortStart;

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
	string msg,tempIP;
	char ip[100],*p=NULL;
	const char *t;
	
	printf("*********Port Scan*********\n");
	if (argc != 1)
	{
		tempIP = TCHAR2char(argv[2]);
		for (int i = 0; i < tempIP.length(); i++)
		{
			*(ip + i) = tempIP[i];
		}
		ip[tempIP.length()] = '\0';
		msg = TCHAR2char(argv[3]);
		PortStart = atoi(msg.c_str());
		for (t = msg.c_str(); *(t++) != '-';);
		PortEnd = atoi(t);
		scan(ip);
		exit(0);
	}
	printf("please input Ip and port\n");
	printf("example:scan 127.0.0.1 1-65535\n");
	printf("input:");
	getline(cin, msg);
	int num = (int)msg.find_first_of(" ");
	msg = msg.substr(num+1, msg.length() - num);
	num = (int)msg.find_first_of(" ");
	tempIP = msg.substr(0, num);
	msg = msg.substr(num + 1, msg.length() - num);
	for (int i = 0; i < tempIP.length(); i++)
	{
		*(ip + i) = tempIP[i];
	}
	ip[tempIP.length()] = '\0';
	PortStart = atoi(msg.c_str());
	for (t = msg.c_str(); *(t++) != '-';);
		PortEnd = atoi(t);

	scan(ip);
	system("pause");
	return 0;
}

void scan(char* ip)
{
	if (PortStart < 1 || PortEnd>65535){
		printf("port error:port belong to 1-65535\n");
		exit(0);
	}
	if (!IsvalidIP(ip)){
		printf("IP error:ip should xxx.xxx.xxx.xxx\n");
		exit(0);
	}

	WSAData wsaDate;
	
	if (WSAStartup(MAKEWORD(2, 2), &wsaDate) != 0)
	{
		printf("WSAStartup failed\n");
		exit(0);
	}

	// 填target
	sockaddr_in sin;
	sin.sin_family = AF_INET; //ipv4
	//sin.sin_port = htons(4444);//将本机字节序转为网络字节序
	sin.sin_addr.S_un.S_addr = inet_addr(ip);

	clock_t TimeStart, TimeEnd;
	HANDLE hThread;
	DWORD dwThreadID;
	TimeStart = clock();
	for (int i = PortStart; i <= PortEnd; i++)
	{
		sin.sin_port = htons(i);
		//创建进程接受数据
		hThread = CreateThread(NULL, 0, ThreadProc, (LPVOID)&sin, 0, &dwThreadID);
		Sleep(10);//主进程等待时间，让子进程有时间获取端口信息。
		if (hThread == NULL)
		{
			printf("CreateThread() failed: %d\n", GetLastError());
			break;
		}
		CloseHandle(hThread);
	}
	Sleep(40);//等待所有进程完毕
	TimeEnd = clock();
	printf("Time cost:%.2fs\n", (float)(TimeEnd - TimeStart) / CLOCKS_PER_SEC);

	WSACleanup();
}

bool IsvalidIP(char* ip)
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

//判断IP格式
bool judge(string s)
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
	//判断是否是再0-255；
	if (atoi(s.c_str()) < 0 || atoi(s.c_str())>255)
		return false;
	return true;
}

DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	SOCKADDR_IN sin = *(SOCKADDR_IN*) lpParam;
	SOCKET con = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

	//printf("%s:%d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));

	int  iResult = connect(con, (SOCKADDR *)& sin, sizeof(sockaddr));
	if (iResult == SOCKET_ERROR)
	{
		//printf("connect function failed with error: %ld\n", WSAGetLastError());
		closesocket(con);
		return 0;
	}
	printf("%s:%d is open\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
	closesocket(con);
	return 0;
}
//scan 192.168.1.1 1-300