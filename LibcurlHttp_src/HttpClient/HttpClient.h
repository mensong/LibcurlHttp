#pragma once
#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <functional>
#include <curl/curl.h>

enum FormFieldType
{
	ftNormal,
	ftFile
};

typedef struct FormField
{
	FormFieldType fieldType;
	std::string fieldName;
	std::string fieldValue;
	std::string fileName;//只有在文件类型才有效
} FormField;

typedef std::map< std::string, std::vector<std::string> > ResponseHeaderFields;

class HttpClient
{
public:
	HttpClient();
	virtual ~HttpClient();

	//设置请求方法，一般为：OPTIONS GET POST PUT HEAD DELETE TRACE CONNECT，还可以为其它任意字符串
	virtual void SetCustomMothod(const std::string& mothod) { m_customMethod = mothod; }
	virtual const std::string& GetCustomMothod(const std::string& mothodDef = "GET") const;

	virtual void SetUrl(const std::string& url) { m_url = url; }
	virtual const std::string& GetUrl() const { return m_url; }

	virtual void SetUserAgent(const std::string& val) { m_userAgent = val; }
	virtual const std::string GetUserAgent() const { return m_userAgent; }

	virtual void SetAutoRedirect(bool autoRedirect) { m_autoRedirect = autoRedirect; }
	virtual bool GetAutoRedirect() { return m_autoRedirect; }

	virtual void SetMaxRedirect(int maxRedirect) { m_maxRedirect = maxRedirect; }
	virtual int GetMaxRedirect() { return m_maxRedirect; }

	virtual void SetHeaders(const std::map<std::string, std::string>& headers) { m_headers = headers; }
	virtual void SetHeader(const std::string& key, const std::string& val);
	virtual const std::string& GetHeader(const std::string& key) const;

	//单位秒
	virtual void SetTimtout(int t) { m_timeout = t; }
	virtual int GetTimeout() const { return m_timeout; }

	//post form
	virtual void ResetFormFields() { m_formFields.clear(); m_isInnerPost = false; }
	virtual void AddFormField(const FormField& field) { m_formFields.push_back(field); m_isInnerPost = true; }
	virtual const std::vector<FormField>& GetFormField() { return m_formFields; }

	//normal post
	virtual void ResetNormalPost() { m_postData.clear(); m_isInnerPost = false; }
	virtual void SetNormalPostData(const std::string& data) { m_postData = data; m_isInnerPost = true; }
	virtual const std::string& GetNormalPostData() { return m_postData; }

	virtual bool Do();

	virtual CURLcode GetCode() const { return m_retCode; }
	virtual int GetHttpCode() const { return m_httpCode; }
	virtual const std::string& GetBody() { return m_body; }
	virtual const ResponseHeaderFields& GetResponseHeaders() { return m_responseHeaders; };

protected:
	virtual bool OnWrited(void* pBuffer, size_t nSize, size_t nMemByte);
	virtual int OnProgress(double dltotal, double dlnow, double ultotal, double ulnow);
	virtual void OnDone(CURLcode code);

protected:
	static size_t _WriteDataCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
	static int _ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
	static size_t _HeaderCallback(void *data, size_t size, size_t nmemb, void *userdata);

public:
	// trim from start
	static inline std::string &ltrim(std::string &s) {  // NOLINT
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
			std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string &rtrim(std::string &s) { // NOLINT
		s.erase(std::find_if(s.rbegin(), s.rend(),
			std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static inline std::string &trim(std::string &s) {  // NOLINT
		return ltrim(rtrim(s));
	}

protected:
	int m_timeout;
	std::string m_customMethod;
	bool m_isInnerPost;
	std::string m_url;
	std::string m_userAgent;
	bool m_autoRedirect;
	int m_maxRedirect;

	std::map<std::string, std::string> m_headers;

	/**
		如果m_formFields有数据，则post form；
		如果m_formFields没有数据，m_postData不为空，则normal post；
		否则get
	*/
	std::vector<FormField> m_formFields;
	std::string m_postData;

	CURLcode m_retCode;
	int m_httpCode;
	std::string m_body;
	ResponseHeaderFields m_responseHeaders;
};

