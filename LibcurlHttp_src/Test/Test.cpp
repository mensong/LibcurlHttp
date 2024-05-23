// Test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "LibcurlHttp.h"
#include <string>
#include <iostream>
#include "..\pystring\pystring.h"
#include "..\HttpHelper\Convertor.h"

bool ProgressCallback(double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow, void* userData)
{
	//<upload position, download position>
	std::pair<COORD, COORD>* outputPos = (std::pair<COORD, COORD>*)userData;
	if (!outputPos)
		return 0;

	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

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

		////测试中断
		//if ((downloadNow / downloadTotal) > 0.5)
		//	return false;
	}

	//恢复光标
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), 
		{ csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y });

	return true;
}

bool g_isBodyTooLarge = false;
bool HeaderCallback(const char* header, void* userData)
{
	printf(header);

	std::string headerKV = header;
	size_t seperator = headerKV.find_first_of(':');
	if (std::string::npos != seperator)
	{
		std::string key = headerKV.substr(0, seperator);
		key = pystring::strip(key);
		if (pystring::equal(key, "Content-Length"))
		{
			std::string value = headerKV.substr(seperator + 1);
			value = pystring::strip(value);
			auto length = atoll(value.c_str());
			if (length > 1024000)
				g_isBodyTooLarge = true;
			else
				g_isBodyTooLarge = false;
		}
	}

	return true;
}

void dumpCode(LibcurlHttp* http)
{
	printf("http_code=%d\n", http->getCode());
}
void dumpBody(LibcurlHttp* http, bool utf8 = true)
{
	if (!g_isBodyTooLarge)
	{
#define MAX_PRINT 4096
		size_t len = 0;
		std::string sBody = http->getBody(len);
		if (sBody.size() > MAX_PRINT)
			sBody[MAX_PRINT] = '\0';

		if (utf8)
		{
			size_t len = strlen(sBody.c_str());
			wprintf(http->UTF8ToUnicode(sBody.c_str(), len));
		}
		else
			printf(sBody.c_str());
	}
	else
	{
		printf("<内容太长>");
	}

	printf("\n");
}

void TestConvert()
{	
	LibcurlHttp* http = HTTP_CLIENT::Ins().CreateHttp();

	const char* pstr = "1a我①△▽○◇□☆▲▼●▓※men";

	//转码测试
	size_t len = strlen(pstr);
	const wchar_t* pwstr = http->AnsiToUnicode(pstr, len);
	const char* pustr = http->UnicodeToUTF8(pwstr, len);
	const wchar_t* pwstr1 = http->UTF8ToUnicode(pustr, len);
	const char* pstr1 = http->UnicodeToAnsi(pwstr1, len);
	if (strcmp(pstr, pstr1) == 0)
		printf("测试通过\n");
	else
		printf("测试不通过\n");

	//错误的utf8转码，则跳过错误码
	len = strlen(pstr);
	const wchar_t* pwstr2 = http->UTF8ToUnicode(pstr, len);
	if (wcscmp(pwstr2, L"1amen") == 0)
		printf("测试通过\n");
	else
		printf("测试不通过\n");

	HTTP_CLIENT::Ins().ReleaseHttp(http);
}

int main(int argc, char** argv)
{
	TestConvert();

	//去除光标
	HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO ConsoleCursorInfo;
	GetConsoleCursorInfo(hConsoleOutput, &ConsoleCursorInfo);
	ConsoleCursorInfo.bVisible = FALSE;
	SetConsoleCursorInfo(hConsoleOutput, &ConsoleCursorInfo);

	//创建HTTP对象
	LibcurlHttp* http = HTTP_CLIENT::Ins().CreateHttp();

	//http->setCustomMethod("mensong");

	//设置进度显示
	std::pair<COORD, COORD> progressPos;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	progressPos.first = csbi.dwCursorPosition;
	csbi.dwCursorPosition.Y += 1;
	progressPos.second = csbi.dwCursorPosition;
	http->setProgress(ProgressCallback, &progressPos);
	//为进度腾出空间
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),
		{ csbi.dwCursorPosition.X, (short)(csbi.dwCursorPosition.Y + 2) });

	//设置头回调
	http->setResponseHeaderCallback(HeaderCallback, NULL, true);

#if 1
	http->get("https://www.baidu.com");
	dumpBody(http);
#endif

#if 0
	char downloadedFileName[MAX_PATH];
	http->download("https://sm.myapp.com/original/Download/LeapFTPSetup_3.1.0.50.exe", 
		"D:/1/\\2/3\\4\\//", downloadedFileName);
#endif

#if 0
	std::string filename = http->UrlUTF8Encode("Test.exe");
	MultipartField mf(NULL, 0, ".\\Test.exe", filename.c_str(), "file", NULL);
	http->postMultipart("http://192.168.77.1/upload", &mf, 1);
#endif

#if 0
	MultipartField arrMF[3];
	arrMF[0].Fill("test1", 5, NULL, NULL, "field1", NULL);
	arrMF[1].Fill("test2", 5, NULL, NULL, "field2", NULL);
	arrMF[2].Fill("test3", 5, NULL, NULL, "field3", NULL);
	http->postMultipart("http://127.0.0.1:8080/post", arrMF, 3);
#endif

#if 0
	std::vector<MultipartField*> pp;
	pp.push_back(new MultipartField("test1", 5, NULL, NULL, "field1", NULL));
	pp.push_back(new MultipartField("test2", 5, NULL, NULL, "field2", NULL));
	pp.push_back(new MultipartField("test3", 5, NULL, NULL, "field3", NULL));
	http->postMultipart("http://127.0.0.1:8080/post", pp.data(), pp.size());
	for (size_t i = 0; i < pp.size(); i++)
		delete pp[i];
#endif

#if 0
	MultipartField* pp1[3];
	pp1[0] = new MultipartField("test1", 5, NULL, NULL, "field1", NULL);
	pp1[1] = new MultipartField("test2", 5, NULL, NULL, "field2", NULL);
	pp1[2] = new MultipartField("test3", 5, NULL, NULL, "field3", NULL);
	http->postMultipart("http://127.0.0.1:8080/post", pp1, 3);
	for (size_t i = 0; i < 3; i++)
		delete pp1[i];
#endif

#if 0
	http->putData("http://127.0.0.1:8080/put", "test", 4);
#endif

#if 0
	http->putFile("http://127.0.0.1:8080/put", ".\\Test.exe");
#endif

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

	dumpCode(http);

	//销毁对象
	HTTP_CLIENT::Ins().ReleaseHttp(http);

    return 0;
}

