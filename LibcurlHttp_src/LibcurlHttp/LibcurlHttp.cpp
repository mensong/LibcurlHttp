// LibcurHttp.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "LibcurlHttp.h"
#include "HttpClientFC.h"
#include "HttpFileDownloadFC.h"
#include "..\HttpHelper\UrlCoding.h"
#include "..\HttpHelper\Convertor.h"
#include "pystring.h"

class LibcurlHttpImp
	: public LibcurlHttp
{
public:
	LibcurlHttpImp()
		: m_httpClient(NULL)
		, m_timeout(0)
		, m_headerCallback(NULL)
		, m_headerUserData(NULL)
		, m_headerWriteBuff(true)
		, m_bodyCallback(NULL)
		, m_bodyUserData(NULL)
		, m_bodyWriteBuff(false)
		, m_progressCallback(NULL)
		, m_progressUserData(NULL)
		, m_autoRedirect(true)
		, m_maxRedirect(5)
		, m_responseBodyDecode(true)
	{
		m_urlEncodeEscape.insert('/');
		m_urlEncodeEscape.insert(':');
		m_urlEncodeEscape.insert('%');
		m_urlEncodeEscape.insert('&');
		m_urlEncodeEscape.insert('?');
		m_urlEncodeEscape.insert('\\');
		m_urlEncodeEscape.insert('=');

		m_userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/9999.9999.9999.9999 Safari/9999.9999";
	}
	~LibcurlHttpImp()
	{
		if (m_httpClient)
		{
			delete m_httpClient;
			m_httpClient = NULL;
		}
	}

	virtual void setTimeout(int t) override
	{
		m_timeout = t;
	}

	virtual void setResponseHeaderCallback(FN_HEADER_CALLBACK cb, void* userData, bool writeBuff = true) override
	{
		m_headerCallback = cb;
		m_headerUserData = userData;
		m_headerWriteBuff = writeBuff;
	}
	
	virtual void setResponseBodyCallback(FN_WRITED_CALLBACK cb, void* userData, bool writeBuff = false) override
	{
		m_bodyCallback = cb;
		m_bodyUserData = userData;
		m_bodyWriteBuff = writeBuff;
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

	virtual void setResponseBodyAutoDecode(bool decode) override
	{
		m_responseBodyDecode = decode;
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

	virtual void setCustomMethod(const char* method) override
	{
		m_customMethod = method;
	}

	virtual bool get(const char* url) override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &m_urlEncodeEscape);
		//return m_http.get(sUrl.c_str(), dealRedirect);

		HttpClientFC* httpClient = new HttpClientFC;
		httpClient->SetUrl(sUrl);
		httpClient->SetTimtout(m_timeout);
		httpClient->SetUserAgent(m_userAgent);
		httpClient->SetHeaders(m_requestHeaders);
		httpClient->SetCustomMethod(m_customMethod);
		httpClient->SetHeaderCallback(m_headerCallback, m_headerUserData, m_headerWriteBuff);
		httpClient->SetBodyCallback(m_bodyCallback, m_bodyUserData, m_bodyWriteBuff);
		httpClient->SetProgress(m_progressCallback, m_progressUserData);
		httpClient->SetAutoRedirect(m_autoRedirect);
		httpClient->SetMaxRedirect(m_maxRedirect);
		httpClient->SetResponseBodyAutoDecode(m_responseBodyDecode);
				
		bool b = httpClient->Do();

		setWorkingHttpClient(httpClient);
		
		m_customMethod = "";
		return b;
	}	

	virtual bool get_a(const char* url, ...)
	{
		va_list argv;
		va_start(argv, url);

		bool b = get_b(url, argv);

		va_end(argv);

		return b;
	}

	virtual bool get_b(const char* url, va_list argv)
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

	virtual bool post(const char* url, const char* content, size_t contentLen,
		const char* contentType = "application/x-www-form-urlencoded") override
	{
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &m_urlEncodeEscape);

		HttpClientFC* httpClient = new HttpClientFC;
		httpClient->SetUrl(sUrl);
		httpClient->SetTimtout(m_timeout);
		httpClient->SetUserAgent(m_userAgent);
		httpClient->SetHeaders(m_requestHeaders);
		httpClient->SetHeader("Content-Type", contentType);
		httpClient->SetCustomMethod(m_customMethod);
		httpClient->SetHeaderCallback(m_headerCallback, m_headerUserData, m_headerWriteBuff);
		httpClient->SetBodyCallback(m_bodyCallback, m_bodyUserData, m_bodyWriteBuff);
		httpClient->SetProgress(m_progressCallback, m_progressUserData);
		httpClient->SetAutoRedirect(m_autoRedirect);
		httpClient->SetMaxRedirect(m_maxRedirect);
		httpClient->SetResponseBodyAutoDecode(m_responseBodyDecode);

		std::string sData(content, contentLen);
		httpClient->SetNormalPostData(sData);
		if (contentLen == 0)
		{//防止post空内容时出现411错误
			httpClient->SetHeader("Content-Length", "0");
		}

		bool b = httpClient->Do();

		setWorkingHttpClient(httpClient);

		m_customMethod = "";
		return b;
	}
	
	virtual bool post_a(const char* url, ...) override
	{
		va_list argv;
		va_start(argv, url);

		bool b = post_b(url, argv);

		va_end(argv);

		return b;
	}

	virtual bool post_b(const char* url, va_list argv)
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

		return post(url, sContet.c_str(), sContet.size());
	}

	virtual bool postMultipart_a(const char* url, ...) override
	{
		va_list argv;
		va_start(argv, url);

		bool b = postMultipart_b(url, argv);

		va_end(argv);

		return b;
	}
	
	virtual bool postMultipart_b(const char* url, va_list argv) override
	{
		int fieldType = 0;
		char* key = NULL;
		char* val = NULL;
		char* mimeType = NULL;
		char* fileName = NULL;
		
		std::vector<MultipartField*> multipartDataArr;

		do
		{
			fieldType = va_arg(argv, int);
			if (!fieldType)
				break;

			key = va_arg(argv, char*);
			if (!key)
				break;

			val = va_arg(argv, char*);
			if (!val)
				val = "";

			mimeType = va_arg(argv, char*);

			if (fieldType == 2)
				fileName = va_arg(argv, char*);

			MultipartField* multipart = new MultipartField();
			if (fieldType == 2)
				multipart->Fill(NULL, 0, val, fileName, key, mimeType);
			else
				multipart->Fill(val, strlen(val), NULL, NULL, key, mimeType);
			
			multipartDataArr.push_back(multipart);			
		} while (1);
		
		bool b = postMultipart(url, multipartDataArr.data(), multipartDataArr.size());

		for (size_t i = 0; i < multipartDataArr.size(); i++)
		{
			delete multipartDataArr[i];
		}
		multipartDataArr.clear();

		return b;
	}
	
	virtual bool postMultipart(const char* url, MultipartField* pMmultipartDataArr[], int nCountMultipartData) override
	{
		HttpClientFC* httpClient = new HttpClientFC;

		std::vector<MultipartField*> params;
		for (int i = 0; i < nCountMultipartData; i++)
			params.push_back(pMmultipartDataArr[i]);
		httpClient->SetMultipartFields(params);

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &m_urlEncodeEscape);
		httpClient->SetUrl(sUrl.c_str());
		httpClient->SetTimtout(m_timeout);
		httpClient->SetUserAgent(m_userAgent);
		httpClient->SetHeaders(m_requestHeaders);
		httpClient->SetCustomMethod(m_customMethod);
		httpClient->SetHeaderCallback(m_headerCallback, m_headerUserData, m_headerWriteBuff);
		httpClient->SetBodyCallback(m_bodyCallback, m_bodyUserData, m_bodyWriteBuff);
		httpClient->SetProgress(m_progressCallback, m_progressUserData);
		httpClient->SetAutoRedirect(m_autoRedirect);
		httpClient->SetMaxRedirect(m_maxRedirect);
		httpClient->SetResponseBodyAutoDecode(m_responseBodyDecode);
		if (nCountMultipartData == 0)
		{//防止post空内容时出现411错误
			httpClient->SetHeader("Content-Length", "0");
		}

		bool b = httpClient->Do();

		setWorkingHttpClient(httpClient);

		m_customMethod = "";
		return b;
	}

	virtual bool postMultipart(const char* url, MultipartField* multipartDataArr, int nCountMultipartData) override
	{
		std::vector<MultipartField*> pMmultipartDataArr;
		for (int i = 0; i < nCountMultipartData; i++)
			pMmultipartDataArr.push_back(&multipartDataArr[i]);
		return postMultipart(url, pMmultipartDataArr.data(), pMmultipartDataArr.size());
	}

	virtual bool download(const char* url, const char* localFileName = NULL, char* downloadedFileName = NULL) override
	{
		HttpFileDownloadFC* downloader = new HttpFileDownloadFC;

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &m_urlEncodeEscape);
		downloader->SetUrl(sUrl.c_str());
		if (localFileName)
			downloader->SetFile(localFileName);
		downloader->SetTimtout(m_timeout);
		downloader->SetUserAgent(m_userAgent);
		downloader->SetHeaders(m_requestHeaders);
		downloader->SetCustomMethod(m_customMethod);
		downloader->SetHeaderCallback(m_headerCallback, m_headerUserData, m_headerWriteBuff);
		downloader->SetProgress(m_progressCallback, m_progressUserData);
		downloader->SetAutoRedirect(m_autoRedirect);
		downloader->SetMaxRedirect(m_maxRedirect);
		
		bool b = downloader->Do();

		setWorkingHttpClient(downloader);

		m_customMethod = "";

		if (downloadedFileName)
			strcpy(downloadedFileName, downloader->GetFile().c_str());

		return b;
	}
	
	virtual bool putData(const char* url, const char* data, size_t dataLen) override
	{
		return putData(url, (const unsigned char*)data, dataLen);
	}

	virtual bool putData(const char* url, const unsigned char* data, size_t dataLen) override
	{
		HttpClientFC* httpClient = new HttpClientFC;
			
		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &m_urlEncodeEscape);
		httpClient->SetUrl(sUrl.c_str());
		httpClient->SetTimtout(m_timeout);
		httpClient->SetUserAgent(m_userAgent);
		httpClient->SetHeaders(m_requestHeaders);
		httpClient->SetCustomMethod(m_customMethod);
		httpClient->SetHeaderCallback(m_headerCallback, m_headerUserData, m_headerWriteBuff);
		httpClient->SetBodyCallback(m_bodyCallback, m_bodyUserData, m_bodyWriteBuff);
		httpClient->SetProgress(m_progressCallback, m_progressUserData);
		httpClient->SetAutoRedirect(m_autoRedirect);
		httpClient->SetMaxRedirect(m_maxRedirect);
		httpClient->SetResponseBodyAutoDecode(m_responseBodyDecode);

		if (dataLen == 0)
		{//防止post空内容时出现411错误
			httpClient->SetHeader("Content-Length", "0");
		}
		else
		{
			httpClient->SetPutData(data, dataLen);
		}

		bool b = httpClient->Do();

		setWorkingHttpClient(httpClient);

		m_customMethod = "";
		return b;
	}

	virtual bool putFile(const char* url, const char* filePath) override
	{
		if (!filePath)
		{
			return false;
		}

		HttpClientFC* httpClient = new HttpClientFC;

		std::string sUrl = UrlCoding::UrlUTF8Encode(url, &m_urlEncodeEscape);
		httpClient->SetUrl(sUrl.c_str());
		httpClient->SetTimtout(m_timeout);
		httpClient->SetUserAgent(m_userAgent);
		httpClient->SetHeaders(m_requestHeaders);
		httpClient->SetCustomMethod(m_customMethod);
		httpClient->SetHeaderCallback(m_headerCallback, m_headerUserData, m_headerWriteBuff);
		httpClient->SetBodyCallback(m_bodyCallback, m_bodyUserData, m_bodyWriteBuff);
		httpClient->SetProgress(m_progressCallback, m_progressUserData);
		httpClient->SetAutoRedirect(m_autoRedirect);
		httpClient->SetMaxRedirect(m_maxRedirect);
		httpClient->SetPutFile(filePath);
		httpClient->SetResponseBodyAutoDecode(m_responseBodyDecode);

		bool b = httpClient->Do();

		setWorkingHttpClient(httpClient);

		m_customMethod = "";
		return b;
	}

	virtual const char* getBody(size_t& len) override
	{
		if (!hasWorkingHttpClient())
		{
			len = 0;
			return NULL;
		}
		const std::string& sBody = m_httpClient->GetBody();
		len = sBody.size();
		return sBody.c_str();
	}

	virtual int getErrorCode() override
	{
		if (!hasWorkingHttpClient())
		{
			return -1;
		}
		return m_httpClient->GetErrorCode();
	}

	virtual int getHttpCode() override
	{
		if (!hasWorkingHttpClient())
		{
			return -1;
		}
		return m_httpClient->GetHttpCode();
	}

	virtual int getResponseHeaderKeysCount() override
	{
		if (!hasWorkingHttpClient())
		{
			return 0;
		}
		return (int)m_httpClient->GetResponseHeaders().size();
	}

	virtual const char* getResponseHeaderKey(int i) override
	{
		if (!hasWorkingHttpClient())
		{
			return "";
		}

		if (i < 0)
			return "";

		const ResponseHeaderFields& responseHeaders = m_httpClient->GetResponseHeaders();
		if (i >= (int)responseHeaders.size())
			return "";

		ResponseHeaderFields::const_iterator it = responseHeaders.cbegin();
		while (i--)
		{
			++it;
		}

		return it->first.c_str();
	}

	virtual int getResponseHeadersCount(const char* key, bool ignoreCase = false) override
	{
		if (!hasWorkingHttpClient())
		{
			return 0;
		}

		if (!key)
		{
			return 0;
		}

		const ResponseHeaderFields& responseHeaders = m_httpClient->GetResponseHeaders();

		ResponseHeaderFields::const_iterator itFinder;
		if (!ignoreCase)
		{
			itFinder = responseHeaders.find(key);
		}
		else
		{
			for (itFinder = responseHeaders.cbegin(); itFinder != responseHeaders.cend(); ++itFinder)
			{
				if (pystring::equal(itFinder->first, key, true))
				{
					break;
				}
			}
		}
		
		if (itFinder == responseHeaders.cend())
		{
			return 0;
		}
		return (int)itFinder->second.size();
	}

	virtual const char* getResponseHeader(const char* key, int i, bool ignoreCase = false) override
	{
		if (!hasWorkingHttpClient())
		{
			return "";
		}

		if (!key)
		{
			return "";
		}

		const ResponseHeaderFields& responseHeaders = m_httpClient->GetResponseHeaders();

		ResponseHeaderFields::const_iterator itFinder;
		if (!ignoreCase)
		{
			itFinder = responseHeaders.find(key);
		}
		else
		{
			for (itFinder = responseHeaders.cbegin(); itFinder != responseHeaders.cend(); ++itFinder)
			{
				if (pystring::equal(itFinder->first, key, true))
				{
					break;
				}
			}
		}

		if (itFinder == responseHeaders.cend())
		{
			return "";
		}
		if (i >= (int)itFinder->second.size())
		{
			return "";
		}

		return itFinder->second[i].c_str();
	}

	const char* UrlGB2312Encode(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		std::string s = UrlCoding::UrlGB2312Encode(std::string(strIn, inOutLen));
		m_convertBuffA.assign(s);
		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}

	const char* UrlGB2312Decode(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		std::string s = UrlCoding::UrlGB2312Decode(std::string(strIn, inOutLen));
		m_convertBuffA.assign(s);
		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}

	virtual const char* UrlUTF8Encode(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		std::string s = UrlCoding::UrlUTF8Encode(std::string(strIn, inOutLen));
		m_convertBuffA.assign(s);
		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}

	virtual const char* UrlUTF8Decode(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		static std::string ms;
		std::string s = UrlCoding::UrlUTF8Decode(std::string(strIn, inOutLen));
		m_convertBuffA.assign(s);
		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}


	virtual const char* UnicodeToAnsi(const wchar_t * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = wcslen(strIn);

		std::string s;
		if (!GL::Unicode2Ansi(s, std::wstring(strIn, inOutLen)))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);

		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}


	virtual const wchar_t* AnsiToUnicode(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		std::wstring s;
		if (!GL::Ansi2Unicode(s, std::string(strIn, inOutLen)))
			m_convertBuffW.assign(L"");
		else
			m_convertBuffW.assign(s);

		inOutLen = m_convertBuffW.size();
		return m_convertBuffW.c_str();
	}

	virtual const char* UTF8ToAnsi(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		std::string s;
		if (!GL::Utf82Ansi(s, std::string(strIn, inOutLen)))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);

		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}


	virtual const wchar_t* UTF8ToUnicode(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		std::wstring s;
		if (!GL::Utf82Unicode(s, std::string(strIn, inOutLen)))
			m_convertBuffW.assign(L"");
		else
			m_convertBuffW.assign(s);

		inOutLen = m_convertBuffW.size();
		return m_convertBuffW.c_str();
	}


	virtual const char* AnsiToUTF8(const char * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = strlen(strIn);

		std::string s;
		if (!GL::Ansi2Utf8(s, std::string(strIn, inOutLen)))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);

		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}


	virtual const char* UnicodeToUTF8(const wchar_t * strIn, size_t& inOutLen) override
	{
		if (inOutLen == 0)
			inOutLen = wcslen(strIn);

		std::string s;
		if (!GL::Unicode2Utf8(s, std::wstring(strIn, inOutLen)))
			m_convertBuffA.assign("");
		else
			m_convertBuffA.assign(s);

		inOutLen = m_convertBuffA.size();
		return m_convertBuffA.c_str();
	}

protected:
	void setWorkingHttpClient(HttpClient* httpClient)
	{
		if (m_httpClient)
		{
			delete m_httpClient;
			m_httpClient = NULL;
		}

		m_httpClient = httpClient;
	}
	bool hasWorkingHttpClient()
	{
		return m_httpClient != NULL;
	}

private:
	int m_timeout;
	std::string m_userAgent;
	std::string m_customMethod;
	std::map<std::string, std::string> m_requestHeaders;

	bool m_autoRedirect;
	int m_maxRedirect;

	bool m_responseBodyDecode;

	//int m_responseCode;
	//std::string m_responseBody;
	//ResponseHeaderFields m_responseHeaders;
	HttpClient* m_httpClient;

	std::set<char> m_urlEncodeEscape;

	FN_HEADER_CALLBACK m_headerCallback;
	void* m_headerUserData;
	bool m_headerWriteBuff;

	FN_WRITED_CALLBACK m_bodyCallback;
	void* m_bodyUserData;
	bool m_bodyWriteBuff;

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

LIBCURLHTTP_API void setCustomMethod(LibcurlHttp* http, const char* method)
{
	return http->setCustomMethod(method);
}

LIBCURLHTTP_API void setResponseHeaderCallback(LibcurlHttp* http, FN_HEADER_CALLBACK cb, void* userData, bool writeBuff)
{
	http->setResponseHeaderCallback(cb, userData, writeBuff);
}

LIBCURLHTTP_API void setResponseBodyCallback(LibcurlHttp* http, FN_WRITED_CALLBACK cb, void* userData, bool writeBuff)
{
	http->setResponseBodyCallback(cb, userData, writeBuff);
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

LIBCURLHTTP_API void setResponseBodyAutoDecode(LibcurlHttp* http, bool decode)
{
	http->setResponseBodyAutoDecode(decode);
}

LIBCURLHTTP_API bool get(LibcurlHttp* http, const char* url)
{
	return http->get(url);
}

LIBCURLHTTP_API bool get_a(LibcurlHttp* http, const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	bool b = http->get_b(url, argv);

	va_end(argv);

	return b;
}

LIBCURLHTTP_API bool post(LibcurlHttp* http, const char* url, const char* content, size_t contentLen,
	const char* contentType /*= "application/x-www-form-urlencoded"*/)
{
	return http->post(url, content, contentLen, contentType);
}

LIBCURLHTTP_API bool post_a(LibcurlHttp* http, const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	bool b = http->post_b(url, argv);

	va_end(argv);

	return b;
}

LIBCURLHTTP_API bool download(LibcurlHttp* http, const char* url, const char* localFileName/*=NULL*/, char* downloadedFileName/* = NULL*/)
{
	return http->download(url, localFileName, downloadedFileName);
}

LIBCURLHTTP_API bool postMultipart(LibcurlHttp* http, const char* url, MultipartField* pMmultipartDataArr[], int nCountMultipartData)
{
	return http->postMultipart(url, pMmultipartDataArr, nCountMultipartData);
}

LIBCURLHTTP_API bool postMultipartA(LibcurlHttp* http, const char* url, MultipartField* multipartDataArr, int nCountMultipartData)
{
	return http->postMultipart(url, multipartDataArr, nCountMultipartData);
}

LIBCURLHTTP_API bool postMultipart_a(LibcurlHttp* http, const char* url, ...)
{
	va_list argv;
	va_start(argv, url);

	bool b = http->postMultipart_b(url, argv);

	va_end(argv);

	return b;
}

LIBCURLHTTP_API bool postMultipart_b(LibcurlHttp* http, const char* url, va_list argv)
{
	return http->postMultipart_b(url, argv);
}

LIBCURLHTTP_API bool putData(LibcurlHttp* http, const char* url, const unsigned char* data, size_t dataLen)
{
	return http->putData(url, data, dataLen);
}

LIBCURLHTTP_API bool putFile(LibcurlHttp* http, const char* url, const char* filePath)
{
	return http->putFile(url, filePath);
}

LIBCURLHTTP_API const char* getBody(LibcurlHttp* http, size_t& len)
{
	return http->getBody(len);
}

LIBCURLHTTP_API int getErrorCode(LibcurlHttp* http)
{
	return http->getErrorCode();
}

LIBCURLHTTP_API int getHttpCode(LibcurlHttp* http)
{
	return http->getHttpCode();
}

LIBCURLHTTP_API int getResponseHeaderKeysCount(LibcurlHttp* http)
{
	return http->getResponseHeaderKeysCount();
}

LIBCURLHTTP_API const char* getResponseHeaderKey(LibcurlHttp* http, int i)
{
	return http->getResponseHeaderKey(i);
}

LIBCURLHTTP_API int getResponseHeadersCount(LibcurlHttp* http, const char* key, bool ignoreCase)
{
	return http->getResponseHeadersCount(key, ignoreCase);
}

LIBCURLHTTP_API const char* getResponseHeader(LibcurlHttp* http, const char* key, int i, bool ignoreCase)
{
	return http->getResponseHeader(key, i, ignoreCase);
}

LIBCURLHTTP_API const char* UrlGB2312Encode(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->UrlGB2312Encode(strIn, inOutLen);
}

LIBCURLHTTP_API const char* UrlGB2312Decode(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->UrlGB2312Decode(strIn, inOutLen);
}

LIBCURLHTTP_API const char* UrlUTF8Encode(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->UrlUTF8Encode(strIn, inOutLen);
}

LIBCURLHTTP_API const char* UrlUTF8Decode(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->UrlUTF8Decode(strIn, inOutLen);
}

LIBCURLHTTP_API const char* UnicodeToAnsi(LibcurlHttp* http, const wchar_t * strIn, size_t& inOutLen)
{
	return http->UnicodeToAnsi(strIn, inOutLen);
}

LIBCURLHTTP_API const wchar_t* AnsiToUnicode(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->AnsiToUnicode(strIn, inOutLen);
}

LIBCURLHTTP_API const char* UTF8ToAnsi(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->UTF8ToAnsi(strIn, inOutLen);
}

LIBCURLHTTP_API const wchar_t* UTF8ToUnicode(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->UTF8ToUnicode(strIn, inOutLen);
}

LIBCURLHTTP_API const char* AnsiToUTF8(LibcurlHttp* http, const char * strIn, size_t& inOutLen)
{
	return http->AnsiToUTF8(strIn, inOutLen);
}

LIBCURLHTTP_API const char* UnicodeToUTF8(LibcurlHttp* http, const wchar_t * strIn, size_t& inOutLen)
{
	return http->UnicodeToUTF8(strIn, inOutLen);
}

