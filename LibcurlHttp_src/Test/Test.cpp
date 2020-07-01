// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "LibcurlHttp.h"
#include <string>
#include <iostream>


std::string ANSItoUTF8(const char* strAnsi)
{
	//获取转换为宽字节后需要的缓冲区大小，创建宽字节缓冲区，936为简体中文GB2312代码页  
	int nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, NULL, NULL);
	WCHAR *wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(CP_ACP, NULL, strAnsi, -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;
	//获取转为UTF8多字节后需要的缓冲区大小，创建多字节缓冲区  
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, NULL, NULL, NULL, NULL);
	CHAR *szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(CP_UTF8, NULL, wszBuffer, -1, szBuffer, nLen, NULL, NULL);
	szBuffer[nLen] = 0;

	std::string s1 = szBuffer;
	//内存清理  
	delete[]wszBuffer;
	delete[]szBuffer;

	return s1;
}

int main()
{
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

	//HTTP_CLIENT::Ins().postForm("http://10.224.104.3/com1", 2, "file", "C:\\Users\\Administrator\\Desktop\\idgcs-min.zip", NULL, 1, "f1", "v1", 1, "f2", "v2", NULL);
	//HTTP_CLIENT::Ins().postForm("http://10.224.104.3/com1", 1, "f1", "v1", 1, "f2", "v2", NULL);
	HTTP_CLIENT::Ins().get("https://www.baidu.com");

	//std::string sData = "[\"6514fd5149f04b1d85ced33a70f1e0c1\",\"ef852b274fd741c5bbbc8ad956d3ea0e\"]";
	//int nCode = HTTP_CLIENT::Ins().post("http://10.224.104.3/com1", sData.data(), sData.size(), true, "application/json; charset=utf-8");

	int nLen = 0;
	const char* pBody = HTTP_CLIENT::Ins().getBody(nLen);
	std::wstring ws = HTTP_CLIENT::Ins().UTF8ToWidebyte(pBody);
	int nk = HTTP_CLIENT::Ins().getResponseHeaderKeysCount();
	for (int i = 0; i < nk; ++i)
	{
		const char* key = HTTP_CLIENT::Ins().getResponseHeaderKey(i);
		std::cout << key << ":" << HTTP_CLIENT::Ins().getResponseHeader(key, 0) << std::endl;
	}

	std::wcout << ws.c_str() << std::endl;

    return 0;
}

