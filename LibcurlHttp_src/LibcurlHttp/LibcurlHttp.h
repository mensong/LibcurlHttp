#pragma once
#ifndef _AFX
#include <windows.h>
#endif
#include <string>
#include <stdint.h>

#ifdef LIBCURLHTTP_EXPORTS
#define LIBCURLHTTP_API extern "C" __declspec(dllexport)
#else
#define LIBCURLHTTP_API extern "C" __declspec(dllimport)
#endif

typedef enum HttpDoErrorCode
{
	ecUrlError = -1,
	ecDataError = -2,
} HttpDoErrorCode;

typedef struct MultipartField
{
	char* contenxtData;	//提交的是直接指定的内容，该字段有内容时，filePath字段无效
	int contenxtDataSize;

	char* filePath;		//提交的是文件，contenxtData有内容时，该字段无效
	char* fileName;		//指定文件名

	char* multipartName;	//名称
	char* mimeType;			//Mime type

	MultipartField()
	{
		contenxtData = NULL;
		contenxtDataSize = 0;
		filePath = NULL;
		fileName = NULL;
		multipartName = NULL;
		mimeType = NULL;
	}

	MultipartField(
		const char* contenxtData, int contenxtDataSize, 
		const char* filePath, 
		const char* fileName, 
		const char* multipartName, 
		const char* mimeType)
	{
		this->contenxtData = NULL;
		this->contenxtDataSize = 0;
		this->filePath = NULL;
		this->fileName = NULL;
		this->multipartName = NULL;
		this->mimeType = NULL;

		if (contenxtData <= 0)
		{
			if (contenxtData)
				contenxtDataSize = strlen(contenxtData);
			else
				contenxtDataSize = 0;
		}

		if (contenxtData)
		{
			if (contenxtDataSize == 0)
			{
				this->contenxtData = new char[1];
				this->contenxtData[0] = '\0';
			}
			else
			{
				this->contenxtData = new char[contenxtDataSize + 1];
				memset(this->contenxtData, 0, contenxtDataSize + 1);
				memcpy_s(this->contenxtData, contenxtDataSize + 1, contenxtData, contenxtDataSize);
			}
		}

		if (filePath)
		{
			size_t len = strlen(filePath);
			this->filePath = new char[len + 1];
			strcpy_s(this->filePath, len + 1, filePath);
		}

		if (fileName)
		{
			size_t len = strlen(fileName);
			this->fileName = new char[len + 1];
			strcpy_s(this->fileName, len + 1, fileName);
		}

		if (multipartName)
		{
			size_t len = strlen(multipartName);
			this->multipartName = new char[len + 1];
			strcpy_s(this->multipartName, len + 1, multipartName);
		}

		if (mimeType)
		{
			size_t len = strlen(mimeType);
			this->mimeType = new char[len + 1];
			strcpy_s(this->mimeType, len + 1, mimeType);
		}
	}

	~MultipartField()
	{
		Release();
	}

	void Release()
	{
		if (contenxtData)
		{
			delete[] contenxtData;
			contenxtData = NULL;
		}
		if (filePath)
		{
			delete[] filePath;
			filePath = NULL;
		}
		if (fileName)
		{
			delete[] fileName;
			fileName = NULL;
		}
		if (multipartName)
		{
			delete[] multipartName;
			multipartName = NULL;
		}
		if (mimeType)
		{
			delete[] mimeType;
			mimeType = NULL;
		}
	}

	void Fill(
		const char* contenxtData, int contenxtDataSize,
		const char* filePath,
		const char* fileName,
		const char* multipartName,
		const char* mimeType)
	{
		new (this) MultipartField(
			contenxtData, contenxtDataSize,
			filePath,
			fileName,
			multipartName,
			mimeType);
	}

private:
	MultipartField(const MultipartField& o) {}
	MultipartField(const MultipartField* o) {}
} MultipartField;

//进度回调原型
typedef int (*FN_PROGRESS_CALLBACK)(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow,
	void* userData);

class LibcurlHttp
{
public:
	//设置超时时长，单位 秒
	virtual void setTimeout(int t) = 0;
	//设置请求头
	virtual void setRequestHeader(const char* key, const char* value) = 0;
	//设置UserAgent
	virtual void setUserAgent(const char* val) = 0;
	//设置cookies
	//virtual void setCookies();

	//设置自定义方法
	//  DELETE PUT HEAD OPTIONS FUCK……，全大写
	virtual void setCustomMethod(const char* method) = 0;

	//设置进度回调
	virtual void setProgress(FN_PROGRESS_CALLBACK progress, void* userData) = 0;

	//设置是否自动重定向
	virtual void setAutoRedirect(bool autoRedirect) = 0;
	//设置自动重定向次数
	virtual void setMaxRedirect(int maxRedirect) = 0;

	//设置当response body的格式为GZIP时，是否解压。默认开启
	virtual void setDecompressIfGzip(bool decompressIfGzip) = 0;

	//http get
	virtual int get(const char* url) = 0;
	//http get
	//不定参数：const char* key1, const char* val1, const char* key2, const char* val2, ……, NULL
	//  不定参数为成对的形式出现，并且后面一定有一个NULL标志不定参数的结束。
	virtual int get_a(const char* url, ...) = 0;
	virtual int get_b(const char* url, va_list argv) = 0;

	//http post
	virtual int post(const char* url, const char* content, int contentLen, const char* contentType = "application/x-www-form-urlencoded") = 0;
	//http post
	//不定参数：const char* key1, const char* val1, const char* key2, const char* val2, ……, NULL
	//  不定参数为成对的形式出现，并且后面一定有一个NULL标志不定参数的结束。
	virtual int post_a(const char* url, ...) = 0;
	virtual int post_b(const char* url, va_list argv) = 0;

	//提交multipart
	virtual int postMultipart(const char* url, MultipartField* pMmultipartDataArr[], int nCountMultipartData) = 0;
	virtual int postMultipart(const char* url, MultipartField* multipartDataArr, int nCountMultipartData) = 0;
	// url, fieldtype1(1:普通字段；2:file字段), fieldname1, fieldvalue1, mimeType, [如果fieldtype1==2则存在]fileName1,    
	//      fieldtype2(1:普通字段；2:file字段), fieldname2, fieldvalue2, mimeType, [如果fieldtype2==2则存在]fileName2, 
	//      ……, NULL(最后必须使用NULL结束)
	virtual int postMultipart_a(const char* url, ...) = 0;
	virtual int postMultipart_b(const char* url, va_list argv) = 0;
	
	//下载网络文件到本地
	// localFileName - 想要保存到的文件路径或目录，如果是目录则会在此目录下根据Content-Disposition的描述自动获取文件名或生成一个临时文件名
	// downloadedFileName - 实际保存到的文件路径，char outFileName[MAX_PATH]; download("https://123.com", NULL, outFileName);
	virtual int download(const char* url, const char* localFileName = NULL, char* downloadedFileName = NULL) = 0;
	
	//put
	virtual int putData(const char* url, const unsigned char* data, size_t dataLen) = 0;
	virtual int putData(const char* url, const char* data, size_t dataLen) = 0;
	virtual int putFile(const char* url, const char* filePath) = 0;
	
	//获得提交后的body内容
	virtual const char* getBody(int& len) = 0;
	//获得提交后的返回code
	virtual int getCode() = 0;
	//获得Response headers
	virtual int getResponseHeaderKeysCount() = 0;
	virtual const char* getResponseHeaderKey(int i) = 0;
	virtual int getResponseHeadersCount(const char* key, bool ignoreCase = false) = 0;
	virtual const char* getResponseHeader(const char* key, int i, bool ignoreCase = false) = 0;

	//url转码
	virtual const char* UrlGB2312Encode(const char * strIn) = 0;
	//url解码
	virtual const char* UrlGB2312Decode(const char * strIn) = 0;

	//url转码
	virtual const char* UrlUTF8Encode(const char * strIn) = 0;
	//url解码
	virtual const char* UrlUTF8Decode(const char * strIn) = 0;

	//字符转码
	virtual const char* WidebyteToAnsi(const wchar_t * strIn) = 0;
	virtual const wchar_t* AnsiToWidebyte(const char * strIn) = 0;
	virtual const char* UTF8ToAnsi(const char * strIn) = 0;
	virtual const wchar_t* UTF8ToWidebyte(const char * strIn) = 0;
	virtual const char* AnsiToUTF8(const char * strIn) = 0;
	virtual const char* WidebyteToUTF8(const wchar_t * strIn) = 0;
};


//创建http对象
LIBCURLHTTP_API LibcurlHttp* CreateHttp(void);
//销毁http对象
LIBCURLHTTP_API void ReleaseHttp(LibcurlHttp* p);

//以单例的方式，注释见LibcurlHttp
LIBCURLHTTP_API void setTimeout(LibcurlHttp* http, int timeout);
LIBCURLHTTP_API void setRequestHeader(LibcurlHttp* http, const char* key, const char* value);
LIBCURLHTTP_API void setUserAgent(LibcurlHttp* http, const char* val);
LIBCURLHTTP_API void setCustomMethod(LibcurlHttp* http, const char* method);
LIBCURLHTTP_API void setProgress(LibcurlHttp* http, FN_PROGRESS_CALLBACK progress, void* userData);
LIBCURLHTTP_API void setAutoRedirect(LibcurlHttp* http, bool autoRedirect);
LIBCURLHTTP_API void setMaxRedirect(LibcurlHttp* http, int maxRedirect);
LIBCURLHTTP_API void setDecompressIfGzip(LibcurlHttp* http, bool decompressIfGzip);

LIBCURLHTTP_API int get(LibcurlHttp* http, const char* url);
LIBCURLHTTP_API int get_a(LibcurlHttp* http, const char* url, ...);

LIBCURLHTTP_API int post(LibcurlHttp* http, const char* url, const char* content, int contentLen, const char* contentType = "application/x-www-form-urlencoded");
LIBCURLHTTP_API int post_a(LibcurlHttp* http, const char* url, ...);

LIBCURLHTTP_API int download(LibcurlHttp* http, const char* url, const char* localFileName = NULL, char* downloadedFileName = NULL);

//pMultipartDataArr = new MultipartField*[X]; or std::vector<MultipartField*> arr; postMultipart(http, url, arr.data(), arr.size());
LIBCURLHTTP_API int postMultipart(LibcurlHttp* http, const char* url, MultipartField* pMmultipartDataArr[], int nCountMultipartData);
LIBCURLHTTP_API int postMultipartA(LibcurlHttp* http, const char* url, MultipartField* multipartDataArr, int nCountMultipartData);
LIBCURLHTTP_API int postMultipart_a(LibcurlHttp* http, const char* url, ...);
LIBCURLHTTP_API int postMultipart_b(LibcurlHttp* http, const char* url, va_list argv);

LIBCURLHTTP_API int putData(LibcurlHttp* http, const char* url, const unsigned char* data, size_t dataLen);
LIBCURLHTTP_API int putFile(LibcurlHttp* http, const char* url, const char* filePath);

LIBCURLHTTP_API const char* getBody(LibcurlHttp* http, int& len);
LIBCURLHTTP_API int getCode(LibcurlHttp* http);
LIBCURLHTTP_API int getResponseHeaderKeysCount(LibcurlHttp* http);
LIBCURLHTTP_API const char* getResponseHeaderKey(LibcurlHttp* http, int i);
LIBCURLHTTP_API int getResponseHeadersCount(LibcurlHttp* http, const char* key, bool ignoreCase);
LIBCURLHTTP_API const char* getResponseHeader(LibcurlHttp* http, const char* key, int i, bool ignoreCase);

LIBCURLHTTP_API const char* UrlGB2312Encode(LibcurlHttp* http, const char * strIn);
LIBCURLHTTP_API const char* UrlGB2312Decode(LibcurlHttp* http, const char * strIn);
LIBCURLHTTP_API const char* UrlUTF8Encode(LibcurlHttp* http, const char * strIn);
LIBCURLHTTP_API const char* UrlUTF8Decode(LibcurlHttp* http, const char * strIn);

LIBCURLHTTP_API const char* WidebyteToAnsi(LibcurlHttp* http, const wchar_t * strIn);
LIBCURLHTTP_API const wchar_t* AnsiToWidebyte(LibcurlHttp* http, const char * strIn);
LIBCURLHTTP_API const char* UTF8ToAnsi(LibcurlHttp* http, const char * strIn);
LIBCURLHTTP_API const wchar_t* UTF8ToWidebyte(LibcurlHttp* http, const char * strIn);
LIBCURLHTTP_API const char* AnsiToUTF8(LibcurlHttp* http, const char * strIn);
LIBCURLHTTP_API const char* WidebyteToUTF8(LibcurlHttp* http, const wchar_t * strIn);





class HTTP_CLIENT
{
public:
	typedef LibcurlHttp* (*FN_CreateHttp)(void);
	typedef void(*FN_ReleaseHttp)(LibcurlHttp* p);
	typedef void(*FN_setTimeout)(LibcurlHttp* http, int timeout);
	typedef void(*FN_setRequestHeader)(LibcurlHttp* http, const char* key, const char* value);
	typedef void(*FN_setUserAgent)(LibcurlHttp* http, const char* val);
	typedef void(*FN_setCustomMethod)(LibcurlHttp* http, const char* method);
	typedef void(*FN_setProgress)(LibcurlHttp* http, FN_PROGRESS_CALLBACK progress, void* userData);
	typedef void(*FN_setAutoRedirect)(LibcurlHttp* http, bool autoRedirect);
	typedef void(*FN_setMaxRedirect)(LibcurlHttp* http, int maxRedirect);
	typedef void(*FN_setDecompressIfGzip)(LibcurlHttp* http, bool decompressIfGzip);
	typedef int(*FN_get)(LibcurlHttp* http, const char* url);
	typedef int(*FN_get_a)(LibcurlHttp* http, const char* url, ...);
	typedef int(*FN_post)(LibcurlHttp* http, const char* url, const char* content, int contentLen, const char* contentType);
	typedef int(*FN_post_a)(LibcurlHttp* http, const char* url, ...);
	typedef int(*FN_download)(LibcurlHttp* http, const char* url, const char* localFileName, char* downloadedFileName);
	typedef int(*FN_postMultipart)(LibcurlHttp* http, const char* url, MultipartField* pMmultipartDataArr[], int nCountMultipartData);
	typedef int(*FN_postMultipartA)(LibcurlHttp* http, const char* url, MultipartField* multipartDataArr, int nCountMultipartData);
	typedef int(*FN_postMultipart_a)(LibcurlHttp* http, const char* url, ...);
	typedef int(*FN_postMultipart_b)(LibcurlHttp* http, const char* url, va_list argv);
	typedef const char* (*FN_getBody)(LibcurlHttp* http, int& len);
	typedef int(*FN_getCode)(LibcurlHttp* http);
	typedef int(*FN_getResponseHeaderKeysCount)(LibcurlHttp* http);
	typedef const char* (*FN_getResponseHeaderKey)(LibcurlHttp* http, int i);
	typedef int(*FN_getResponseHeadersCount)(LibcurlHttp* http, const char* key);
	typedef const char* (*FN_getResponseHeader)(LibcurlHttp* http, const char* key, int i);
	typedef const char* (*FN_UrlGB2312Encode)(LibcurlHttp* http, const char * strIn);
	typedef const char* (*FN_UrlGB2312Decode)(LibcurlHttp* http, const char * strIn);
	typedef const char* (*FN_UrlUTF8Encode)(LibcurlHttp* http, const char * strIn);
	typedef const char* (*FN_UrlUTF8Decode)(LibcurlHttp* http, const char * strIn);
	typedef const char* (*FN_WidebyteToAnsi)(LibcurlHttp* http, const wchar_t * strIn);
	typedef const wchar_t* (*FN_AnsiToWidebyte)(LibcurlHttp* http, const char * strIn);
	typedef const char* (*FN_UTF8ToAnsi)(LibcurlHttp* http, const char * strIn);
	typedef const wchar_t* (*FN_UTF8ToWidebyte)(LibcurlHttp* http, const char * strIn);
	typedef const char* (*FN_AnsiToUTF8)(LibcurlHttp* http, const char * strIn);
	typedef const char* (*FN_WidebyteToUTF8)(LibcurlHttp* http, const wchar_t * strIn);

#define DEF_PROC(hDll, name) \
	name = (FN_##name)::GetProcAddress(hDll, #name)

	HTTP_CLIENT()
	{
		hDll = LoadLibraryFromCurrentDir("LibcurlHttp.dll");
		if (!hDll)
			return;

		DEF_PROC(hDll, CreateHttp);
		DEF_PROC(hDll, ReleaseHttp);
		DEF_PROC(hDll, setTimeout);
		DEF_PROC(hDll, setRequestHeader);
		DEF_PROC(hDll, setUserAgent);
		DEF_PROC(hDll, setCustomMethod);
		DEF_PROC(hDll, setProgress);
		DEF_PROC(hDll, setAutoRedirect);
		DEF_PROC(hDll, setMaxRedirect); 
		DEF_PROC(hDll, setDecompressIfGzip);
		DEF_PROC(hDll, get);
		DEF_PROC(hDll, get_a);
		DEF_PROC(hDll, post);
		DEF_PROC(hDll, post_a);
		DEF_PROC(hDll, download);
		DEF_PROC(hDll, postMultipart);
		DEF_PROC(hDll, postMultipartA);
		DEF_PROC(hDll, postMultipart_a);
		DEF_PROC(hDll, postMultipart_b);
		DEF_PROC(hDll, getBody);
		DEF_PROC(hDll, getCode);
		DEF_PROC(hDll, getResponseHeaderKeysCount);
		DEF_PROC(hDll, getResponseHeaderKey);
		DEF_PROC(hDll, getResponseHeadersCount);
		DEF_PROC(hDll, getResponseHeader);
		DEF_PROC(hDll, UrlGB2312Encode);
		DEF_PROC(hDll, UrlGB2312Decode);
		DEF_PROC(hDll, UrlUTF8Encode);
		DEF_PROC(hDll, UrlUTF8Decode);
		DEF_PROC(hDll, WidebyteToAnsi);
		DEF_PROC(hDll, AnsiToWidebyte);
		DEF_PROC(hDll, UTF8ToAnsi);
		DEF_PROC(hDll, UTF8ToWidebyte);
		DEF_PROC(hDll, AnsiToUTF8);
		DEF_PROC(hDll, WidebyteToUTF8);
	}
	
	FN_CreateHttp			CreateHttp;
	FN_ReleaseHttp			ReleaseHttp;
	FN_setTimeout			setTimeout;
	FN_setRequestHeader		setRequestHeader;
	FN_setUserAgent			setUserAgent;
	FN_setCustomMethod		setCustomMethod;
	FN_setProgress			setProgress;
	FN_setAutoRedirect		setAutoRedirect;
	FN_setMaxRedirect		setMaxRedirect;
	FN_setDecompressIfGzip	setDecompressIfGzip;
	FN_get					get;
	FN_get_a				get_a;
	FN_post					post;
	FN_post_a				post_a;
	FN_download				download;
	FN_postMultipart		postMultipart;
	FN_postMultipartA		postMultipartA;
	FN_postMultipart_a		postMultipart_a;
	FN_postMultipart_b		postMultipart_b;
	FN_getBody				getBody;
	FN_getCode				getCode;
	FN_getResponseHeaderKeysCount getResponseHeaderKeysCount;
	FN_getResponseHeaderKey getResponseHeaderKey;
	FN_getResponseHeadersCount getResponseHeadersCount;
	FN_getResponseHeader	getResponseHeader;
	FN_UrlGB2312Encode		UrlGB2312Encode;
	FN_UrlGB2312Decode		UrlGB2312Decode;
	FN_UrlUTF8Encode		UrlUTF8Encode;
	FN_UrlUTF8Decode		UrlUTF8Decode;
	FN_WidebyteToAnsi		WidebyteToAnsi;
	FN_AnsiToWidebyte		AnsiToWidebyte;
	FN_UTF8ToAnsi			UTF8ToAnsi;
	FN_UTF8ToWidebyte		UTF8ToWidebyte;
	FN_AnsiToUTF8			AnsiToUTF8;
	FN_WidebyteToUTF8		WidebyteToUTF8;
	
public:
	~HTTP_CLIENT()
	{
		if (hDll)
		{
			FreeLibrary(hDll);
			hDll = NULL;
		}
	}

	static HTTP_CLIENT& Ins()
	{
		static HTTP_CLIENT s_ins;
		return s_ins;
	}

	static HMODULE LoadLibraryFromCurrentDir(const char* dllName)
	{
		char selfPath[MAX_PATH];
		MEMORY_BASIC_INFORMATION mbi;
		HMODULE hModule = ((::VirtualQuery(LoadLibraryFromCurrentDir, &mbi, sizeof(mbi)) != 0) ? 
			(HMODULE)mbi.AllocationBase : NULL);
		::GetModuleFileNameA(hModule, selfPath, MAX_PATH);
		std::string moduleDir(selfPath);
		size_t idx = moduleDir.find_last_of('\\');
		moduleDir = moduleDir.substr(0, idx);
		std::string modulePath = moduleDir + "\\" + dllName;
		char curDir[MAX_PATH];
		::GetCurrentDirectoryA(MAX_PATH, curDir);
		::SetCurrentDirectoryA(moduleDir.c_str());
		HMODULE hDll = LoadLibraryA(modulePath.c_str());
		::SetCurrentDirectoryA(curDir);
		if (!hDll)
		{
			DWORD err = ::GetLastError();
			char buf[10];
			sprintf_s(buf, "%u", err);
			::MessageBoxA(NULL, ("找不到" + modulePath + "模块:" + buf).c_str(),
				"找不到模块", MB_OK | MB_ICONERROR);
		}
		return hDll;
	}

	HMODULE hDll;
};
