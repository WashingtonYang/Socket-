// NetWork5_Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include<Winsock2.h>
#include<string>
#include<iostream>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsaData;
	int reVal;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		printf("WSAStartup error\n");
	}
	SOCKET sockClient = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//���ػ�·��ַ127������һ̨�����ϲ��Ե�IP
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4444);//�ͷ������˵Ķ˿ںű���һ��
	if (!connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))//���ӷ������ˣ��׽��֣���ַת�������ȣ�
	{
		printf("���ӷ������ɹ�\n");
	}
	else
	{
		printf("����������ʧ��\n");
		return 0;
	}
	while (true)
	{
		cout << "Send message:" ;
		CHAR buff[8192];
		string str;
		getline(cin, str);
		for (int i = 0; i < str.length(); i++)
			buff[i] = str[i];
		buff[str.length()] = '\0';
		send(sockClient, buff, strlen(buff), 0);
		if (strcmp(buff, "bye") == 0)
		{
			printf("�����˳���\n");
			closesocket(sockClient);
			break;
		}
		reVal=recv(sockClient, buff, 8192, 0);
		buff[reVal] = '\0';
		cout << buff << endl;
		Sleep(1);
	}
	system("pause");
	WSACleanup();
	return 0;
}

