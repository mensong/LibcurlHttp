// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "LibcurlHttp.h"
#include <string>
#include <iostream>
#include "Md5.h"
#include "..\pystring\pystring.h"

int PROGRESS_CALLBACK(double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow, void* userData)
{
	//<upload position, download position>
	std::pair<COORD, COORD>* outputPos = (std::pair<COORD, COORD>*)userData;
	if (!outputPos)
		return 0;

	if (uploadTotal != 0)
	{
		COORD coord;
		coord.X = outputPos->first.X;
		coord.Y = outputPos->first.Y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
		
		printf("↑%.0f/%.0f = %d%%\n", uploadNow, uploadTotal, (int)((uploadNow / uploadTotal) * 100));
	}

	if (downloadTotal != 0)
	{
		COORD coord;
		coord.X = outputPos->second.X; 
		coord.Y = outputPos->second.Y;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);

		printf("↓%.0f/%.0f = %d%%\n", downloadNow, downloadTotal, (int)((downloadNow / downloadTotal) * 100));
	}
	return 0;
}

int main(int argc, char** argv)
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

	//HTTP_CLIENT::Ins().setCustomMethod("ILOVEYOU");

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

	LibcurlHttp* http = HTTP_CLIENT::Ins().CreateHttp();

	//http->setCustomMethod("post");

	std::pair<COORD, COORD> progressPos;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	progressPos.first = csbi.dwCursorPosition;
	csbi.dwCursorPosition.Y += 1;
	progressPos.second = csbi.dwCursorPosition;
	HTTP_CLIENT::Ins().setProgress(http, PROGRESS_CALLBACK, &progressPos);

	//char downloadedFileName[MAX_PATH];
	//HTTP_CLIENT::Ins().download(http, "https://sm.myapp.com/original/Download/LeapFTPSetup_3.1.0.50.exe", "E:/1/\\2/3\\4\\//", downloadedFileName);

	//std::string filename = http->UrlUTF8Encode("Test.exe");
	//MultipartField* mf = new MultipartField(NULL, 0, "Test.exe", filename.c_str(), "file", NULL);
	//http->postMultipart("http://192.168.77.1/upload", &mf, 1);
	//delete mf;

	//MultipartField* mf1 = new MultipartField("test", 4, NULL, NULL, "file", NULL);
	//http->postMultipart("http://192.168.77.1/upload", &mf1, 1);
	//delete mf1;
	//int len = 0;
	//const char* body = http->getBody(len);
	//std::string sBody = http->UTF8ToAnsi(body);

	http->putData("http://192.168.77.1/upload", (const unsigned char*)"test", 4);
	http->putFile("http://192.168.77.1/upload", "Test.exe");

#if 0
	std::string url(1024, 0);
	std::cout << "输入url:";
	std::cin.getline(&url[0], 1024);

	std::string cookies(1024, 0);
	std::cout << "输入cookies:";
	std::cin.getline(&cookies[0], 1024);

	std::string data(10240, 0);
	std::cout << "输入提交内容:";
	std::cin.getline(&data[0], 10240);

	http->setRequestHeader("Accept", "application/json, text/plain, */*");
	http->setRequestHeader("Accept-Encoding", "gzip, deflate");
	http->setRequestHeader("Connection", "keep-alive");
	http->setRequestHeader("Content-Type", "application/x-www-form-urlencoded");	
	http->setRequestHeader("Cookie", cookies.c_str());
	http->setRequestHeader("User-Agent", "MDI");
		
	SYSTEMTIME st;
	::GetLocalTime(&st);
	std::string Platform_Auth = md5(
		std::to_string(st.wYear) + "-" +
		std::to_string(st.wMonth) + "-" +
		std::to_string(st.wDay));
	http->setRequestHeader("Platform_Auth", Platform_Auth.c_str());
	//不自动重定向，以获得当cookies过期时的错误信息
	http->setAutoRedirect(false);

	int httpCode = http->post(url.c_str(),
		data.c_str(), data.size(), "application/x-www-form-urlencoded");


	int len = 0;
	const char* body = http->getBody(len);
	std::string sBody = http->UTF8ToAnsi(body);

	std::cout << "http code=" << httpCode << std::endl;
	std::cout << "body=" << sBody << std::endl;
#endif

	HTTP_CLIENT::Ins().ReleaseHttp(http);

    return 0;
}

