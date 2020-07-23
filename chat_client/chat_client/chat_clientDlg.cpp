
// chat_clientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "chat_client.h"
#include "chat_clientDlg.h"
#include "afxdialogex.h"
#include <winsock2.h>
#include<iostream>
#include <string>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_RECVDATA WM_USER+1


DWORD WINAPI re(LPVOID lpParam);

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

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


// Cchat_clientDlg �Ի���



Cchat_clientDlg::Cchat_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cchat_clientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cchat_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cchat_clientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECVDATA, OnRecvData)
	ON_BN_CLICKED(IDC_BUTTON1, &Cchat_clientDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// Cchat_clientDlg ��Ϣ�������

BOOL Cchat_clientDlg::OnInitDialog()
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

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	init();
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cchat_clientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cchat_clientDlg::OnPaint()
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
HCURSOR Cchat_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


struct RECUPARAM
{
	SOCKET Socket;
	HWND hwnd;
};
bool Cchat_clientDlg::init()
{
	WSADATA wsaData;
	if (WSAStartup(0x0202, &wsaData) != 0)
	{
		MessageBox(_T("WSAStartup error"));
	}
	sockClient = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//���ػ�·��ַ127������һ̨�����ϲ��Ե�IP
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4444);//�ͷ������˵Ķ˿ںű���һ��  
	if (!connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))//���ӷ������ˣ��׽��֣���ַת�������ȣ�
	{
		MessageBox(_T("���������ӳɹ�"));
	}
	DWORD ThreadID;
	RECUPARAM *pRecvParam = new RECUPARAM;
	pRecvParam->Socket = sockClient;
	pRecvParam->hwnd = m_hWnd;
	HANDLE ThreadHadle = CreateThread(NULL, 0, re, (LPVOID)pRecvParam, 0, &ThreadID);
	CloseHandle(ThreadHadle);
	return true;
}


DWORD WINAPI re(LPVOID lpParam)
{
	SOCKET sock = ((RECUPARAM*)lpParam)->Socket;
	HWND hwnd = ((RECUPARAM*)lpParam)->hwnd;
	SOCKADDR_IN addrFrom = {};
	int len = sizeof(SOCKADDR);
	char recvBuf[8192], tempbuf[8192];
	int retval;
	while (true)
	{
		retval = recv(sock, recvBuf,8192, 0);
		if (SOCKET_ERROR == retval)
		{
			break;
		}
		recvBuf[retval] = '\0';
		sprintf(tempbuf, "%s",recvBuf);
		::PostMessage(hwnd, WM_RECVDATA, 0, (LPARAM)tempbuf);
	}
	return 0;
}


LRESULT Cchat_clientDlg::OnRecvData(WPARAM wParam, LPARAM lParam)
{
	CString str = (char*)lParam;
	CString  strTemp;
	GetDlgItemText(IDC_EDIT1, strTemp);
	str += "\r\n";
	str += strTemp;
	SetDlgItemText(IDC_EDIT1, str);
	return 0;
}

void Cchat_clientDlg::OnBnClickedButton1()
{
	/*DWORD dwIP=0;
	SOCKADDR_IN addrTo;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port = htons(8888);
	addrTo.sin_addr.S_un.S_addr = htonl(dwIP);*/
	CString strSend;
	GetDlgItemText(IDC_EDIT2, strSend);
	send(sockClient, strSend, strSend.GetLength() + 1, 0);
	SetDlgItemText(IDC_EDIT2, "");
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
