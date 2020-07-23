
// Chat_severDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Chat_sever.h"
#include "Chat_severDlg.h"
#include "afxdialogex.h"
#include <winsock2.h>
#include<iostream>
#include <string>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DWORD WINAPI work(LPVOID PortID);

struct client
{
	SOCKET Socket;
	int name;
}cli[10];

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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
int cont = 0;
int num = 0;
int a = 0;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChat_severDlg �Ի���



CChat_severDlg::CChat_severDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChat_severDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChat_severDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_programLangList);
}

BEGIN_MESSAGE_MAP(CChat_severDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CChat_severDlg ��Ϣ�������

BOOL CChat_severDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��



	HANDLE ThreadHadle = CreateThread(NULL, 0, init, NULL, 0, &ThreadID);
	CloseHandle(ThreadHadle);
	



	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	CRect rect;
	// ��ȡ��������б���ͼ�ؼ���λ�úʹ�С   
	m_programLangList.GetClientRect(&rect);

	// Ϊ�б���ͼ�ؼ����ȫ��ѡ�к�դ����   
	m_programLangList.SetExtendedStyle(m_programLangList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// Ϊ�б���ͼ�ؼ����һ��   
	m_programLangList.InsertColumn(0, _T("�Ѿ������û�"), LVCFMT_CENTER, rect.Width(), 0);
	//MessageBox(_T("sssss"));
	/*for (int i = 0; i < cont; i++)
	{
	m_programLangList.InsertItem(i, _T("�û�", cli[i].name));
	RefreshIndividualControl(IDC_LIST);
	}*/


	SetTimer(1, 500, NULL);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChat_severDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChat_severDlg::OnPaint()
{

	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChat_severDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



DWORD WINAPI work(LPVOID PortID)
{
	HANDLE CompletionPort = (HANDLE)PortID;//��ɶ˿ھ��
	DWORD BytesTransferred;				   //���ݴ�����ֽ���
	LPPER_HANDLE_DATA PerHandleData;	   //Socket����ṹ��
	LPPER_IO_OPERATION_DATA PerIoData;	   //i/o�����ṹ��

	while (true)
	{
		//�����ɶ˿�״̬
		//�õ�IO�������
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
		//������ݴ�����ɾ��˳�
		if (BytesTransferred == 0)
		{
			cout << "closing socket:" << PerHandleData->Socket << endl;
			closesocket(PerHandleData->Socket);
			//�ͷŽṹ����Դ
			delete PerIoData;
			delete PerHandleData;
			continue;
		}	
		/*if (!strcmp(PerIoData->Buffer, "bye"))
		{
			cout << "closing socket:" << PerHandleData->Socket << endl;
			int flag = 1;
			for (int i = 0; i < cont; i++)
			{
				if (PerHandleData->Socket==cli[i].Socket)
				{
					if (i == cont - 1)
					{
						cont--;
						a--;
						break;
					}
					for (int j = 0; j < cont; j++)
					{
						if (i == j) flag = 0;
						if (flag)
							cli[j].Socket = cli[j].Socket;
						else
							cli[j].Socket = cli[j+1].Socket;
						if (j==cont-1)
						{
							cont--;
							a--;
							break;
						}
					}
					break;
				}
			}
			closesocket(PerHandleData->Socket);
			//�ͷŽṹ����Դ
			delete PerIoData;
			delete PerHandleData;
			continue;
		}*/
		//ת����Ϣ
		int number = 0;
		for (int i = 0; i < cont; i++)
		{
			if (PerHandleData->Socket == cli[i].Socket)
			{
				number = i;
			}
		}
		
		for (int i = 0; i < cont; i++)
		{
			char buffer[8192] = "�û�";
			buffer[strlen(buffer)] = '0'+number;
			buffer[strlen(buffer)] = ':';
			int flag = strlen(buffer);
			for (int j = strlen(buffer); j < strlen(buffer) + strlen(PerIoData->Buffer); j++)
			{
				buffer[j] = PerIoData->Buffer[j - flag];
			}
			send(cli[i].Socket, buffer, strlen(buffer), 0);
		}

		// ������ socket Ͷ��WSARecv����
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = 8192;
		WSARecv(PerHandleData->Socket, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
	}

	return 0;
}

DWORD WINAPI CChat_severDlg::init(LPVOID PortID)
{
	CChat_severDlg a;
	GetSystemInfo(&a.SystemInfo);
	a.workThread = a.SystemInfo.dwNumberOfProcessors * 2;
	WSADATA wsaData;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		printf("WSAStartup error\n");
	}
	//������ɶ˿�
	if ((a.CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
	{
		a.MessageBox(_T("CreateIoCOmpletionPort failed:%d", GetLastError()));
		return false;
	}
	//���������߳�
	for (int i = 0; i < a.workThread; i++)
	{
		HANDLE ThreadHadle;
		ThreadHadle = CreateThread(NULL, 0, work, a.CompletionPort, 0, &a.ThreadID);
		if (ThreadHadle)
			CloseHandle(ThreadHadle);
	}
	//����socket
	if ((a.Listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		a.MessageBox(_T("WSASocket error:%d", GetLastError()));
		return false;
	}
	//�󶨵����ص�ַ
	a.sAddr.sin_family = AF_INET; // Э���ΪIPV4��  
	a.sAddr.sin_port = htons(4444);// �˿�  ��Ϊ������С��ģʽ�������Ǵ��ģʽ������htons�ѱ����ֽ���תΪ�����ֽ��� 
	a.sAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	a.retVal = bind(a.Listen, (PSOCKADDR)&a.sAddr, sizeof(a.sAddr));
	if (a.retVal == SOCKET_ERROR)
	{
		a.MessageBox(_T("bind error:%ld", WSAGetLastError()));
		closesocket(a.Listen);
		WSACleanup();
		return false;
	}
	//��������
	if (listen(a.Listen, 5) == SOCKET_ERROR) {
		a.MessageBox(_T("listen failed with error:%ld", WSAGetLastError()));
		closesocket(a.Listen);
		WSACleanup();
		return false;
	}
	LPPER_HANDLE_DATA perHandledata;
	LPPER_IO_OPERATION_DATA PerIoData;
	while (true)
	{
		a.Accept = WSAAccept(a.Listen, NULL, NULL, NULL, 0);
		cli[cont].Socket = a.Accept;
		cli[cont].name = num++;
		//Accept = accept(a.Listen, 0, 0);
		//����socket ���
		perHandledata = new PER_HANDLE_DATA();
		perHandledata->Socket = a.Accept;
		// ������Ŀͻ��˺���ɶ˿���ϵ����
		CreateIoCompletionPort((HANDLE)a.Accept, a.CompletionPort, (DWORD)perHandledata, 0);

		//ΪIO�����ڴ�ռ����
		PerIoData = new PER_IO_OPERATION_DATA();
		//��ʼ��I/O����
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = 8192;
		//�������ݣ��ŵ�PerIoData
		DWORD Flags = 0;
		DWORD dwRecv = 0;
		WSARecv(a.Accept, &PerIoData->DataBuf, 1, &dwRecv, &Flags, &PerIoData->Overlapped, NULL);
		cont++;
	}
	WSACleanup();
	return 0;
}


void CChat_severDlg::RefreshIndividualControl(UINT uCtlID)
{
	CRect rect;
	GetDlgItem(uCtlID)->GetWindowRect(&rect); //��ȡ�ؼ�����??
	ScreenToClient(&rect); //��Ļ����Կͻ������ת�� ? ?
	InvalidateRect(rect);//ˢ��?
}




void CChat_severDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nIDEvent)
	{
	case 1:
	{
		//MessageBox(_T("assaa"));
		if (cont - a>0)
		{
			string s = "�û�";
			s += to_string(a);
			CString x;
			x = s.c_str();
			m_programLangList.InsertItem(cont,x);
			RefreshIndividualControl(IDC_LIST);
			a++;
		}
		break;
	}
	default:
		break;
	}
	CDialogEx::OnTimer(nIDEvent);
}
