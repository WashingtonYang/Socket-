// Network4_serve.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Serve.h"
#include <WinSock2.h>
#include <stdio.h>
#include <atlstr.h>
#include <string.h>
#include <iostream>

using namespace std;
#pragma comment(lib,"ws2_32.lib")

int _tmain(int argc, _TCHAR* argv[])
{
	Serve C;
	C.initServer();
	system("pause");
	return 0;
}

