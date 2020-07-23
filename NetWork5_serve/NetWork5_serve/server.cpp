#include "stdafx.h"
#include "server.h"
#include <winsock2.h>
#include<iostream>
#include <string>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
DWORD WINAPI work(LPVOID PortID);

typedef struct
{
	SOCKET Socket;
}*LPPER_HANDLE_DATA, PER_HANDLE_DATA;

typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	CHAR Buffer[8192];
}PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

server::server()
{
	WSADATA wsaData;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		printf("WSAStartup error\n");
	}
	GetSystemInfo(&SystemInfo);
	workThread = SystemInfo.dwNumberOfProcessors * 2;
}

server::~server()
{
	WSACleanup();
}


int server::init()
{
	//创建完成端口
	if ((CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
	{
		cout << "CreateIoCOmpletionPort failed:" << GetLastError() << endl;
		return -1;
	}
	//创建工作线程
	for (int i = 0; i < workThread; i++)
	{
		HANDLE ThreadHadle;
		ThreadHadle=CreateThread(NULL, 0, work, CompletionPort,0,&ThreadID);
		if(ThreadHadle)
			CloseHandle(ThreadHadle);
	}
	//创建socket
	if ((Listen=WSASocket(AF_INET, SOCK_STREAM,0,NULL,0,WSA_FLAG_OVERLAPPED))==INVALID_SOCKET)
	{
		cout << "WSASocket error:" << GetLastError() << endl;
		return -1;
	}
	//绑定到本地地址
	sAddr.sin_family = AF_INET; // 协议簇为IPV4的  
	sAddr.sin_port = htons(4444);// 端口  因为本机是小端模式，网络是大端模式，调用htons把本机字节序转为网络字节序 
	sAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	retVal = bind(Listen, (PSOCKADDR)&sAddr, sizeof(sAddr));
	if (retVal == SOCKET_ERROR)
	{
		cout << "bind () error" << WSAGetLastError() << endl;
		closesocket(Listen);
		WSACleanup();
		return -1;
	}
	//开启监听
	if (listen(Listen, 5) == SOCKET_ERROR) {
		wprintf(L"listen failed with error: %ld\n", WSAGetLastError());
		closesocket(Listen);
		WSACleanup();
		return -1;
	}//等待队列最大长度为5
	cout << "*********" << endl;
	LPPER_HANDLE_DATA perHandledata;
	LPPER_IO_OPERATION_DATA PerIoData;
	while (true)
	{
		Accept = WSAAccept(Listen, NULL, NULL, NULL, 0);
		//cout << "*********" << endl;
		//Accept = accept(Listen, 0, 0);
		cout << "Socket" << Accept << "已连接" << endl;
		//分配socket 句柄
		perHandledata = new PER_HANDLE_DATA();
		perHandledata->Socket = Accept;
		// 将接入的客户端和完成端口联系起来
		CreateIoCompletionPort((HANDLE)Accept, CompletionPort, (DWORD)perHandledata, 0);

		//为IO分配内存空间操作
		PerIoData = new PER_IO_OPERATION_DATA();
		//初始化I/O操作
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = 8192;
		//接受数据，放到PerIoData
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		WSARecv(Accept, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
	}
	
	return 0;
}


DWORD WINAPI work(LPVOID PortID)
{
	HANDLE CompletionPort = (HANDLE)PortID;//完成端口句柄
	DWORD BytesTransferred;				   //数据传输的字节数
	LPPER_HANDLE_DATA PerHandleData;	   //Socket句柄结构体
	LPPER_IO_OPERATION_DATA PerIoData;	   //i/o操作结构体

	while (true)
	{
		//检查完成端口状态
		//得到IO操作结果
		if (0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE))
		{
			if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED))
			{
				cout << "closingsocket" << PerHandleData->Socket << endl;
				closesocket(PerHandleData->Socket);

				delete PerIoData;
				delete PerHandleData;
				continue;
			}
			else
			{
				printf("GetQueuedCompletionStatus failed!\n");
			}
			return 0;
		}
		//如果数据传输完成就退出
		if (BytesTransferred == 0)
		{
			cout << "closing socket:" << PerHandleData->Socket << endl;
			closesocket(PerHandleData->Socket);
			//释放结构体资源
			delete PerIoData;
			delete PerHandleData;
			continue;
		}
		//打印获取的数据
		cout <<"["<<PerHandleData->Socket << "]:" << PerIoData->Buffer << endl;
		if (!strcmp(PerIoData->Buffer,"bye"))
		{
			cout << "closing socket:" << PerHandleData->Socket << endl;
			closesocket(PerHandleData->Socket);
			//释放结构体资源
			delete PerIoData;
			delete PerHandleData;
			continue;
		}
		CHAR buff[8192];
		cout << "[127.0.0.1]:";
		string str;
		getline(cin, str);
		for (int i = 0; i < str.length(); i++)
			buff[i] = str[i];
		buff[str.length()] = '\0';
		send(PerHandleData->Socket, buff, strlen(buff), 0);
		// 继续向 socket 投递WSARecv操作
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = 8192;
		WSARecv(PerHandleData->Socket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
	}
	return 0;
}
