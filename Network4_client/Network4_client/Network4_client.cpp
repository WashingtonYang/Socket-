// Network4_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "client.h"
#include <WinSock2.h>
#include <stdio.h>
#include <atlstr.h>
#include <string>
#include <iostream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	int port;
	string ip;
	
	printf("请输入服务器ip:");
	cin >> ip;

	printf("请输入服务器端口号:");
	cin >> port;
	
	client C;
	C.Connect(ip, port);
	system("pause");
	return 0;
}

