
// Chat_severDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include <winsock2.h>
#include<iostream>
#include <string>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

// CChat_severDlg 对话框
class CChat_severDlg : public CDialogEx
{
// 构造
public:
	CChat_severDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CHAT_SEVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_programLangList;

	SOCKET Listen;
	SOCKET Accept;
	SYSTEM_INFO SystemInfo;
	int workThread;
	HANDLE CompletionPort;
	DWORD ThreadID;
	sockaddr_in sAddr;
	int retVal;
	static DWORD WINAPI init(LPVOID PortID);
	void RefreshIndividualControl(UINT uCtlID);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	UINT M;
};
