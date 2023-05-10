#pragma once
#include <string>

#ifdef LIBCURLHTTP_EXPORTS
#define LIBCURLHTTP_API extern "C" __declspec(dllexport)
#else
#define LIBCURLHTTP_API extern "C" __declspec(dllimport)
#endif

typedef enum FieldType
{
	ftNormal,
	ftFile
} FieldType;

typedef struct FormField
{
	FieldType fieldType;
	std::string fieldName;
	std::string fieldValue;
	std::string fileName;//ֻ�����ļ����Ͳ���Ч
} FormField;

typedef struct MultipartField
{
	std::string contenxtData;	//�ύ����ֱ��ָ�������ݣ����ֶ�������ʱ��filePath�ֶ���Ч
	std::string filePath;		//�ύ�����ļ���contenxtData������ʱ�����ֶ���Ч
	
	std::string fileName;			//ָ���ļ���	
	std::string multipartName;		//����
	std::string mimeType;			//Mime type

} MultipartField;

//���Ȼص�ԭ�� int PROGRESS_CALLBACK(double dltotal, double dlnow, double ultotal, double ulnow, void* userData);
typedef int(*FN_PROGRESS_CALLBACK)(
	double dltotal,
	double dlnow,
	double ultotal,
	double ulnow,
	void* userData);

class LibcurlHttp
{
public:
	//���ó�ʱʱ������λ ��
	virtual void setTimeout(int t) = 0;
	//��������ͷ
	virtual void setRequestHeader(const char* key, const char* value) = 0;
	//����UserAgent
	virtual void setUserAgent(const char* val) = 0;
	//����cookies
	//virtual void setCookies();

	//�����Զ��巽��
	//  DELETE PUT HEAD OPTIONS FUCK������ȫ��д
	virtual void setCustomMothod(const char* mothod) = 0;

	//���ý��Ȼص�
	virtual void setProgress(FN_PROGRESS_CALLBACK progress, void* userData) = 0;

	//�����Ƿ��Զ��ض���
	virtual void setAutoRedirect(bool autoRedirect) = 0;
	//�����Զ��ض������
	virtual void setMaxRedirect(int maxRedirect) = 0;

	//http get
	virtual int get(const char* url) = 0;
	//http get
	//����������const char* key1, const char* val1, const char* key2, const char* val2, ����, NULL
	//  ��������Ϊ�ɶԵ���ʽ���֣����Һ���һ����һ��NULL��־���������Ľ�����
	virtual int get_a(const char* url, ...) = 0;
	virtual int get_b(const char* url, va_list argv) = 0;

	//http post
	virtual int post(const char* url, const char* content, int contentLen, const char* contentType = "application/x-www-form-urlencoded") = 0;
	//http post
	//����������const char* key1, const char* val1, const char* key2, const char* val2, ����, NULL
	//  ��������Ϊ�ɶԵ���ʽ���֣����Һ���һ����һ��NULL��־���������Ľ�����
	virtual int post_a(const char* url, ...) = 0;
	virtual int post_b(const char* url, va_list argv) = 0;

	//�ύ��
	virtual int postForm(const char* url, FormField* formDataArr, int nCountFormData) = 0;
	// url, fieldtype1(1:��ͨ�ֶΣ�2:file�ֶ�), fieldname1, fieldvalue1, [���fieldtype1==2�����]fileName1,    
	//      fieldtype2(1:��ͨ�ֶΣ�2:file�ֶ�), fieldname2, fieldvalue2, [���fieldtype2==2�����]fileName2, 
	//      ����, NULL
	virtual int postForm_a(const char* url, ...) = 0;
	virtual int postForm_b(const char* url, va_list argv) = 0;

	//�ύmultipart
	virtual int postMultipart(const char* url, MultipartField* multipartDataArr, int nCountMultipartData) = 0;
		
	//���������ļ�������
	virtual int download(const char* url, const char* localFileName = NULL) = 0;
	
	//����ύ���body����
	virtual const char* getBody(int& len) = 0;
	//����ύ��ķ���code
	virtual int getCode() = 0;
	//���Response headers
	virtual int getResponseHeaderKeysCount() = 0;
	virtual const char* getResponseHeaderKey(int i) = 0;
	virtual int getResponseHeadersCount(const char* key) = 0;
	virtual const char* getResponseHeader(const char* key, int i) = 0;

	//urlת��
	virtual const char* UrlGB2312Encode(const char * strIn) = 0;
	//url����
	virtual const char* UrlGB2312Decode(const char * strIn) = 0;

	//urlת��
	virtual const char* UrlUTF8Encode(const char * strIn) = 0;
	//url����
	virtual const char* UrlUTF8Decode(const char * strIn) = 0;

	//�ַ�ת��
	virtual const char* WidebyteToAnsi(const wchar_t * strIn) = 0;
	virtual const wchar_t* AnsiToWidebyte(const char * strIn) = 0;
	virtual const char* UTF8ToAnsi(const char * strIn) = 0;
	virtual const wchar_t* UTF8ToWidebyte(const char * strIn) = 0;
	virtual const char* AnsiToUTF8(const char * strIn) = 0;
	virtual const char* WidebyteToUTF8(const wchar_t * strIn) = 0;
};


//����http����
LIBCURLHTTP_API LibcurlHttp* CreateHttp(void);
//����http����
LIBCURLHTTP_API void ReleaseHttp(LibcurlHttp* p);

//�Ե����ķ�ʽ��ע�ͼ�LibcurlHttp
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
LIBCURLHTTP_API int postForm(LibcurlHttp* http, const char* url, FormField* formDataArr, int nCountFormData);
LIBCURLHTTP_API int postForm_a(LibcurlHttp* http, const char* url, ...);
LIBCURLHTTP_API int postMultipart(LibcurlHttp* http, const char* url, MultipartField* multipartDataArr, int nCountMultipartData);

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





class HTTP_CLIENT
{
public:
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
	typedef int(*FN_postForm)(LibcurlHttp* http, const char* url, FormField* formDataArr, int nCoutFormData);
	typedef int(*FN_postForm_a)(LibcurlHttp* http, const char* url, ...);
	typedef int(*FN_postMultipart)(LibcurlHttp* http, const char* url, MultipartField* multipartDataArr, int nCountMultipartData);
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
		DEF_PROC(hDll, setCustomMothod);
		DEF_PROC(hDll, setProgress);
		DEF_PROC(hDll, setAutoRedirect);
		DEF_PROC(hDll, setMaxRedirect);
		DEF_PROC(hDll, get);
		DEF_PROC(hDll, get_a);
		DEF_PROC(hDll, post);
		DEF_PROC(hDll, post_a);
		DEF_PROC(hDll, download);
		DEF_PROC(hDll, postForm);
		DEF_PROC(hDll, postForm_a);
		DEF_PROC(hDll, postMultipart);
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
	FN_postMultipart		postMultipart;
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
		if (!s_ins)
			s_ins = new HTTP_CLIENT;
		return *s_ins;
	}

	static void Rel()
	{
		if (s_ins)
		{
			delete s_ins;
			s_ins = NULL;
		}
	}

	static HMODULE LoadLibraryFromCurrentDir(const char* dllName)
	{
		char selfPath[MAX_PATH];
		MEMORY_BASIC_INFORMATION mbi;
		HMODULE hModule = ((::VirtualQuery(LoadLibraryFromCurrentDir, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
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
			::MessageBoxA(NULL, ("�Ҳ���" + modulePath + "ģ��:" + buf).c_str(), "�Ҳ���ģ��", MB_OK | MB_ICONERROR);
		}
		return hDll;
	}

	HMODULE hDll;
	static HTTP_CLIENT* s_ins;
};

#ifndef LIBCURLHTTP_EXPORTS
__declspec(selectany) HTTP_CLIENT* HTTP_CLIENT::s_ins = NULL;
#endif