// NetWork_6.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <pcap.h>
#include <WinSock2.h>
#include <ntddndis.h>
#include <conio.h>
#include <iostream>
#include <stdio.h>
#include<stdio.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wpcap.lib")

#pragma warning(disable:4996)

using namespace std;

#define ETH_ARP         0x0806  //��̫��֡���ͱ�ʾ�������ݵ����ͣ�����ARP�����Ӧ����˵�����ֶε�ֵΪx0806
#define ARP_HARDWARE    1  //Ӳ�������ֶ�ֵΪ��ʾ��̫����ַ
#define ETH_IP          0x0800  //Э�������ֶα�ʾҪӳ���Э���ַ����ֵΪx0800��ʾIP��ַ
#define ARP_REQUEST     1   //ARP����
#define ARP_RESPONSE       2      //ARPӦ��

//14�ֽ���̫���ײ�
struct EthernetHeader
{
	u_char DestMAC[6];    //Ŀ��MAC��ַ 6�ֽ�
	u_char SourMAC[6];   //ԴMAC��ַ 6�ֽ�
	u_short EthType;         //��һ��Э�����ͣ���0x0800������һ����IPЭ�飬0x0806Ϊarp  2�ֽ�
};

//28�ֽ�ARP֡�ṹ
struct ArpHeader
{
	unsigned short hdType;   //Ӳ������
	unsigned short proType;   //Э������
	unsigned char hdSize;   //Ӳ����ַ����
	unsigned char proSize;   //Э���ַ����
	unsigned short op;   //�������ͣ�ARP����1����ARPӦ��2����RARP����3����RARPӦ��4����
	u_char smac[6];   //ԴMAC��ַ
	u_char sip[4];   //ԴIP��ַ
	u_char dmac[6];   //Ŀ��MAC��ַ
	u_char dip[4];   //Ŀ��IP��ַ
};

//��������arp���İ����ܳ���42�ֽ�
struct ArpPacket {
	EthernetHeader ed;
	ArpHeader ah;
};

pcap_if_t *alldevs; //�����б�
pcap_if_t *d; //һ������
pcap_addr_t *pAddr; //������ַ
pcap_t *adhandle;
int i = 0;
char errbuf[PCAP_ERRBUF_SIZE];


void getDevice();
void attack();
void Send();
char *iptos(u_long in);

int _tmain(int argc, _TCHAR* argv[])
{
	getDevice();
	attack();
	system("pause");
	return 0;
}

void getDevice()
{
	int inum;//�û���Ҫ��ȡ�����������
	//��ȡ��������������
							//Դ��ַ            ָ��Pcap_rmtauth�Ľṹ��ָ��   ָ��pacp_if_t�ṹ��ָ��  ָ���û�����Ļ�����
	if (pcap_findalldevs_ex(PCAP_SRC_IF_STRING, NULL /* auth is not needed */, &alldevs, errbuf) == -1)
	{
		cout << "Error in pcap_findalldevs_ex:" << errbuf << endl;
		exit(1);
	}

	for (d = alldevs; d != NULL; d = d->next)
	{
		printf("%d. %s", ++i, d->name);
		if (d->description)
			printf(" (%s)\n", d->description);
		else
			printf(" (No description available)\n");
	}
	
	if (i == 0)
	{
		printf("\nNo interfaces found! Make sure WinPcap is installed.\n");
		return;
	}
	cout << "Enter the interface number (1-" << i << "):";
	cin >> inum;

	if (inum < 1 || inum > i)
	{
		printf("\nInterface number out of range.\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		return ;
	}

	//��ת��ѡȡ��������
	for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);

	//���豸
	if ((adhandle = pcap_open(d->name,          // �豸��
		65536,            // 65535��֤�ܲ��񵽲�ͬ������·���ϵ�ÿ�����ݰ���ȫ������
		PCAP_OPENFLAG_PROMISCUOUS,    // ����ģʽ
		1000,             // ��ȡ��ʱʱ��
		NULL,             // Զ�̻�����֤
		errbuf            // ���󻺳��
		)) == NULL)
	{
		cout << "open " << d->name << "error" << endl;
		pcap_freealldevs(alldevs);
		return;
	}

	cout << "\nListening on" << d->description << endl;
	
	/* ������Ҫ�豸�б��ˣ��ͷ��� */
	//pcap_freealldevs(alldevs);  
	
}

void attack()
{
	struct bpf_program fcode;
	char packet_filter[] = "ip and udp";
	unsigned long ip;
	unsigned long netmask;
	//unsigned long FakeIp = inet_addr("192.168.1.1");
	if (pcap_datalink(adhandle) != DLT_EN10MB)
	{
		printf("\nonly work network.\n");
		/* �ͷ��豸�б� */
		pcap_freealldevs(alldevs);
		exit(0);
	}

	for (pAddr = d->addresses; pAddr; pAddr = pAddr->next)
	{
		string s;
		//cout << d->addresses << endl;
		//��ȡIP����������
		ip = ((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr;
		s = iptos(((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr);
		cout << iptos(((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr) << endl;
		netmask = ((struct sockaddr_in *)(pAddr->netmask))->sin_addr.S_un.S_addr;
		if (!netmask)
			netmask = 0xffffff;
		if (pcap_compile(adhandle,&fcode,packet_filter,1,netmask) <0)
		{
			printf("\nonly IP and udp.\n");
			/* �ͷ��豸�б� */
			pcap_freealldevs(alldevs);
			exit(0);
		}
		if (pcap_setfilter(adhandle, &fcode) < 0)
		{
			printf("\set filter error.\n");
			/* �ͷ��豸�б� */
			pcap_freealldevs(alldevs);
			exit(0);
		}

		Send();	
	}
}

void Send()
{
	//��ʼ���ARP�����������д���ڴ����У�������ʱ���ݿ�������д
	unsigned char sendbuf[42]; //arp���ṹ��С��42���ֽ�
	unsigned char mac[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x66 };
	unsigned char source_ip[4] = { 0xc0, 0xa8, 0xb4, 0x80 };
	EthernetHeader eh;
	ArpHeader ah;
	//��ֵMAC��ַ
	memset(eh.DestMAC, 0xff, 6);   //��̫���ײ�Ŀ��MAC��ַ��ȫΪ�㲥��ַ
	memcpy(eh.SourMAC, mac, 6);   //��̫���ײ�ԴMAC��ַ
	memcpy(ah.smac, mac, 6);   //ARP�ֶ�ԴMAC��ַ
	memset(ah.dmac, 0xff, 6);   //ARP�ֶ�Ŀ��MAC��ַ
	memcpy(ah.sip, source_ip, 4);   //ARP�ֶ�ԴIP��ַ
	memset(ah.dip, 0x05, 4);   //ARP�ֶ�Ŀ��IP��ַ
	eh.EthType = htons(ETH_ARP);   //htons�����������޷��Ŷ�������ת���������ֽ�˳��
	ah.hdType = htons(ARP_HARDWARE);
	ah.proType = htons(ETH_IP);
	ah.hdSize = 6;
	ah.proSize = 4;
	ah.op = htons(ARP_REQUEST);

	//����һ��ARP����
	memset(sendbuf, 0, sizeof(sendbuf));   //ARP����
	memcpy(sendbuf, &eh, sizeof(eh));
	memcpy(sendbuf + sizeof(eh), &ah, sizeof(ah));
	//������ͳɹ�
	if (pcap_sendpacket(adhandle, sendbuf, 42) == 0) {
		printf("\nPacketSend succeed\n");
	}
	else {
		printf("PacketSendPacket in getmine Error: %d\n", GetLastError());
	}
}

/* ���������͵�IP��ַת�����ַ������͵� */
#define IPTOSBUFFERS    12
char *iptos(u_long in)
{
	static char output[IPTOSBUFFERS][3 * 4 + 3 + 1];
	static short which;
	u_char *p;

	p = (u_char *)&in;
	which = (which + 1 == IPTOSBUFFERS ? 0 : which + 1);
	sprintf(output[which], "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
	return output[which];
}




