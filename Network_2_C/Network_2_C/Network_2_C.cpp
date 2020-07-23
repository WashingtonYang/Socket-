// Network_2_C.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <string>
#pragma comment(lib,"WS2_32")
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	//初始化winSock；
	WSAData wsaDate;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDate) != 0)
	{
		printf("WSAStartup failed\n");
		return -1;
	}
	//创建UDP套接字
	//socket对象
	SOCKET udpClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpClient == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return -1;
	}

	// 也可以在这里调用bind函数绑定一个本地地址
	// 否则系统将会自动安排

	// 填充sockaddr_in结构
	sockaddr_in sin;
	sin.sin_family = AF_INET; //ipv4
	sin.sin_port = htons(4444);//将本机字节序转为网络字节序
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//发送数据
	int iResult = 0;
	char buff[1024];
	string str;
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	printf("********Client********\n");
	while (true)
	{
		printf("Send message:");
		getline(cin, str);
		for (int i = 0; i < str.length(); i++)
			buff[i] = str[i];
		buff[str.length()] = '\0';
		

		iResult = sendto(udpClient, buff, strlen(buff), 0, (SOCKADDR *)& sin, sizeof(sin));
		if (iResult == SOCKET_ERROR) {
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(udpClient);
			WSACleanup();
			return -1;
		}
		if (strcmp(buff, "bye") == 0)
		{
			printf("Exiting.\n");
			break;
		}
		printf("-------------------------\n");
		printf("Receiving datagrams...\n");
		//接受数据
		iResult = recvfrom(udpClient, buff, 1024, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			printf("recvfrom failed with error %d\n", WSAGetLastError());
			closesocket(udpClient);
			WSACleanup();
			return 1;
		}
		else{
			buff[iResult] = '\0';
			printf(" Recv message（%s）：%s\n", inet_ntoa(SenderAddr.sin_addr), buff);
			if (strcmp(buff, "bye") == 0)
			{
				printf("Exiting.\n");
				break;
			}
		}
	}//关闭socket
	closesocket(udpClient);

	//释放资源退出
	WSACleanup();
	system("pause");
	return 0;
}

