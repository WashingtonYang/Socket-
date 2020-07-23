#include "stdafx.h"
#include "client.h"
#include <WinSock2.h>
#include <stdio.h>
#include <atlstr.h>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#undef UNICODE

WSADATA wsa;
client::client()
{
	
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("WSAStartup failed\n");
		exit;
	}
}


client::~client()
{
	WSACleanup();
}

void client::Connect(string ip,int port)
{
	
	unsigned long long file_size = 0;
	char buff[1024];
	
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
	addrSrv.sin_port = ntohs(port);
	addrSrv.sin_family = AF_INET;
	getchar();
	while (true)
	{
		SOCKET sockClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (!connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))
		{  
			printf("连接服务器成功\n");
			printf("请输入获取文件的路径:");
			
			
			string str;

			getline(cin, str);

			for (int i = 0; i < str.length(); i++)
				buff[i] = str[i];
			buff[str.length()] = '\0';
			int retVal = send(sockClient, buff, strlen(buff), 0);
			if (retVal == SOCKET_ERROR)
			{
				printf("send error\n");
				closesocket(sockClient);
				exit;
			}
			if (strcmp(buff, "bye") == 0)
			{
				printf("程序退出！\n");
				closesocket(sockClient);
				break;
			}
			string a = "D:\\\\";
			cout << str << "------------------" << endl;
			a = a + str.substr(str.find_last_of('\\') + 1, str.length());
			cout << "文件下载到" << a << endl;

			ofstream File;
			const int bufferSize = 1024;
			char buffer[bufferSize];
			int readLen = 0;
			int haverecv = 0;
			File.open(a.c_str(), ios::binary);
			if (!File)
			{
				printf("文件打开失败\n");
			}
			do
			{
				readLen = recv(sockClient, buffer, bufferSize, 0);
				haverecv += readLen;
				if (readLen == 0)
				{
					break;
				}
				else
				{
					File.write(buffer, readLen);
				}
			} while (true);
			File.close();
			printf("文件接受完毕---->%d\n", haverecv);
			closesocket(sockClient);
			
			Sleep(10);
		}
		else
		{
			printf("连接服务器失败\n");
			break;
			closesocket(sockClient);
		}
	}
}

