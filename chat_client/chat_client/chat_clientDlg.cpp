
// chat_clientDlg.cpp : 实现文件
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// Cchat_clientDlg 对话框



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


// Cchat_clientDlg 消息处理程序

BOOL Cchat_clientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	init();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Cchat_clientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
	addrSrv.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//本地回路地址127，用于一台机器上测试的IP
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(4444);//和服务器端的端口号保持一致  
	if (!connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR)))//连接服务器端（套接字，地址转换，长度）
	{
		MessageBox(_T("服务器连接成功"));
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
	// TODO:  在此添加控件通知处理程序代码
}
