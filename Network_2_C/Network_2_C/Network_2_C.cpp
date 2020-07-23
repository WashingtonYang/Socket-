// Network_2_C.cpp : �������̨Ӧ�ó������ڵ㡣
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
	//��ʼ��winSock��
	WSAData wsaDate;
	if (WSAStartup(MAKEWORD(2, 2), &wsaDate) != 0)
	{
		printf("WSAStartup failed\n");
		return -1;
	}
	//����UDP�׽���
	//socket����
	SOCKET udpClient = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (udpClient == INVALID_SOCKET)
	{
		printf("Failed socket() \n");
		return -1;
	}

	// Ҳ�������������bind������һ�����ص�ַ
	// ����ϵͳ�����Զ�����

	// ���sockaddr_in�ṹ
	sockaddr_in sin;
	sin.sin_family = AF_INET; //ipv4
	sin.sin_port = htons(4444);//�������ֽ���תΪ�����ֽ���
	sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

	//��������
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
		//��������
		iResult = recvfrom(udpClient, buff, 1024, 0, (SOCKADDR *)& SenderAddr, &SenderAddrSize);
		if (iResult == SOCKET_ERROR) {
			printf("recvfrom failed with error %d\n", WSAGetLastError());
			closesocket(udpClient);
			WSACleanup();
			return 1;
		}
		else{
			buff[iResult] = '\0';
			printf(" Recv message��%s����%s\n", inet_ntoa(SenderAddr.sin_addr), buff);
			if (strcmp(buff, "bye") == 0)
			{
				printf("Exiting.\n");
				break;
			}
		}
	}//�ر�socket
	closesocket(udpClient);

	//�ͷ���Դ�˳�
	WSACleanup();
	system("pause");
	return 0;
}

