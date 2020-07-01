#pragma once

#ifdef LIBCURLHTTP_EXPORTS
#define LIBCURLHTTP_API extern "C" __declspec(dllexport)
#else
#define LIBCURLHTTP_API extern "C" __declspec(dllimport)
#endif

class LibcurlHttp
{
public:
	//设置超时时长
	virtual void setTimeout(int t) = 0;
	//设置请求头
	virtual void setRequestHeader(const char* key, const char* value) = 0;
	//设置UserAgent
	virtual void setUserAgent(const char* val) = 0;
	//设置cookies
	//virtual void setCookies();

	//设置自定义方法
	//  DELETE PUT HEAD OPTIONS FUCK……，全大写
	virtual void setCustomMothod(const char* mothod) = 0;

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
	//提交表单
	// url, fieldtype1(1:普通字段；2:file字段), fieldname1, fieldvalue1, [如果fieldtype1==2则存在]fileName1,    
	//      fieldtype2(1:普通字段；2:file字段), fieldname2, fieldvalue2, [如果fieldtype2==1则存在]fileName2, 
	//      ……, NULL
	virtual int postForm_a(const char* url, ...) = 0;
	virtual int postForm_b(const char* url, va_list argv) = 0;
		
	//下载网络文件到本地
	virtual int download(const char* url, const char* localFileName=NULL) = 0;
		
	//获得提交后的body内容
	virtual const char* getBody(int& len) = 0;
	//获得提交后的返回code
	virtual int getCode() = 0;
	//获得Response headers
	virtual int getResponseHeaderKeysCount() = 0;
	virtual const char* getResponseHeaderKey(int i) = 0;
	virtual int getResponseHeadersCount(const char* key) = 0;
	virtual const char* getResponseHeader(const char* key, int i) = 0;

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


//以对象的方式
LIBCURLHTTP_API LibcurlHttp* CreateHttp(void);
LIBCURLHTTP_API void ReleaseHttp(LibcurlHttp* p);

//以单例的方式，注释见LibcurlHttp
LIBCURLHTTP_API void setTimeout(int timeout);
LIBCURLHTTP_API void setRequestHeader(const char* key, const char* value);
LIBCURLHTTP_API void setUserAgent(const char* val);
LIBCURLHTTP_API void setCustomMothod(const char* mothod);

LIBCURLHTTP_API int get(const char* url);
LIBCURLHTTP_API int get_a(const char* url, ...);
LIBCURLHTTP_API int post(const char* url, const char* content, int contentLen, const char* contentType = "application/x-www-form-urlencoded");
LIBCURLHTTP_API int post_a(const char* url, ...);
LIBCURLHTTP_API int download(const char* url, const char* localFileName = NULL);
LIBCURLHTTP_API int postForm(const char* url, ...);

LIBCURLHTTP_API const char* getBody(int& len);
LIBCURLHTTP_API int getCode();
LIBCURLHTTP_API int getResponseHeaderKeysCount();
LIBCURLHTTP_API const char* getResponseHeaderKey(int i);
LIBCURLHTTP_API int getResponseHeadersCount(const char* key);
LIBCURLHTTP_API const char* getResponseHeader(const char* key, int i);

LIBCURLHTTP_API const char* UrlGB2312Encode(const char * strIn);
LIBCURLHTTP_API const char* UrlGB2312Decode(const char * strIn);
LIBCURLHTTP_API const char* UrlUTF8Encode(const char * strIn);
LIBCURLHTTP_API const char* UrlUTF8Decode(const char * strIn);

LIBCURLHTTP_API const char* WidebyteToAnsi(const wchar_t * strIn);
LIBCURLHTTP_API const wchar_t* AnsiToWidebyte(const char * strIn);
LIBCURLHTTP_API const char* UTF8ToAnsi(const char * strIn);
LIBCURLHTTP_API const wchar_t* UTF8ToWidebyte(const char * strIn);
LIBCURLHTTP_API const char* AnsiToUTF8(const char * strIn);
LIBCURLHTTP_API const char* WidebyteToUTF8(const wchar_t * strIn);



#ifndef LIBCURLHTTP_EXPORTS
typedef LibcurlHttp* (*FN_CreateHttp)(void);
typedef void(*FN_ReleaseHttp)(LibcurlHttp* p);
typedef void(*FN_setTimeout)(int timeout);
typedef void(*FN_setRequestHeader)(const char* key, const char* value);
typedef void(*FN_setUserAgent)(const char* val);
typedef void(*FN_setCustomMothod)(const char* mothod);
typedef int(*FN_get)(const char* url);
typedef int(*FN_get_a)(const char* url, ...);
typedef int(*FN_post)(const char* url, const char* content, int contentLen, const char* contentType);
typedef int(*FN_post_a)(const char* url, ...);
typedef int(*FN_download)(const char* url, const char* localFileName);
typedef int(*FN_postForm)(const char* url, ...);
typedef const char* (*FN_getBody)(int& len);
typedef int (*FN_getCode)();
typedef int (*FN_getResponseHeaderKeysCount)();
typedef const char* (*FN_getResponseHeaderKey)(int i);
typedef int (*FN_getResponseHeadersCount)(const char* key);
typedef const char* (*FN_getResponseHeader)(const char* key, int i);
typedef const char* (*FN_UrlGB2312Encode)(const char * strIn);
typedef const char* (*FN_UrlGB2312Decode)(const char * strIn);
typedef const char* (*FN_UrlUTF8Encode)(const char * strIn);
typedef const char* (*FN_UrlUTF8Decode)(const char * strIn);
typedef const char* (*FN_WidebyteToAnsi)(const wchar_t * strIn);
typedef const wchar_t* (*FN_AnsiToWidebyte)(const char * strIn);
typedef const char* (*FN_UTF8ToAnsi)(const char * strIn);
typedef const wchar_t* (*FN_UTF8ToWidebyte)(const char * strIn);
typedef const char* (*FN_AnsiToUTF8)(const char * strIn);
typedef const char* (*FN_WidebyteToUTF8)(const wchar_t * strIn);


#define DEF_PROC(hDll, name) \
	FN_##name name = (FN_##name)::GetProcAddress(hDll, #name)

#define INIT_LIBCURLHTTP(hDll) \
	HMODULE __hDll__ = (hDll); \
	DEF_PROC(__hDll__, CreateHttp); \
	DEF_PROC(__hDll__, ReleaseHttp); \
	DEF_PROC(__hDll__, setTimeout); \
	DEF_PROC(__hDll__, setRequestHeader); \
	DEF_PROC(__hDll__, setUserAgent); \
	DEF_PROC(__hDll__, setCustomMothod); \
	DEF_PROC(__hDll__, get); \
	DEF_PROC(__hDll__, get_a); \
	DEF_PROC(__hDll__, post); \
	DEF_PROC(__hDll__, post_a); \
	DEF_PROC(__hDll__, download); \
	DEF_PROC(__hDll__, postForm); \
	DEF_PROC(__hDll__, getBody); \
	DEF_PROC(__hDll__, getCode); \
	DEF_PROC(__hDll__, getResponseHeaderKeysCount); \
	DEF_PROC(__hDll__, getResponseHeaderKey); \
	DEF_PROC(__hDll__, getResponseHeadersCount); \
	DEF_PROC(__hDll__, getResponseHeader); \
	DEF_PROC(__hDll__, UrlGB2312Encode); \
	DEF_PROC(__hDll__, UrlGB2312Decode); \
	DEF_PROC(__hDll__, UrlUTF8Encode); \
	DEF_PROC(__hDll__, UrlUTF8Decode); \
	DEF_PROC(__hDll__, WidebyteToAnsi); \
	DEF_PROC(__hDll__, AnsiToWidebyte); \
	DEF_PROC(__hDll__, UTF8ToAnsi); \
	DEF_PROC(__hDll__, UTF8ToWidebyte); \
	DEF_PROC(__hDll__, AnsiToUTF8); \
	DEF_PROC(__hDll__, WidebyteToUTF8);

class HTTP_CLIENT
{
public:
private:
	static HTTP_CLIENT* s_ins;

public:
	static HTTP_CLIENT& Ins()
	{
		if (!s_ins)
			s_ins = new HTTP_CLIENT;
		return *s_ins;
	}

	static void Release()
	{
		if (s_ins)
		{
			delete s_ins;
			s_ins = NULL;
		}
	}

	HTTP_CLIENT()
	{
		hDll = LoadLibraryA("LibcurlHttp.dll");
		if (hDll)
		{
			INIT_LIBCURLHTTP(hDll);
			this->CreateHttp			=   CreateHttp;
			this->ReleaseHttp			=   ReleaseHttp;
			this->setTimeout			=   setTimeout;
			this->setRequestHeader		=   setRequestHeader;
			this->setUserAgent			=   setUserAgent;
			this->setCustomMothod       =   setCustomMothod;
			this->get					=   get;
			this->get_a					=   get_a;
			this->post					=   post;
			this->post_a				=   post_a;
			this->download				=   download; 
			this->postForm				=   postForm; 
			this->getBody				=   getBody; 
			this->getCode				=   getCode;
			this->getResponseHeaderKeysCount = getResponseHeaderKeysCount;
			this->getResponseHeaderKey	=	getResponseHeaderKey;
			this->getResponseHeadersCount = getResponseHeadersCount;
			this->getResponseHeader		=	getResponseHeader;
			this->UrlGB2312Encode		=   UrlGB2312Encode;
			this->UrlGB2312Decode		=   UrlGB2312Decode;
			this->UrlUTF8Encode			=   UrlUTF8Encode;
			this->UrlUTF8Decode			=   UrlUTF8Decode;
			this->WidebyteToAnsi		=	WidebyteToAnsi;
			this->AnsiToWidebyte		=	AnsiToWidebyte;
			this->UTF8ToAnsi			=	UTF8ToAnsi;
			this->UTF8ToWidebyte		=	UTF8ToWidebyte;
			this->AnsiToUTF8			=	AnsiToUTF8;
			this->WidebyteToUTF8		=	WidebyteToUTF8;
		}
		else
		{
			::MessageBoxA(NULL, "找不到LibcurlHttp.dll模块", "找不到模块", MB_OK | MB_ICONERROR);
		}
	}
	~HTTP_CLIENT()
	{
		if (hDll)
		{
			FreeLibrary(hDll);
			hDll = NULL;
		}
	}

	FN_CreateHttp			CreateHttp;
	FN_ReleaseHttp			ReleaseHttp;
	FN_setTimeout			setTimeout;
	FN_setRequestHeader		setRequestHeader;
	FN_setUserAgent			setUserAgent;
	FN_setCustomMothod		setCustomMothod;
	FN_get					get;
	FN_get_a				get_a;
	FN_post					post;
	FN_post_a				post_a;
	FN_download				download;
	FN_postForm				postForm;
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

	HMODULE hDll;
};
__declspec(selectany) HTTP_CLIENT* HTTP_CLIENT::s_ins = NULL;

#endif