
// chat_client.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cchat_clientApp: 
// �йش����ʵ�֣������ chat_client.cpp
//

class Cchat_clientApp : public CWinApp
{
public:
	Cchat_clientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cchat_clientApp theApp;