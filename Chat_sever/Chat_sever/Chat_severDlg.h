
// Chat_severDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <winsock2.h>
#include<iostream>
#include <string>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

// CChat_severDlg �Ի���
class CChat_severDlg : public CDialogEx
{
// ����
public:
	CChat_severDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHAT_SEVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
