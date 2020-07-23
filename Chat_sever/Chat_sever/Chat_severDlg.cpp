
// Chat_severDlg.cpp : 实现文件
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

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
int cont = 0;
int num = 0;
int a = 0;
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


// CChat_severDlg 对话框



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


// CChat_severDlg 消息处理程序

BOOL CChat_severDlg::OnInitDialog()
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



	HANDLE ThreadHadle = CreateThread(NULL, 0, init, NULL, 0, &ThreadID);
	CloseHandle(ThreadHadle);
	



	// TODO:  在此添加额外的初始化代码
	CRect rect;
	// 获取编程语言列表视图控件的位置和大小   
	m_programLangList.GetClientRect(&rect);

	// 为列表视图控件添加全行选中和栅格风格   
	m_programLangList.SetExtendedStyle(m_programLangList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	// 为列表视图控件添加一列   
	m_programLangList.InsertColumn(0, _T("已经连接用户"), LVCFMT_CENTER, rect.Width(), 0);
	//MessageBox(_T("sssss"));
	/*for (int i = 0; i < cont; i++)
	{
	m_programLangList.InsertItem(i, _T("用户", cli[i].name));
	RefreshIndividualControl(IDC_LIST);
	}*/


	SetTimer(1, 500, NULL);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CChat_severDlg::OnPaint()
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
HCURSOR CChat_severDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
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
			//释放结构体资源
			delete PerIoData;
			delete PerHandleData;
			continue;
		}*/
		//转发消息
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
			char buffer[8192] = "用户";
			buffer[strlen(buffer)] = '0'+number;
			buffer[strlen(buffer)] = ':';
			int flag = strlen(buffer);
			for (int j = strlen(buffer); j < strlen(buffer) + strlen(PerIoData->Buffer); j++)
			{
				buffer[j] = PerIoData->Buffer[j - flag];
			}
			send(cli[i].Socket, buffer, strlen(buffer), 0);
		}

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
	//创建完成端口
	if ((a.CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL)
	{
		a.MessageBox(_T("CreateIoCOmpletionPort failed:%d", GetLastError()));
		return false;
	}
	//创建工作线程
	for (int i = 0; i < a.workThread; i++)
	{
		HANDLE ThreadHadle;
		ThreadHadle = CreateThread(NULL, 0, work, a.CompletionPort, 0, &a.ThreadID);
		if (ThreadHadle)
			CloseHandle(ThreadHadle);
	}
	//创建socket
	if ((a.Listen = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		a.MessageBox(_T("WSASocket error:%d", GetLastError()));
		return false;
	}
	//绑定到本地地址
	a.sAddr.sin_family = AF_INET; // 协议簇为IPV4的  
	a.sAddr.sin_port = htons(4444);// 端口  因为本机是小端模式，网络是大端模式，调用htons把本机字节序转为网络字节序 
	a.sAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	a.retVal = bind(a.Listen, (PSOCKADDR)&a.sAddr, sizeof(a.sAddr));
	if (a.retVal == SOCKET_ERROR)
	{
		a.MessageBox(_T("bind error:%ld", WSAGetLastError()));
		closesocket(a.Listen);
		WSACleanup();
		return false;
	}
	//开启监听
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
		//分配socket 句柄
		perHandledata = new PER_HANDLE_DATA();
		perHandledata->Socket = a.Accept;
		// 将接入的客户端和完成端口联系起来
		CreateIoCompletionPort((HANDLE)a.Accept, a.CompletionPort, (DWORD)perHandledata, 0);

		//为IO分配内存空间操作
		PerIoData = new PER_IO_OPERATION_DATA();
		//初始化I/O操作
		ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
		PerIoData->DataBuf.buf = PerIoData->Buffer;
		PerIoData->DataBuf.len = 8192;
		//接受数据，放到PerIoData
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
	GetDlgItem(uCtlID)->GetWindowRect(&rect); //获取控件区域??
	ScreenToClient(&rect); //屏幕区域对客户区域的转换 ? ?
	InvalidateRect(rect);//刷新?
}




void CChat_severDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	switch (nIDEvent)
	{
	case 1:
	{
		//MessageBox(_T("assaa"));
		if (cont - a>0)
		{
			string s = "用户";
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
