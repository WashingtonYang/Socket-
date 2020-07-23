// NetWork_6.cpp : 定义控制台应用程序的入口点。
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

#define ETH_ARP         0x0806  //以太网帧类型表示后面数据的类型，对于ARP请求或应答来说，该字段的值为x0806
#define ARP_HARDWARE    1  //硬件类型字段值为表示以太网地址
#define ETH_IP          0x0800  //协议类型字段表示要映射的协议地址类型值为x0800表示IP地址
#define ARP_REQUEST     1   //ARP请求
#define ARP_RESPONSE       2      //ARP应答

//14字节以太网首部
struct EthernetHeader
{
	u_char DestMAC[6];    //目的MAC地址 6字节
	u_char SourMAC[6];   //源MAC地址 6字节
	u_short EthType;         //上一层协议类型，如0x0800代表上一层是IP协议，0x0806为arp  2字节
};

//28字节ARP帧结构
struct ArpHeader
{
	unsigned short hdType;   //硬件类型
	unsigned short proType;   //协议类型
	unsigned char hdSize;   //硬件地址长度
	unsigned char proSize;   //协议地址长度
	unsigned short op;   //操作类型，ARP请求（1），ARP应答（2），RARP请求（3），RARP应答（4）。
	u_char smac[6];   //源MAC地址
	u_char sip[4];   //源IP地址
	u_char dmac[6];   //目的MAC地址
	u_char dip[4];   //目的IP地址
};

//定义整个arp报文包，总长度42字节
struct ArpPacket {
	EthernetHeader ed;
	ArpHeader ah;
};

pcap_if_t *alldevs; //网卡列表
pcap_if_t *d; //一个网卡
pcap_addr_t *pAddr; //网卡地址
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
	int inum;//用户想要获取的设配器编号
	//获取网络适配器参数
							//源地址            指向Pcap_rmtauth的结构体指针   指向pacp_if_t结构体指针  指向用户分配的缓冲区
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
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		return ;
	}

	//跳转到选取的设配器
	for (d = alldevs, i = 0; i< inum - 1; d = d->next, i++);

	//打开设备
	if ((adhandle = pcap_open(d->name,          // 设备名
		65536,            // 65535保证能捕获到不同数据链路层上的每个数据包的全部内容
		PCAP_OPENFLAG_PROMISCUOUS,    // 混杂模式
		1000,             // 读取超时时间
		NULL,             // 远程机器验证
		errbuf            // 错误缓冲池
		)) == NULL)
	{
		cout << "open " << d->name << "error" << endl;
		pcap_freealldevs(alldevs);
		return;
	}

	cout << "\nListening on" << d->description << endl;
	
	/* 不再需要设备列表了，释放它 */
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
		/* 释放设备列表 */
		pcap_freealldevs(alldevs);
		exit(0);
	}

	for (pAddr = d->addresses; pAddr; pAddr = pAddr->next)
	{
		string s;
		//cout << d->addresses << endl;
		//获取IP和子网掩码
		ip = ((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr;
		s = iptos(((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr);
		cout << iptos(((struct sockaddr_in *)pAddr->addr)->sin_addr.s_addr) << endl;
		netmask = ((struct sockaddr_in *)(pAddr->netmask))->sin_addr.S_un.S_addr;
		if (!netmask)
			netmask = 0xffffff;
		if (pcap_compile(adhandle,&fcode,packet_filter,1,netmask) <0)
		{
			printf("\nonly IP and udp.\n");
			/* 释放设备列表 */
			pcap_freealldevs(alldevs);
			exit(0);
		}
		if (pcap_setfilter(adhandle, &fcode) < 0)
		{
			printf("\set filter error.\n");
			/* 释放设备列表 */
			pcap_freealldevs(alldevs);
			exit(0);
		}

		Send();	
	}
}

void Send()
{
	//开始填充ARP包，填充数据写死在代码中，测试用时数据可随意填写
	unsigned char sendbuf[42]; //arp包结构大小，42个字节
	unsigned char mac[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x66 };
	unsigned char source_ip[4] = { 0xc0, 0xa8, 0xb4, 0x80 };
	EthernetHeader eh;
	ArpHeader ah;
	//赋值MAC地址
	memset(eh.DestMAC, 0xff, 6);   //以太网首部目的MAC地址，全为广播地址
	memcpy(eh.SourMAC, mac, 6);   //以太网首部源MAC地址
	memcpy(ah.smac, mac, 6);   //ARP字段源MAC地址
	memset(ah.dmac, 0xff, 6);   //ARP字段目的MAC地址
	memcpy(ah.sip, source_ip, 4);   //ARP字段源IP地址
	memset(ah.dip, 0x05, 4);   //ARP字段目的IP地址
	eh.EthType = htons(ETH_ARP);   //htons：将主机的无符号短整形数转换成网络字节顺序
	ah.hdType = htons(ARP_HARDWARE);
	ah.proType = htons(ETH_IP);
	ah.hdSize = 6;
	ah.proSize = 4;
	ah.op = htons(ARP_REQUEST);

	//构造一个ARP请求
	memset(sendbuf, 0, sizeof(sendbuf));   //ARP清零
	memcpy(sendbuf, &eh, sizeof(eh));
	memcpy(sendbuf + sizeof(eh), &ah, sizeof(ah));
	//如果发送成功
	if (pcap_sendpacket(adhandle, sendbuf, 42) == 0) {
		printf("\nPacketSend succeed\n");
	}
	else {
		printf("PacketSendPacket in getmine Error: %d\n", GetLastError());
	}
}

/* 将数字类型的IP地址转换成字符串类型的 */
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




