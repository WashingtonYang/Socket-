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
		printf("服务器监听中......\n");
		HANDLE hTread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL);
	}
}


bool Serve::sendFile()
{
	//创建套接字
	ftpServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ftpServer == INVALID_SOCKET)
	{
		cout << "socket () error" << WSAGetLastError() << endl;
		closesocket(ftpServer);
		WSACleanup();
		return false;
	}
	// 填充sockaddr_in
	sockaddr_in sAddr;
	sAddr.sin_family = AF_INET; // 协议簇为IPV4的  
	sAddr.sin_port = htons(4444);// 端口  因为本机是小端模式，网络是大端模式，调用htons把本机字节序转为网络字节序 
	sAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	// 绑定套接字到这个本地地址
	int retVal;
	// bind(socket描述字， 绑定给listenfd的协议地址，地址长度)
	retVal = bind(ftpServer, (sockaddr *)&sAddr, sizeof(sAddr));
	if (retVal == SOCKET_ERROR)
	{
		cout << "bind () error" << WSAGetLastError() << endl;
		closesocket(ftpServer);
		WSACleanup();
		return false;
	}
	//开启监听
	if (listen(ftpServer, 5) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ftpServer);
		WSACleanup();
		return 1;
	}//等待队列最大长度为5

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
		
		printf("---等待新的连接---\n");

		SOCKET sockAccept = accept(ftpServer, (SOCKADDR FAR*)&from, &len);
		
		if (sockAccept == INVALID_SOCKET && flag==1)
		{
			printf("accept error:%d\n", WSAGetLastError());
		}
		else if (flag)
		{
			flag = 0;
			printf("[%s]连接\n", inet_ntoa(from.sin_addr));
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
			printf("程序退出！\n");
			closesocket(sockAccept);
			break;
		}
		printf("获取%s文件中...\n", buf);
		const int bufferSize = 1024;
		char buffer[bufferSize];
		int readLen = 0;
		int havesend = 0;
		ifstream File;
		File.open(buf, ios::binary);
		if (!File)
		{
			cout << "文件打开失败" << endl;
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