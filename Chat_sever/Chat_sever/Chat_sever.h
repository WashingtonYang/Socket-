
// Chat_sever.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CChat_severApp: 
// �йش����ʵ�֣������ Chat_sever.cpp
//

class CChat_severApp : public CWinApp
{
public:
	CChat_severApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CChat_severApp theApp;