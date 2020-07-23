// Network_1.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include <stdio.h>
#include <IPHlpApi.h>
#include <string>
#include <vector>
#include <sstream>
using namespace std;

#pragma comment(lib,"WS2_32")
#pragma comment(lib,"Iphlpapi.lib")

u_char g_ucLocalMac[6];
DWORD  g_dwGatewayIp;
DWORD  g_dwLocalIP;
DWORD g_dwMask;

void getHost();
void getIp(const char* name);


//��_TCHAR *תΪ char*
char* TCHAR2char(const TCHAR* STR)
{
	//�����ַ����ĳ���
	int size = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, FALSE);
	//����һ�����ֽڵ��ַ�������
	char* str = new char[sizeof(char) * size];
	//��STRת��str
	WideCharToMultiByte(CP_ACP, 0, STR, -1, str, size, NULL, FALSE);
	return str;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 1)
	{
		for (int i = 2; i < argc; i++)
		{
			const char * domain = TCHAR2char(argv[i]);
			getIp(domain);
		}
		exit(0);
	}

	int input;
	cout << "-----------------------------------" << endl;
	cout << "����1��ȡ����������Ϣ" << endl;
	cout << "����2ͬʱʵ�ֶԶ����������" << endl;
	cout << "��������ֵ�˳�����" << endl;
	cout << "-----------------------------------" << endl;
	cin >> input;
	if (input == 1)
		getHost();
	else if (input == 2)
	{
		int a = 1;
		string str;
		cout << "�������ӣ�getip www.baidu.com www.163.com" << endl;
		getchar();
		getline(cin, str);
		istringstream tmp(str);
		while (tmp >> str)
		{
			if (a){
				a = 0;
				continue;
			}
			const char * domain = str.c_str();
			getIp(domain);
		}
		system("pause");
	}
	else
		exit(0);
	return 0;
}

void getIp(const char * name)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	struct hostent *host = gethostbyname(name);
	if (!host){
		puts("Get IP address error!");
		//system("pause");
		exit(0);
	}
	//����
	printf("**********************************************\n");
	printf("%s\n",name);
	//ip��ַ
	for (int i = 0; host->h_addr_list[i]; i++){
		printf("IP addr %d: %s\n", i + 1, inet_ntoa(*(struct in_addr*)host->h_addr_list[i]));
	}
	printf("**********************************************\n");
}


void getHost()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;

	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	pAdapterInfo = (PIP_ADAPTER_INFO)malloc(ulOutBufLen);

	dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	if (dwRetVal == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
		if (pAdapterInfo == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			exit(0);
		}
	}

	if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
	{
		cout << "������ϸ��Ϣ:" << endl;
		pAdapter = pAdapterInfo;
		//�������������
		while (pAdapter != NULL)
		{
			
			memcpy(g_ucLocalMac, pAdapter->Address, 6);

			g_dwGatewayIp = inet_addr(pAdapter->GatewayList.IpAddress.String);
			g_dwLocalIP = inet_addr(pAdapter->GatewayList.IpAddress.String);
			g_dwMask = inet_addr(pAdapter->IpAddressList.IpMask.String);
			printf("**********************************************\n");
			printf("Adapter name:  %s\n", pAdapter->AdapterName);
			pAdapter = pAdapter->Next;
			in_addr in;
			in.S_un.S_addr = g_dwLocalIP;
			printf("Ip Address: %s \n", inet_ntoa(in));

			in.S_un.S_addr = g_dwMask;
			printf("Ip Mask: %s \n", inet_ntoa(in));

			in.S_un.S_addr = g_dwGatewayIp;
			printf("GatewayIp: %s \n", inet_ntoa(in));

			u_char *p = g_ucLocalMac;
			printf("MAC Address: %02X-%02X-%02X-%02X-%02X-%02X\n", p[0], p[1], p[2], p[3], p[4], p[5]);
			printf("**********************************************\n\n");
		}
	}
	if (pAdapterInfo) free(pAdapterInfo);
}

