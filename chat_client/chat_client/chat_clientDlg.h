
// chat_clientDlg.h : ͷ�ļ�
//

#pragma once
#include <winsock2.h>
#include<iostream>
#include <string>
using namespace std;
#pragma comment (lib, "Ws2_32.lib")

// Cchat_clientDlg �Ի���
class Cchat_clientDlg : public CDialogEx
{
// ����
public:
	Cchat_clientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	SOCKET sockClient;
// �Ի�������
	enum { IDD = IDD_CHAT_CLIENT_DIALOG };

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
	afx_msg LRESULT OnRecvData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	bool init();
	afx_msg void OnBnClickedButton1();
};
