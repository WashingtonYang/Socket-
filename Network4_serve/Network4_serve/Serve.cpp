#include "stdafx.h"
#include "Serve.h"
#include <WinSock2.h>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <iostream>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#undef UNICODE
DWORD WINAPI ThreadProc(LPVOID lpParameter);
SOCKET ftpServer;

Serve::Serve()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup failed\n");
		exit;
	}
}


Serve::~Serve()
{
	WSACleanup();
}


void Serve::initServer()
{
	if (sendFile())
	{
		printf("������������......\n");
		HANDLE hTread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
	}
}


bool Serve::sendFile()
{
	//�����׽���
	ftpServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ftpServer == INVALID_SOCKET)
	{
		cout << "socket () error" << WSAGetLastError() << endl;
		closesocket(ftpServer);
		WSACleanup();
		return false;
	}
	// ���sockaddr_in
	sockaddr_in sAddr;
	sAddr.sin_family = AF_INET; // Э���ΪIPV4��  
	sAddr.sin_port = htons(4444);// �˿�  ��Ϊ������С��ģʽ�������Ǵ��ģʽ������htons�ѱ����ֽ���תΪ�����ֽ��� 
	sAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// ���׽��ֵ�������ص�ַ
	int retVal;
	// bind(socket�����֣� �󶨸�listenfd��Э���ַ����ַ����)
	retVal = bind(ftpServer, (sockaddr *)&sAddr, sizeof(sAddr));
	if (retVal == SOCKET_ERROR)
	{
		cout << "bind () error" << WSAGetLastError() << endl;
		closesocket(ftpServer);
		WSACleanup();
		return false;
	}
	//��������
	if (listen(ftpServer, 5) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ftpServer);
		WSACleanup();
		return 1;
	}//�ȴ�������󳤶�Ϊ5

	return true;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	SOCKADDR_IN from;
	int len = sizeof(SOCKADDR);
	int flag = 1;
	char buf[1024];
	while (true)
	{
		
		printf("---�ȴ��µ�����---\n");

		SOCKET sockAccept = accept(ftpServer, (SOCKADDR FAR*)&from, &len);
		
		if (sockAccept == INVALID_SOCKET && flag==1)
		{
			printf("accept error:%d\n", WSAGetLastError());
		}
		else if (flag)
		{
			flag = 0;
			printf("[%s]����\n", inet_ntoa(from.sin_addr));
		}
		int retVal = recv(sockAccept, buf, sizeof(buf)+1, NULL);
		if (SOCKET_ERROR == retVal)
		{
			printf("recv failed \n");
			closesocket(sockAccept);
			closesocket(ftpServer);
		}
		buf[retVal] = '\0';
		if (strcmp(buf, "bye") == 0)
		{
			printf("�����˳���\n");
			closesocket(sockAccept);
			break;
		}
		printf("��ȡ%s�ļ���...\n", buf);
		const int bufferSize = 1024;
		char buffer[bufferSize];
		int readLen = 0;
		int havesend = 0;
		ifstream File;
		File.open(buf, ios::binary);
		if (!File)
		{
			cout << "�ļ���ʧ��" << endl;
		}	
		else
		{
			while (!File.eof())
			{
				File.read(buffer, bufferSize);
				readLen = File.gcount();
				send(sockAccept, buffer, readLen, 0);
				havesend += readLen;
			}
			File.close();
			cout << "send:" << havesend << endl;
		}
		closesocket(sockAccept);
	}
	closesocket(ftpServer);
	return 0;
}