// Network_2.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <WinSock2.h>
#include <iostream>
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
	SOCKET udpServer = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpServer == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return -1;
	}

	// 填充sockaddr_in结构
	sockaddr_in sin;
	sin.sin_family = AF_INET; //ipv4
	sin.sin_port = htons(4444);//将本机字节序转为网络字节序
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//绑定到Socket
	int iResult = 0;
	iResult = bind(udpServer, (SOCKADDR *) &sin, sizeof(sin));
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error %u\n", WSAGetLastError());
		closesocket(udpServer);
		WSACleanup();
		return 1;
	}
	
	//接受数据
	char buff[1024];
	string str;
	
	printf("********server********\n");
	printf("Receiving datagrams...\n");
	struct sockaddr_in SenderAddr;
	int SenderAddrSize = sizeof(SenderAddr);
	while (true)
	{
		
		iResult = recvfrom(udpServer,buff, 1024, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);

		if (iResult == SOCKET_ERROR) {
			printf("recvfrom failed with error %d\n", WSAGetLastError());
			closesocket(udpServer);
			WSACleanup();
			return -1;
		}
		else{
			buff[iResult] = '\0';
			printf(" Recv message (%s) : %s \n", inet_ntoa(SenderAddr.sin_addr), buff);
			if (strcmp(buff,"bye")==0)
			{
				printf("Exiting.\n");
				break;
			}
		}
		printf("-------------------------\n");
		//发送数据
		printf("Send message：");
		getline(cin,str);
		for (int i = 0; i < str.length(); i++)
			buff[i] = str[i];
		buff[str.length()] = '\0';

		iResult = sendto(udpServer, buff, strlen(buff), 0, (SOCKADDR *)& SenderAddr, sizeof(SenderAddr));
		if (iResult == SOCKET_ERROR) {
			printf("sendto failed with error: %d\n", WSAGetLastError());
			closesocket(udpServer);
			WSACleanup();
			return 1;
		}
		if (strcmp(buff,"bye")==0)
		{
			printf("Exiting.\n");
			break;
		}
	}
	//关闭socket
	closesocket(udpServer);

	//释放资源退出
	WSACleanup();
	system("pause");
	return 0;
}






/*
2015.5    HT
多线程端口扫描

netstat -an

*/
/*#include "stdafx.h"
#include <iostream>
#include <process.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

// 线程传递的端口参数
typedef struct
{
	unsigned int min_port;
	unsigned int max_port;
}Port;
Port *lpPort = new Port;
// 线程数
int Thread_NUM;
// 互斥量
HANDLE hMutex;

DWORD WINAPI scan(LPVOID lpParameter)
{
	// 获得参数
	Port* port = (Port*)lpParameter;
	SOCKET sockfd = NULL;
	SOCKADDR_IN sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	for (unsigned int i = port->min_port; i <= port->max_port; ++i)
	{
		if (sockfd == NULL)
		{
			// 创建套接字
			sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if (sockfd == INVALID_SOCKET)
			{
				cout << "创建socket失败！" << endl;
				return 0;
			}
		}
		sin.sin_port = htons(i);
		// 连接服务器
		int ret = connect(sockfd, (sockaddr *)&sin, sizeof(sockaddr));
		// 请求一个互斥量的访问权
		WaitForSingleObject(hMutex, INFINITE);
		if (ret == SOCKET_ERROR)
		{
			cout << "Port: " << i << "    not open !" << endl;
		}
		else
		{
			cout << "Port: " << i << "    open !" << endl;
			closesocket(sockfd);
			sockfd = NULL;
		}
		// 释放一个互斥量的访问权
		ReleaseMutex(hMutex);
	}

	return 0;
}

int main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		cout << "找不到 WinSock dll!" << endl;
		return 0;
	}

	hMutex = CreateMutex(NULL, FALSE, NULL);

	cout << "输入起始端口，结束端口号： ";
	cin >> lpPort->min_port >> lpPort->max_port;
	cout << "输入线程数(小于200)： ";
	cin >> Thread_NUM;
	HANDLE handle[200];
	int Scan_Len = (lpPort->max_port - lpPort->min_port) / Thread_NUM;

	for (int i = 0; i < Thread_NUM; ++i)
	{
		Port *NewPort = new Port;
		NewPort->min_port = lpPort->min_port + Scan_Len * i;
		if (i < (Thread_NUM - 1))
			NewPort->max_port = NewPort->min_port + Scan_Len - 1;
		else
		{
			NewPort->max_port = lpPort->max_port;
			handle[i] = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)scan, NewPort, 0, NULL);
			break;
		}
		handle[i] = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)scan, NewPort, 0, NULL);
	}
	WaitForMultipleObjects(Thread_NUM, handle, TRUE, INFINITE);

	CloseHandle(hMutex);
	system("pause");
	return 0;
}*/