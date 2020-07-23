#include <stdio.h>
#include <atlstr.h>
#include <string>
#include <iostream>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#undef UNICODE
#pragma once
class client
{
public:
	client();
	~client();
	
	void Connect(string ip, int port);
	
};

