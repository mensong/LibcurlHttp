// LibcurHttp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LibcurlHttp.h"
#include "..\HttpClient\HttpFileDownload.h"
#include "..\HttpHelper\UrlCoding.h"
#include "..\HttpHelper\Convertor.h"


class LibcurlHttpImp
	: public LibcurlHttp
{
public:
	LibcurlHttpImp()
		: m_timeout(0)
		, m_responseCode(0)
	{
		ms_urlEncodeEscape.insert('/');
		ms_urlEncodeEscape.insert(':');
		ms_urlEncodeEscape.insert('%');
		ms_urlEncodeEscape.insert('&');
		ms_urlEncodeEscape.insert('?');
		ms_urlEncodeEscape.insert('\\');
		ms_urlEncodeEscape.insert('=');

		m_userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36";
	}

	virtual void setTimeout(int t) override
	{
		m_timeout = t;
	}
	
	virtual int get(const char* url) override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		//return m_http.get(sUrl.c_str(), dealRedirect);

		HttpClient httpClient;
		httpClient.SetUrl(sUrl);
		httpClient.SetTimtout(m_timeout);
		httpClient.SetUserAgent(m_userAgent);
		httpClient.SetHeaders(m_requestHeaders);
		httpClient.SetCustomMothod(m_customMothod);
				
		httpClient.Do();

		m_responseCode = httpClient.GetHttpCode();
		m_responseBody = httpClient.GetBody();
		m_responseHeaders = httpClient.GetResponseHeaders();
		
		m_customMothod = "";
		return m_responseCode;
	}	

	virtual int get_a(const char* url, ...)
	{
		va_list argv;
		va_start(argv, url);

		int code = get_b(url, argv);

		va_end(argv);

		return code;
	}

	virtual int get_b(const char* url, va_list argv)
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

		return get(uri.c_str());
	}

	virtual int post(const char* url, const char* content, int contentLen, const char* contentType = "application/x-www-form-urlencoded") override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);

		HttpClient httpClient;
		httpClient.SetUrl(sUrl);
		httpClient.SetTimtout(m_timeout);
		httpClient.SetUserAgent(m_userAgent);
		httpClient.SetHeaders(m_requestHeaders);
		httpClient.SetHeader("Content-Type", contentType);
		httpClient.SetCustomMothod(m_customMothod);

		std::string sData(content, contentLen);
		httpClient.SetNormalPostData(sData);

		httpClient.Do();

		m_responseCode = httpClient.GetHttpCode();
		m_responseBody = httpClient.GetBody();
		m_responseHeaders = httpClient.GetResponseHeaders();

		m_customMothod = "";
		return m_responseCode;
	}
	
	virtual int post_a(const char* url, ...) override
	{
		va_list argv;
		va_start(argv, url);

		int code = post_b(url, argv);

		va_end(argv);

		return code;
	}

	virtual int post_b(const char* url, va_list argv)
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

		return post(url, sContet.c_str(), (int)sContet.size());
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
		client.SetTimtout(m_timeout);
		client.SetUserAgent(m_userAgent);
		client.SetHeaders(m_requestHeaders);
		client.SetCustomMothod(m_customMothod);

		client.Do();

		m_responseCode = client.GetHttpCode();
		m_responseBody = client.GetBody();
		m_responseHeaders = client.GetResponseHeaders();

		m_customMothod = "";
		return m_responseCode;
	}
		
	virtual void setRequestHeader(const char* key, const char* value) override
	{
		if (!key || key[0] == '\0')
			return;
		if (!value || value[0] == '\0')
		{
			m_requestHeaders.erase(key);
			return;
		}

		m_requestHeaders[key] = value;
	}

	virtual void setUserAgent(const char* val) override
	{
		m_userAgent = val;
	}

	virtual void setCustomMothod(const char* mothod) override
	{
		m_customMothod = mothod;
	}

	virtual int download(const char* url, const char* localFileName=NULL) override
	{
		HttpFileDownload downloader;

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		downloader.SetUrl(sUrl.c_str());
		if (localFileName)
			downloader.SetFileName(localFileName);
		downloader.SetTimtout(m_timeout);
		downloader.SetUserAgent(m_userAgent);
		downloader.SetHeaders(m_requestHeaders);
		downloader.SetCustomMothod(m_customMothod);
		
		downloader.Do();

		m_responseCode = downloader.GetHttpCode();
		m_responseHeaders = downloader.GetResponseHeaders();

		m_customMothod = "";
		return m_responseCode;
	}
	
	virtual const char* getBody(int& len) override
	{
		const std::string& sBody = m_responseBody;
		len = (int)sBody.size();
		return sBody.c_str();
	}

	virtual int getCode() override
	{
		return m_responseCode;
	}

	virtual int getResponseHeaderKeysCount() override
	{
		return m_responseHeaders.size();
	}

	virtual const char* getResponseHeaderKey(int i) override
	{
		if (i >= m_responseHeaders.size())
			return NULL;

		ResponseHeaderFields::iterator it = m_responseHeaders.begin();
		while (i--)
		{
			++it;
		}

		return it->first.c_str();
	}

	virtual int getResponseHeadersCount(const char* key) override
	{
		if (!key)
		{
			return 0;
		}

		ResponseHeaderFields::iterator itFinder = m_responseHeaders.find(key);
		if (itFinder == m_responseHeaders.end())
		{
			return 0;
		}
		
		return itFinder->second.size();		
	}

	virtual const char* getResponseHeader(const char* key, int i) override
	{
		if (!key)
		{
			return NULL;
		}

		ResponseHeaderFields::iterator itFinder = m_responseHeaders.find(key);
		if (itFinder == m_responseHeaders.end())
		{
			return NULL;
		}

		if (i >= itFinder->second.size())
			return NULL;

		return itFinder->second[i].c_str();
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
	int m_timeout;
	std::string m_userAgent;
	std::string m_customMothod;
	std::map<std::string, std::string> m_requestHeaders;

	int m_responseCode;
	std::string m_responseBody;
	ResponseHeaderFields m_responseHeaders;

	std::set<char> ms_urlEncodeEscape;	
};

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

LIBCURLHTTP_API void setCustomMothod(const char* mothod)
{
	return _instance()->setCustomMothod(mothod);
}

LIBCURLHTTP_API int get(const char* url)
{
	return _instance()->get(url);
}

LIBCURLHTTP_API int get_a(const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	int ret = _instance()->get_b(url, argv);

	va_end(argv);

	return ret;
}

LIBCURLHTTP_API int post(const char* url, const char* content, int contentLen, const char* contentType /*= "application/x-www-form-urlencoded"*/)
{
	return _instance()->post(url, content, contentLen, contentType);
}

LIBCURLHTTP_API int post_a(const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	int ret = _instance()->post_b(url, argv);

	va_end(argv);

	return ret;
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

LIBCURLHTTP_API const char* getBody(int& len)
{
	return _instance()->getBody(len);
}

LIBCURLHTTP_API int getCode()
{
	return _instance()->getCode();
}

LIBCURLHTTP_API int getResponseHeaderKeysCount()
{
	return _instance()->getResponseHeaderKeysCount();
}

LIBCURLHTTP_API const char* getResponseHeaderKey(int i)
{
	return _instance()->getResponseHeaderKey(i);
}

LIBCURLHTTP_API int getResponseHeadersCount(const char* key)
{
	return _instance()->getResponseHeadersCount(key);
}

LIBCURLHTTP_API const char* getResponseHeader(const char* key, int i)
{
	return _instance()->getResponseHeader(key, i);
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
