// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "LibcurlHttp.h"
#include <string>
#include <iostream>


int PROGRESS_CALLBACK(double dltotal, double dlnow, double ultotal, double ulnow, void* userData)
{
	if (dltotal != 0)
	{
		COORD coord;
		coord.X = 0; coord.Y = 0;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

		printf("%.2f/%.2f = %d%%\n", dlnow, dltotal, (int)((dlnow / dltotal) * 100));
	}
	return 0;
}

int main()
{
	//去除光标
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	GetConsoleCursorInfo(hConsoleOutput, &ConsoleCursorInfo);
	ConsoleCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsoleOutput, &ConsoleCursorInfo);

	//HMODULE hDll = LoadLibraryA("LibcurlHttp.dll");
	//if (!hDll)
	//	return 1;

	///*
	////test
	//typedef LibcurlHttp* (*FN_CreateHttp)(void);
	//FN_CreateHttp CreateHttp = (FN_CreateHttp)GetProcAddress(hDll, "CreateHttp");
	//typedef void (*FN_ReleaseHttp)(LibcurlHttp* p);
	//FN_ReleaseHttp ReleaseHttp = (FN_ReleaseHttp)GetProcAddress(hDll, "ReleaseHttp");
	//if (CreateHttp && ReleaseHttp)
	//{
	//	LibcurlHttp* http = CreateHttp();
	//	http->post_a("https://www.baidu.com", true, "a", "123", "b", "456", NULL);
	//	int nLen = 0;
	//	const char* szBody = http->getBody(nLen);
	//	ReleaseHttp(http);
	//}

	////test
	//typedef int (*FN_get_a)(const char* url, bool dealRedirect, ...);
	//FN_get_a get_a = (FN_get_a)GetProcAddress(hDll, "get_a");
	//typedef const char* (*FN_getBody)(int& len);
	//FN_getBody getBody = (FN_getBody)GetProcAddress(hDll, "getBody");
	//if (get_a && getBody)
	//{
	//	int code = get_a("https://www.baidu.com", true, "a", "123", "b", "456", NULL);
	//	int nLen = 0;
	//	const char* szBody = getBody(nLen);
	//	
	//}

	////test
	//typedef int (*FN_download)(const char* url, const char* localFileName);
	//FN_download download = (FN_download)GetProcAddress(hDll, "download");
	//if (download)
	//{
	//	int code = download("http://192.168.1.8/vs2008_sp1.rar", "d:\\1.rar");
	//	printf("%d", code);
	//}
	//*/

	//typedef int (*FN_postForm)(const char* url, ...);
	//FN_postForm postForm = (FN_postForm)GetProcAddress(hDll, "postForm");
	//if (postForm)
	//{
	//	int n = postForm("http://192.168.1.8/host/", "file", "D:\\1.rar", 1, NULL);
	//}

	//FreeLibrary(hDll);

	//HTTP_CLIENT::Ins().setCustomMothod("ILOVEYOU");

	//HTTP_CLIENT::Ins().postForm_a("http://10.224.104.3/com1", 2, "file", "C:\\Users\\Administrator\\Desktop\\idgcs-min.zip", NULL, 1, "f1", "v1", 1, "f2", "v2", NULL);
	//HTTP_CLIENT::Ins().postForm_a("http://10.224.104.3/com1", 2, "file", "C:\\Users\\Administrator\\Desktop\\idgcs-min.zip", "idgcs-min.zip", 1, "f1", "v1", 1, "f2", "v2", NULL);
	//HTTP_CLIENT::Ins().postForm_a("http://10.224.104.3/com1", 1, "f1", "v1", 1, "f2", "v2", NULL);
	//HTTP_CLIENT::Ins().get("https://www.baidu.com");

	/*
	FORM_FIELD* fields = new FORM_FIELD[2];
	FillFormField(fields[0], 0, "a", "1", "");
	FillFormField(fields[1], 1, "file", "C:\\Users\\Administrator\\Desktop\\idgcs-min.zip", "idgcs-min.zip");
	HTTP_CLIENT::Ins().postForm("http://10.224.104.3/com1", fields, 2);
	delete[] fields;
	*/

	//std::string sData = "[\"6514fd5149f04b1d85ced33a70f1e0c1\",\"ef852b274fd741c5bbbc8ad956d3ea0e\"]";
	//int nCode = HTTP_CLIENT::Ins().post("http://10.224.104.3/com1", sData.data(), sData.size(), true, "application/json; charset=utf-8");

	//int nLen = 0;
	//const char* pBody = HTTP_CLIENT::Ins().getBody(nLen);
	//std::wstring ws = HTTP_CLIENT::Ins().UTF8ToWidebyte(pBody);
	//int nk = HTTP_CLIENT::Ins().getResponseHeaderKeysCount();
	//for (int i = 0; i < nk; ++i)
	//{
	//	const char* key = HTTP_CLIENT::Ins().getResponseHeaderKey(i);
	//	std::cout << key << ":" << HTTP_CLIENT::Ins().getResponseHeader(key, 0) << std::endl;
	//}
	//std::wcout << ws.c_str() << std::endl;

	//HTTP_CLIENT::Ins().setProgress(PROGRESS_CALLBACK, NULL);
	//int code = HTTP_CLIENT::Ins().download("http://download.rz520.com/small/photoshopcs6.zip", NULL);

	HTTP_CLIENT::Ins().get("https://www.baidu.com");

    return 0;
}

