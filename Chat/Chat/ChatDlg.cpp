
// ChatDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Chat.h"
#include "ChatDlg.h"
#include "afxdialogex.h"

#pragma warning(disable:4996)

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define WM_RECVDATA WM_USER+1
DWORD WINAPI RecvProc(LPVOID lpParam);
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
struct RECUPARAM
{
	SOCKET sock;
	HWND hwnd;
};
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


// CChatDlg 对话框



CChatDlg::CChatDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CChatDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CChatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_RECVDATA,OnRecvData)
	ON_BN_CLICKED(IDC_BUTTON_SEND, &CChatDlg::OnBnClickedButtonSend)
END_MESSAGE_MAP()


// CChatDlg 消息处理程序

BOOL CChatDlg::OnInitDialog()
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
	Initsocket();
	RECUPARAM *pRecvParam = new RECUPARAM;
	pRecvParam->sock = m_socket;
	pRecvParam->hwnd = m_hWnd;
	HANDLE hThread = CreateThread(NULL, 0, RecvProc, (LPVOID)pRecvParam, 0, NULL);
	CloseHandle(hThread);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CChatDlg::OnPaint()
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
HCURSOR CChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



bool CChatDlg::Initsocket()
{
	m_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (INVALID_SOCKET == m_socket)
	{
		MessageBox(_T("套接字创建失败"));
		return FALSE;
	}
	SOCKADDR_IN addrSocket;
	addrSocket.sin_family = AF_INET;
	addrSocket.sin_port = htons(8888);
	addrSocket.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retval=bind(m_socket, (SOCKADDR*)&addrSocket, sizeof(SOCKADDR));
	if (SOCKET_ERROR == retval)
	{
		MessageBox(_T("绑定失败"));
		closesocket(m_socket);
		return FALSE;
	}

	return TRUE;
}

DWORD WINAPI RecvProc(LPVOID lpParam)
{
	SOCKET sock = ((RECUPARAM*)lpParam)->sock;
	HWND hwnd = ((RECUPARAM*)lpParam)->hwnd;

	SOCKADDR_IN addrFrom;
	int len = sizeof(SOCKADDR);
	char recvBuf[200], tempbuf[300];
	int retval;
	while (true)
	{
		retval = recvfrom(sock, recvBuf, 200, 0, (SOCKADDR*)&addrFrom, &len);
		if (SOCKET_ERROR==retval)
		{
			break;
		}
		sprintf(tempbuf, "%s说:%s", inet_ntoa(addrFrom.sin_addr), recvBuf);
		::PostMessage(hwnd, WM_RECVDATA, 0, (LPARAM)tempbuf);
	}
	return 0;
}

LRESULT CChatDlg::OnRecvData(WPARAM wParam, LPARAM lParam)
{
	CString str = (char*)lParam;
	CString  strTemp;
	GetDlgItemText(IDC_EDIT_RECV, strTemp);
	str += "\r\n";
	str += strTemp;
	SetDlgItemText(IDC_EDIT_RECV, str);
	return 0;
}



void CChatDlg::OnBnClickedButtonSend()
{
	
	DWORD dwIP;
	((CIPAddressCtrl*)GetDlgItem(IDC_IPADDRESS1))->GetAddress(dwIP);
	SOCKADDR_IN addrTo;
	addrTo.sin_family = AF_INET;
	addrTo.sin_port=htons(8888);
	addrTo.sin_addr.S_un.S_addr=htonl(dwIP);
	CString strSend;
	GetDlgItemText(IDC_EDIT_SEND, strSend);
	sendto(m_socket, strSend, strSend.GetLength() + 1, 0,
		(sockaddr*)&addrTo,sizeof(SOCKADDR));
		SetDlgItemText(IDC_EDIT_SEND, "");

	// TODO:  在此添加控件通知处理程序代码
}
