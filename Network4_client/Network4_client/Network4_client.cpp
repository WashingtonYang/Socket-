// Network4_client.cpp : �������̨Ӧ�ó������ڵ㡣
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
	
	printf("�����������ip:");
	cin >> ip;

	printf("������������˿ں�:");
	cin >> port;
	
	client C;
	C.Connect(ip, port);
	system("pause");
	return 0;
}

