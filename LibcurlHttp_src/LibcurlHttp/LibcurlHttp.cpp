// LibcurHttp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LibcurlHttp.h"
#include "HttpClientFC.h"
#include "HttpFileDownloadFC.h"
#include "..\HttpHelper\UrlCoding.h"
#include "..\HttpHelper\Convertor.h"


class LibcurlHttpImp
	: public LibcurlHttp
{
public:
	LibcurlHttpImp()
		: m_timeout(0)
		, m_responseCode(0)
		, m_progressCallback(NULL)
		, m_progressUserData(NULL)
		, m_autoRedirect(true)
		, m_maxRedirect(5)
	{
		ms_urlEncodeEscape.insert('/');
		ms_urlEncodeEscape.insert(':');
		ms_urlEncodeEscape.insert('%');
		ms_urlEncodeEscape.insert('&');
		ms_urlEncodeEscape.insert('?');
		ms_urlEncodeEscape.insert('\\');
		ms_urlEncodeEscape.insert('=');

		m_userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/9999.9999.9999.9999 Safari/9999.9999";
	}

	virtual void setTimeout(int t) override
	{
		m_timeout = t;
	}

	virtual void setProgress(FN_PROGRESS_CALLBACK progress, void* userData) override
	{
		m_progressCallback = progress;
		m_progressUserData = userData;
	}

	virtual void setAutoRedirect(bool autoRedirect) override
	{
		m_autoRedirect = autoRedirect;
	}


	virtual void setMaxRedirect(int maxRedirect) override
	{
		m_maxRedirect = maxRedirect;
	}
	
	virtual int get(const char* url) override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		//return m_http.get(sUrl.c_str(), dealRedirect);

		HttpClientFC httpClient;
		httpClient.SetUrl(sUrl);
		httpClient.SetTimtout(m_timeout);
		httpClient.SetUserAgent(m_userAgent);
		httpClient.SetHeaders(m_requestHeaders);
		httpClient.SetCustomMothod(m_customMothod);
		httpClient.SetProgress(m_progressCallback, m_progressUserData);
		httpClient.SetAutoRedirect(m_autoRedirect);
		httpClient.SetMaxRedirect(m_maxRedirect);
				
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
				
		std::string uri = url;
		if (uri.find('?') == std::string::npos)
			uri += std::string("?") + sContet;
		else if (uri.size() > 0 && uri[uri.size()-1] != '?')//存在参数
			uri += std::string("&") + sContet;
		else//http://xxx/xx?
			uri += sContet;

		return get(uri.c_str());
	}

	virtual int post(const char* url, const char* content, int contentLen, const char* contentType = "application/x-www-form-urlencoded") override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);

		HttpClientFC httpClient;
		httpClient.SetUrl(sUrl);
		httpClient.SetTimtout(m_timeout);
		httpClient.SetUserAgent(m_userAgent);
		httpClient.SetHeaders(m_requestHeaders);
		httpClient.SetHeader("Content-Type", contentType);
		httpClient.SetCustomMothod(m_customMothod);
		httpClient.SetProgress(m_progressCallback, m_progressUserData);
		httpClient.SetAutoRedirect(m_autoRedirect);
		httpClient.SetMaxRedirect(m_maxRedirect);

		std::string sData(content, contentLen);
		httpClient.SetNormalPostData(sData);
		if (contentLen == 0)
		{//防止post空内容时出现411错误
			httpClient.SetHeader("Content-Length", "0");
		}

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

	virtual int postForm(const char* url, FormField* formDataArr, int nCountFormData)
	{
		HttpClientFC httpClient;

		for (int i = 0; i < nCountFormData; ++i)
		{
			if (formDataArr[i].fieldName == NULL)
				continue;
			
			httpClient.AddFormField(formDataArr[i]);
		}

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		httpClient.SetUrl(sUrl.c_str());
		httpClient.SetTimtout(m_timeout);
		httpClient.SetUserAgent(m_userAgent);
		httpClient.SetHeaders(m_requestHeaders);
		httpClient.SetCustomMothod(m_customMothod);
		httpClient.SetProgress(m_progressCallback, m_progressUserData);
		httpClient.SetAutoRedirect(m_autoRedirect);
		httpClient.SetMaxRedirect(m_maxRedirect);
		if (nCountFormData == 0)
		{//防止post空内容时出现411错误
			httpClient.SetHeader("Content-Length", "0");
		}

		httpClient.Do();

		m_responseCode = httpClient.GetHttpCode();
		m_responseBody = httpClient.GetBody();
		m_responseHeaders = httpClient.GetResponseHeaders();

		m_customMothod = "";
		return m_responseCode;
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
		HttpClientFC httpClient;

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

				httpClient.AddFormField(ff);
			}
		} while (1);
		
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		httpClient.SetUrl(sUrl.c_str());
		httpClient.SetTimtout(m_timeout);
		httpClient.SetUserAgent(m_userAgent);
		httpClient.SetHeaders(m_requestHeaders);
		httpClient.SetCustomMothod(m_customMothod);
		httpClient.SetProgress(m_progressCallback, m_progressUserData);
		httpClient.SetAutoRedirect(m_autoRedirect);
		httpClient.SetMaxRedirect(m_maxRedirect);
		if (httpClient.GetFormFields().size() == 0)
		{//防止post空内容时出现411错误
			httpClient.SetHeader("Content-Length", "0");
		}

		httpClient.Do();

		m_responseCode = httpClient.GetHttpCode();
		m_responseBody = httpClient.GetBody();
		m_responseHeaders = httpClient.GetResponseHeaders();

		m_customMothod = "";
		return m_responseCode;
	}
	
	virtual int postMultipart(const char* url, MultipartField* multipartDataArr, int nCountMultipartData) override
	{
		HttpClientFC httpClient;

		for (int i = 0; i < nCountMultipartData; ++i)
		{
			httpClient.AddMultipartField(multipartDataArr[i]);
		}

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		httpClient.SetUrl(sUrl.c_str());
		httpClient.SetTimtout(m_timeout);
		httpClient.SetUserAgent(m_userAgent);
		httpClient.SetHeaders(m_requestHeaders);
		httpClient.SetCustomMothod(m_customMothod);
		httpClient.SetProgress(m_progressCallback, m_progressUserData);
		httpClient.SetAutoRedirect(m_autoRedirect);
		httpClient.SetMaxRedirect(m_maxRedirect);
		if (nCountMultipartData == 0)
		{//防止post空内容时出现411错误
			httpClient.SetHeader("Content-Length", "0");
		}

		httpClient.Do();

		m_responseCode = httpClient.GetHttpCode();
		m_responseBody = httpClient.GetBody();
		m_responseHeaders = httpClient.GetResponseHeaders();

		m_customMothod = "";
		return m_responseCode;
	}

	virtual void setRequestHeader(const char* key, const char* value) override
	{
		if (!key || key[0] == '\0')
			return;
		if (!value)
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

	virtual int download(const char* url, const char* localFileName = NULL, char* downloadedFileName = NULL) override
	{
		HttpFileDownloadFC downloader;

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &ms_urlEncodeEscape);
		downloader.SetUrl(sUrl.c_str());
		if (localFileName)
			downloader.SetFileName(localFileName);
		downloader.SetTimtout(m_timeout);
		downloader.SetUserAgent(m_userAgent);
		downloader.SetHeaders(m_requestHeaders);
		downloader.SetCustomMothod(m_customMothod);
		downloader.SetProgress(m_progressCallback, m_progressUserData);
		downloader.SetAutoRedirect(m_autoRedirect);
		downloader.SetMaxRedirect(m_maxRedirect);
		
		downloader.Do();

		m_responseCode = downloader.GetHttpCode();
		m_responseHeaders = downloader.GetResponseHeaders();

		m_customMothod = "";

		if (downloadedFileName)
			strcpy(downloadedFileName, downloader.GetFileName().c_str());

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
		return (int)m_responseHeaders.size();
	}

	virtual const char* getResponseHeaderKey(int i) override
	{
		if (i >= (int)m_responseHeaders.size())
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
		
		return (int)itFinder->second.size();		
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

		if (i >= (int)itFinder->second.size())
			return NULL;

		return itFinder->second[i].c_str();
	}

	const char* UrlGB2312Encode(const char * strIn) override
	{
		std::string s = UrlCoding::UrlGB2312Encode(strIn);
		m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}

	const char* UrlGB2312Decode(const char * strIn) override
	{
		std::string s = UrlCoding::UrlGB2312Decode(strIn);
		m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}

	virtual const char* UrlUTF8Encode(const char * strIn) override
	{
		std::string s = UrlCoding::UrlUTF8Encode(strIn);
		m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}

	virtual const char* UrlUTF8Decode(const char * strIn) override
	{
		static std::string ms;
		std::string s = UrlCoding::UrlUTF8Decode(strIn);
		m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}


	virtual const char* WidebyteToAnsi(const wchar_t * strIn) override
	{
		std::string s;
		if (!GL::WideByte2Ansi(s, strIn))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}


	virtual const wchar_t* AnsiToWidebyte(const char * strIn) override
	{
		std::wstring s;
		if (!GL::Ansi2WideByte(s, strIn))
			m_convertBuffW.assign(L"");
		else
			m_convertBuffW.assign(s);
		return m_convertBuffW.c_str();
	}

	virtual const char* UTF8ToAnsi(const char * strIn) override
	{
		std::string s;
		if (!GL::Utf82Ansi(s, strIn))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}


	virtual const wchar_t* UTF8ToWidebyte(const char * strIn) override
	{
		std::wstring s;
		if (!GL::Utf82WideByte(s, strIn))
			m_convertBuffW.assign(L"");
		else
			m_convertBuffW.assign(s);
		return m_convertBuffW.c_str();
	}


	virtual const char* AnsiToUTF8(const char * strIn) override
	{
		std::string s;
		if (!GL::Ansi2Utf8(s, strIn))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}


	virtual const char* WidebyteToUTF8(const wchar_t * strIn) override
	{
		std::string s;
		if (!GL::WideByte2Utf8(s, strIn))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);
		return m_convertBuffA.c_str();
	}

private:
	int m_timeout;
	std::string m_userAgent;
	std::string m_customMothod;
	std::map<std::string, std::string> m_requestHeaders;

	bool m_autoRedirect;
	int m_maxRedirect;

	int m_responseCode;
	std::string m_responseBody;
	ResponseHeaderFields m_responseHeaders;

	std::set<char> ms_urlEncodeEscape;

	FN_PROGRESS_CALLBACK m_progressCallback;
	void* m_progressUserData;

	std::string m_convertBuffA;
	std::wstring m_convertBuffW;
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


LIBCURLHTTP_API void setTimeout(LibcurlHttp* http, int timeout)
{
	return http->setTimeout(timeout);
}

LIBCURLHTTP_API void setRequestHeader(LibcurlHttp* http, const char* key, const char* value)
{
	return http->setRequestHeader(key, value);
}

LIBCURLHTTP_API void setUserAgent(LibcurlHttp* http, const char* val)
{
	return http->setUserAgent(val);
}

LIBCURLHTTP_API void setCustomMothod(LibcurlHttp* http, const char* mothod)
{
	return http->setCustomMothod(mothod);
}

LIBCURLHTTP_API void setProgress(LibcurlHttp* http, FN_PROGRESS_CALLBACK progress, void* userData)
{
	http->setProgress(progress, userData);
}

LIBCURLHTTP_API void setAutoRedirect(LibcurlHttp* http, bool autoRedirect)
{
	http->setAutoRedirect(autoRedirect);
}

LIBCURLHTTP_API void setMaxRedirect(LibcurlHttp* http, int maxRedirect)
{
	http->setMaxRedirect(maxRedirect);
}

LIBCURLHTTP_API int get(LibcurlHttp* http, const char* url)
{
	return http->get(url);
}

LIBCURLHTTP_API int get_a(LibcurlHttp* http, const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	int ret = http->get_b(url, argv);

	va_end(argv);

	return ret;
}

LIBCURLHTTP_API int post(LibcurlHttp* http, const char* url, const char* content, int contentLen, const char* contentType /*= "application/x-www-form-urlencoded"*/)
{
	return http->post(url, content, contentLen, contentType);
}

LIBCURLHTTP_API int post_a(LibcurlHttp* http, const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	int ret = http->post_b(url, argv);

	va_end(argv);

	return ret;
}

LIBCURLHTTP_API int download(LibcurlHttp* http, const char* url, const char* localFileName/*=NULL*/, char* downloadedFileName/* = NULL*/)
{
	return http->download(url, localFileName, downloadedFileName);
}

LIBCURLHTTP_API int postForm(LibcurlHttp* http, const char* url, FormField* formDataArr, int nCountFormData)
{
	return http->postForm(url, formDataArr, nCountFormData);
}

LIBCURLHTTP_API int postForm_a(LibcurlHttp* http, const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	int ret = http->postForm_b(url, argv);

	va_end(argv);

	return ret;
}

LIBCURLHTTP_API int postMultipart(LibcurlHttp* http, const char* url, MultipartField* multipartDataArr, int nCountMultipartData)
{
	return http->postMultipart(url, multipartDataArr, nCountMultipartData);
}

LIBCURLHTTP_API const char* getBody(LibcurlHttp* http, int& len)
{
	return http->getBody(len);
}

LIBCURLHTTP_API int getCode(LibcurlHttp* http)
{
	return http->getCode();
}

LIBCURLHTTP_API int getResponseHeaderKeysCount(LibcurlHttp* http)
{
	return http->getResponseHeaderKeysCount();
}

LIBCURLHTTP_API const char* getResponseHeaderKey(LibcurlHttp* http, int i)
{
	return http->getResponseHeaderKey(i);
}

LIBCURLHTTP_API int getResponseHeadersCount(LibcurlHttp* http, const char* key)
{
	return http->getResponseHeadersCount(key);
}

LIBCURLHTTP_API const char* getResponseHeader(LibcurlHttp* http, const char* key, int i)
{
	return http->getResponseHeader(key, i);
}

LIBCURLHTTP_API const char* UrlGB2312Encode(LibcurlHttp* http, const char * strIn)
{
	return http->UrlGB2312Encode(strIn);
}

LIBCURLHTTP_API const char* UrlGB2312Decode(LibcurlHttp* http, const char * strIn)
{
	return http->UrlGB2312Decode(strIn);
}

LIBCURLHTTP_API const char* UrlUTF8Encode(LibcurlHttp* http, const char * strIn)
{
	return http->UrlUTF8Encode(strIn);
}

LIBCURLHTTP_API const char* UrlUTF8Decode(LibcurlHttp* http, const char * strIn)
{
	return http->UrlUTF8Decode(strIn);
}

LIBCURLHTTP_API const char* WidebyteToAnsi(LibcurlHttp* http, const wchar_t * strIn)
{
	return http->WidebyteToAnsi(strIn);
}

LIBCURLHTTP_API const wchar_t* AnsiToWidebyte(LibcurlHttp* http, const char * strIn)
{
	return http->AnsiToWidebyte(strIn);
}

LIBCURLHTTP_API const char* UTF8ToAnsi(LibcurlHttp* http, const char * strIn)
{
	return http->UTF8ToAnsi(strIn);
}

LIBCURLHTTP_API const wchar_t* UTF8ToWidebyte(LibcurlHttp* http, const char * strIn)
{
	return http->UTF8ToWidebyte(strIn);
}

LIBCURLHTTP_API const char* AnsiToUTF8(LibcurlHttp* http, const char * strIn)
{
	return http->AnsiToUTF8(strIn);
}

LIBCURLHTTP_API const char* WidebyteToUTF8(LibcurlHttp* http, const wchar_t * strIn)
{
	return http->WidebyteToUTF8(strIn);
}

