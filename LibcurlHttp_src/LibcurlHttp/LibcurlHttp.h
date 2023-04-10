#pragma once

#ifdef LIBCURLHTTP_EXPORTS
#define LIBCURLHTTP_API extern "C" __declspec(dllexport)
#else
#define LIBCURLHTTP_API extern "C" __declspec(dllimport)
#endif

//进度回调原型 int PROGRESS_CALLBACK(double dltotal, double dlnow, double ultotal, double ulnow, void* userData);
typedef int(*FN_PROGRESS_CALLBACK)(
	double dltotal,
	double dlnow,
	double ultotal,
	double ulnow,
	void* userData);

typedef struct FORM_FIELD
{
	int fieldType;//0=普通字段；1=文件字段
	char fieldName[256];
	char fieldValue[MAX_PATH];//字段值或文件路径
	char fileName[MAX_PATH];//只有在文件类型才有效
} FORM_FIELD;

#define FillFormField(target, _fieldType, _fieldName, _fieldValue, _fileName) \
	target.fieldType = _fieldType; \
	if (_fieldName) strcpy(target.fieldName, _fieldName); \
	if (_fieldValue) strcpy(target.fieldValue, _fieldValue); \
	if (_fileName) strcpy(target.fileName, _fileName); 

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
	virtual void setCustomMothod(const char* mothod) = 0;

	//设置进度回调
	virtual void setProgress(FN_PROGRESS_CALLBACK progress, void* userData) = 0;

	//设置是否自动重定向
	virtual void setAutoRedirect(bool autoRedirect) = 0;
	//设置自动重定向次数
	virtual void setMaxRedirect(int maxRedirect) = 0;

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
	virtual int postForm(const char* url, FORM_FIELD* formData, int nSizeFormData) = 0;
	// url, fieldtype1(1:普通字段；2:file字段), fieldname1, fieldvalue1, [如果fieldtype1==2则存在]fileName1,    
	//      fieldtype2(1:普通字段；2:file字段), fieldname2, fieldvalue2, [如果fieldtype2==2则存在]fileName2, 
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


//创建http对象
LIBCURLHTTP_API LibcurlHttp* CreateHttp(void);
//销毁http对象
LIBCURLHTTP_API void ReleaseHttp(LibcurlHttp* p);

//以单例的方式，注释见LibcurlHttp
LIBCURLHTTP_API void setTimeout(LibcurlHttp* http, int timeout);
LIBCURLHTTP_API void setRequestHeader(LibcurlHttp* http, const char* key, const char* value);
LIBCURLHTTP_API void setUserAgent(LibcurlHttp* http, const char* val);
LIBCURLHTTP_API void setCustomMothod(LibcurlHttp* http, const char* mothod);
LIBCURLHTTP_API void setProgress(LibcurlHttp* http, FN_PROGRESS_CALLBACK progress, void* userData);
LIBCURLHTTP_API void setAutoRedirect(LibcurlHttp* http, bool autoRedirect);
LIBCURLHTTP_API void setMaxRedirect(LibcurlHttp* http, int maxRedirect);

LIBCURLHTTP_API int get(LibcurlHttp* http, const char* url);
LIBCURLHTTP_API int get_a(LibcurlHttp* http, const char* url, ...);
LIBCURLHTTP_API int post(LibcurlHttp* http, const char* url, const char* content, int contentLen, const char* contentType = "application/x-www-form-urlencoded");
LIBCURLHTTP_API int post_a(LibcurlHttp* http, const char* url, ...);
LIBCURLHTTP_API int download(LibcurlHttp* http, const char* url, const char* localFileName = NULL);
LIBCURLHTTP_API int postForm(LibcurlHttp* http, const char* url, FORM_FIELD* formData, int nSizeFormData);
LIBCURLHTTP_API int postForm_a(LibcurlHttp* http, const char* url, ...);

LIBCURLHTTP_API const char* getBody(LibcurlHttp* http, int& len);
LIBCURLHTTP_API int getCode(LibcurlHttp* http);
LIBCURLHTTP_API int getResponseHeaderKeysCount(LibcurlHttp* http);
LIBCURLHTTP_API const char* getResponseHeaderKey(LibcurlHttp* http, int i);
LIBCURLHTTP_API int getResponseHeadersCount(LibcurlHttp* http, const char* key);
LIBCURLHTTP_API const char* getResponseHeader(LibcurlHttp* http, const char* key, int i);

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



#ifndef LIBCURLHTTP_EXPORTS
typedef LibcurlHttp* (*FN_CreateHttp)(void);
typedef void(*FN_ReleaseHttp)(LibcurlHttp* p);
typedef void(*FN_setTimeout)(LibcurlHttp* http, int timeout);
typedef void(*FN_setRequestHeader)(LibcurlHttp* http, const char* key, const char* value);
typedef void(*FN_setUserAgent)(LibcurlHttp* http, const char* val);
typedef void(*FN_setCustomMothod)(LibcurlHttp* http, const char* mothod);
typedef void(*FN_setProgress)(LibcurlHttp* http, FN_PROGRESS_CALLBACK progress, void* userData);
typedef void(*FN_setAutoRedirect)(LibcurlHttp* http, bool autoRedirect);
typedef void(*FN_setMaxRedirect)(LibcurlHttp* http, int maxRedirect);
typedef int(*FN_get)(LibcurlHttp* http, const char* url);
typedef int(*FN_get_a)(LibcurlHttp* http, const char* url, ...);
typedef int(*FN_post)(LibcurlHttp* http, const char* url, const char* content, int contentLen, const char* contentType);
typedef int(*FN_post_a)(LibcurlHttp* http, const char* url, ...);
typedef int(*FN_download)(LibcurlHttp* http, const char* url, const char* localFileName);
typedef int(*FN_postForm)(LibcurlHttp* http, const char* url, FORM_FIELD* formData, int nSizeFormData);
typedef int(*FN_postForm_a)(LibcurlHttp* http, const char* url, ...);
typedef const char* (*FN_getBody)(LibcurlHttp* http, int& len);
typedef int (*FN_getCode)(LibcurlHttp* http);
typedef int (*FN_getResponseHeaderKeysCount)(LibcurlHttp* http);
typedef const char* (*FN_getResponseHeaderKey)(LibcurlHttp* http, int i);
typedef int (*FN_getResponseHeadersCount)(LibcurlHttp* http, const char* key);
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
	FN_##name name = (FN_##name)::GetProcAddress(hDll, #name)

#define INIT_LIBCURLHTTP(hDll) \
	HMODULE __hDll__ = (hDll); \
	DEF_PROC(__hDll__, CreateHttp); \
	DEF_PROC(__hDll__, ReleaseHttp); \
	DEF_PROC(__hDll__, setTimeout); \
	DEF_PROC(__hDll__, setRequestHeader); \
	DEF_PROC(__hDll__, setUserAgent); \
	DEF_PROC(__hDll__, setCustomMothod); \
	DEF_PROC(__hDll__, setProgress); \
	DEF_PROC(__hDll__, setAutoRedirect); \
	DEF_PROC(__hDll__, setMaxRedirect); \
	DEF_PROC(__hDll__, get); \
	DEF_PROC(__hDll__, get_a); \
	DEF_PROC(__hDll__, post); \
	DEF_PROC(__hDll__, post_a); \
	DEF_PROC(__hDll__, download); \
	DEF_PROC(__hDll__, postForm); \
	DEF_PROC(__hDll__, postForm_a); \
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
			this->setProgress			=	setProgress;
			this->setAutoRedirect		=	setAutoRedirect;
			this->setMaxRedirect		=	setMaxRedirect;
			this->get					=   get;
			this->get_a					=   get_a;
			this->post					=   post;
			this->post_a				=   post_a;
			this->download				=   download; 
			this->postForm				=   postForm; 
			this->postForm_a			=   postForm_a; 
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
	FN_setProgress			setProgress;
	FN_setAutoRedirect		setAutoRedirect;
	FN_setMaxRedirect		setMaxRedirect;
	FN_get					get;
	FN_get_a				get_a;
	FN_post					post;
	FN_post_a				post_a;
	FN_download				download;
	FN_postForm				postForm;
	FN_postForm_a			postForm_a;
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