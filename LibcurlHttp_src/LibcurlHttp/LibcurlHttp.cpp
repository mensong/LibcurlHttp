// LibcurHttp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LibcurlHttp.h"
#include "..\HttpHelper\HttpHelper.h"
#include "..\HttpClient\HttpFileDownload.h"
#include "..\HttpHelper\UrlCoding.h"
#include "..\HttpHelper\Convertor.h"


class LibcurlHttpImp
	: public LibcurlHttp
{
private:
	std::set<char> ms_urlEncodeEscape;

public:
	LibcurlHttpImp()
	{
		ms_urlEncodeEscape.insert('/');
		ms_urlEncodeEscape.insert(':');
		ms_urlEncodeEscape.insert('%');
		ms_urlEncodeEscape.insert('&');
		ms_urlEncodeEscape.insert('?');
		ms_urlEncodeEscape.insert('\\');
		ms_urlEncodeEscape.insert('=');
	}

	virtual void setTimeout(int t) override
	{
		m_http.timeOut = t;
	}
	
	virtual int get(const char* url, bool dealRedirect = true) override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		return m_http.get(sUrl.c_str(), dealRedirect);
	}	

	virtual int get_a(const char* url, bool dealRedirect, ...)
	{
		va_list argv;
		va_start(argv, dealRedirect);

		int code = get_b(url, dealRedirect, argv);

		va_end(argv);

		return code;
	}

	virtual int get_b(const char* url, bool dealRedirect, va_list argv)
	{
		std::string sContet;

		char* key = NULL;
		char* val = NULL;

		do
		{
			key = va_arg(argv, char*);
			if (!key)
				break;
			val = va_arg(argv, char*);
			if (!val)
				break;

			if (sContet.empty())
				sContet = key + std::string("=") + val;
			else
				sContet += std::string("&") + key + std::string("=") + val;
		} while (1);

		std::string uri = url + std::string("?") + sContet;

		return get(uri.c_str(), dealRedirect);
	}

	virtual int post(const char* url, const char* content, int contentLen, bool dealRedirect = true, const char* contentType = "application/x-www-form-urlencoded") override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		return m_http.post(sUrl.c_str(), content, contentLen, dealRedirect, contentType);
	}
	
	virtual int post_a(const char* url, bool dealRedirect, ...) override
	{
		va_list argv;
		va_start(argv, dealRedirect);

		int code = post_b(url, dealRedirect, argv);

		va_end(argv);

		return code;
	}

	virtual int post_b(const char* url, bool dealRedirect, va_list argv)
	{
		std::string sContet;

		char* key = NULL;
		char* val = NULL;

		do
		{
			key = va_arg(argv, char*);
			if (!key)
				break;
			val = va_arg(argv, char*);
			if (!val)
				break;

			if (sContet.empty())
				sContet = key + std::string("=") + val;
			else
				sContet += std::string("&") + key + std::string("=") + val;
		} while (1);

		return post(url, sContet.c_str(), (int)sContet.size(), dealRedirect);
	}

	virtual int postForm_a(const char* url, ...) override
	{
		va_list argv;
		va_start(argv, url);

		int code = postForm_b(url, argv);

		va_end(argv);

		return code;
	}
	
	virtual int postForm_b(const char* url, va_list argv) override
	{
		HttpClient client;

		int fieldType = 0;
		char* key = NULL;
		char* val = NULL;
		char* fileName = NULL;
		
		do
		{
			bool fieldValid = true;
			fieldType = va_arg(argv, int);
			if (!fieldType)
				break;
			key = va_arg(argv, char*);
			if (!key)
				break;
			val = va_arg(argv, char*);
			if (!val)
				fieldValid = false;//允许传null值过来，但一旦传null过来，则表示此field为无效field，将不参与传递
			if (fieldType == 2)
				fileName = va_arg(argv, char*);

			if (fieldValid)
			{
				FormField ff;
				ff.fieldName = key;
				ff.fieldValue = val;
				if (fieldType == 1)
					ff.fieldType = ftNormal;
				else if (fieldType == 2)
				{
					ff.fieldType = ftFile;
					if (fileName)
						ff.fileName = fileName;
				}

				client.AddFormField(ff);
			}
		} while (1);
		
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		client.SetUrl(sUrl.c_str());
		client.SetTimtout(m_http.timeOut);
		client.SetUserAgent(m_http.userAgent);
		client.SetHeaders(m_http.headers);

		client.Do();
		client.Wait();

		int retCode = client.GetHttpCode();
		m_http.response.body = client.GetBody();
		return retCode;
	}

	virtual const char* getLocation() override
	{
		return m_http.getLocation();
	}
	
	virtual const char* getBody(int& len) override
	{
		const std::string& sBody = m_http.response.body;
		len = (int)sBody.size();
		return sBody.c_str();
	}
	
	virtual int getCode() override
	{
		return m_http.response.code;
	}

	virtual void setRequestHeader(const char* key, const char* value) override
	{
		m_http.headers[key] = value;
	}

	virtual void setUserAgent(const char* val) override
	{
		m_http.userAgent = val;
	}
	
	virtual void setMaxRedirectNum(int num) override
	{
		m_http.maxRedirectNum = num;
	}		

	virtual int download(const char* url, const char* localFileName=NULL) override
	{
		HttpFileDownload downloader;

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		downloader.SetUrl(sUrl.c_str());
		if (localFileName)
			downloader.SetFileName(localFileName);
		downloader.SetTimtout(m_http.timeOut);
		downloader.SetUserAgent(m_http.userAgent);
		downloader.SetHeaders(m_http.headers);
		
		downloader.Do();
		downloader.Wait();

		int retCode = downloader.GetHttpCode();
		return retCode;
	}
	
	virtual const char* UrlGB2312Encode(const char * strIn) override
	{
		static std::string ms;
		std::string s = UrlCoding::UrlGB2312Encode(strIn);
		ms = s;
		return ms.c_str();
	}

	virtual const char* UrlGB2312Decode(const char * strIn) override
	{
		static std::string ms;
		std::string s = UrlCoding::UrlGB2312Decode(strIn);
		ms = s;
		return ms.c_str();
	}

	virtual const char* UrlUTF8Encode(const char * strIn) override
	{
		static std::string ms;
		std::string s = UrlCoding::UrlUTF8Encode(strIn);
		ms = s;
		return ms.c_str();
	}

	virtual const char* UrlUTF8Decode(const char * strIn) override
	{
		static std::string ms;
		std::string s = UrlCoding::UrlUTF8Decode(strIn);
		ms = s;
		return ms.c_str();
	}


	virtual const char* WidebyteToAnsi(const wchar_t * strIn) override
	{
		static std::string ms;
		if (!GL::WideByte2Ansi(ms, strIn))
			ms = "";
		return ms.c_str();
	}


	virtual const wchar_t* AnsiToWidebyte(const char * strIn) override
	{
		static std::wstring ms;
		if (!GL::Ansi2WideByte(ms, strIn))
			ms = L"";
		return ms.c_str();
	}

	virtual const char* UTF8ToAnsi(const char * strIn) override
	{
		static std::string ms;
		if (!GL::Utf82Ansi(ms, strIn))
			ms = "";
		return ms.c_str();
	}


	virtual const wchar_t* UTF8ToWidebyte(const char * strIn) override
	{
		static std::wstring ms;
		if (!GL::Utf82WideByte(ms, strIn))
			ms = L"";
		return ms.c_str();
	}


	virtual const char* AnsiToUTF8(const char * strIn) override
	{
		static std::string ms;
		if (!GL::Ansi2Utf8(ms, strIn))
			ms = "";
		return ms.c_str();
	}


	virtual const char* WidebyteToUTF8(const wchar_t * strIn) override
	{
		static std::string ms;
		if (!GL::WideByte2Utf8(ms, strIn))
			ms = "";
		return ms.c_str();
	}

private:
	HttpHelper m_http;
};


LIBCURLHTTP_API void LoadCookies(const char* file)
{
	HttpHelper::LoadCookies(file);
}

LIBCURLHTTP_API void SaveCookies(const char* file)
{
	HttpHelper::SaveCookies(file);
}

LIBCURLHTTP_API LibcurlHttp* CreateHttp(void)
{
	return new LibcurlHttpImp;
}

LIBCURLHTTP_API void ReleaseHttp(LibcurlHttp* p)
{
	if (p)
		delete p;
}


LibcurlHttp* _instance()
{
	static LibcurlHttp* s_instance = NULL;
	if (!s_instance)
		s_instance = new LibcurlHttpImp;
	return s_instance;
}

LIBCURLHTTP_API void setTimeout(int timeout)
{
	return _instance()->setTimeout(timeout);
}

LIBCURLHTTP_API void setRequestHeader(const char* key, const char* value)
{
	return _instance()->setRequestHeader(key, value);
}

LIBCURLHTTP_API void setUserAgent(const char* val)
{
	return _instance()->setUserAgent(val);
}

LIBCURLHTTP_API void setMaxRedirectNum(int num)
{
	return _instance()->setMaxRedirectNum(num);
}

LIBCURLHTTP_API int get(const char* url, bool dealRedirect /*= true*/)
{
	return _instance()->get(url, dealRedirect);
}

LIBCURLHTTP_API int get_a(const char* url, bool dealRedirect, ...)
{
	va_list argv;
	va_start(argv, dealRedirect);

	int ret = _instance()->get_b(url, dealRedirect, argv);

	va_end(argv);

	return ret;
}

LIBCURLHTTP_API int post(const char* url, const char* content, int contentLen, bool dealRedirect /*= true*/, const char* contentType /*= "application/x-www-form-urlencoded"*/)
{
	return _instance()->post(url, content, contentLen, dealRedirect, contentType);
}

LIBCURLHTTP_API int post_a(const char* url, bool dealRedirect, ...)
{
	va_list argv;
	va_start(argv, dealRedirect);

	int ret = _instance()->post_b(url, dealRedirect, argv);

	va_end(argv);

	return ret;
}

LIBCURLHTTP_API const char* getLocation()
{
	return _instance()->getLocation();
}

LIBCURLHTTP_API const char* getBody(int& len)
{
	return _instance()->getBody(len);
}

LIBCURLHTTP_API int getCode()
{
	return _instance()->getCode();
}

LIBCURLHTTP_API int download(const char* url, const char* localFileName/*=NULL*/)
{
	return _instance()->download(url, localFileName);
}

LIBCURLHTTP_API int postForm(const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	int ret = _instance()->postForm_b(url, argv);

	va_end(argv);

	return ret;
}

LIBCURLHTTP_API const char* UrlGB2312Encode(const char * strIn)
{
	return _instance()->UrlGB2312Encode(strIn);
}

LIBCURLHTTP_API const char* UrlGB2312Decode(const char * strIn)
{
	return _instance()->UrlGB2312Decode(strIn);
}

LIBCURLHTTP_API const char* UrlUTF8Encode(const char * strIn)
{
	return _instance()->UrlUTF8Encode(strIn);
}

LIBCURLHTTP_API const char* UrlUTF8Decode(const char * strIn)
{
	return _instance()->UrlUTF8Decode(strIn);
}

LIBCURLHTTP_API const char* WidebyteToAnsi(const wchar_t * strIn)
{
	return _instance()->WidebyteToAnsi(strIn);
}

LIBCURLHTTP_API const wchar_t* AnsiToWidebyte(const char * strIn)
{
	return _instance()->AnsiToWidebyte(strIn);
}

LIBCURLHTTP_API const char* UTF8ToAnsi(const char * strIn)
{
	return _instance()->UTF8ToAnsi(strIn);
}

LIBCURLHTTP_API const wchar_t* UTF8ToWidebyte(const char * strIn)
{
	return _instance()->UTF8ToWidebyte(strIn);
}

LIBCURLHTTP_API const char* AnsiToUTF8(const char * strIn)
{
	return _instance()->AnsiToUTF8(strIn);
}

LIBCURLHTTP_API const char* WidebyteToUTF8(const wchar_t * strIn)
{
	return _instance()->WidebyteToUTF8(strIn);
}

