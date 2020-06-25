#pragma once
#include <map>
#include <string>
#include <vector>
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

class HttpClient
{
public:
	HttpClient();
	virtual ~HttpClient();

	void SetUrl(const std::string& url) { m_url = url; }
	const std::string& GetUrl() const { return m_url; }

	void SetUserAgent(const std::string& val) { m_userAgent = val; }
	const std::string GetUserAgent() const { return m_userAgent; }

	void SetHeaders(const std::map<std::string, std::string> headers) { m_headers = headers; }
	void SetHeader(const std::string& key, const std::string& val);
	const std::string& GetHeader(const std::string& key) const;

	//单位秒
	void SetTimtout(int t) { m_timeout = t; }
	int GetTimeout() const { return m_timeout; }
		
	//post form
	void ResetFormFields() { m_formFields.clear(); }
	void AddFormField(const FormField& field) { m_formFields.push_back(field); }

	//normal post
	void ResetNormalPost() { m_postData.clear(); }
	void SetNormalPostData(const std::string& data) { m_postData = data; }

	//get
	//...

	virtual bool Do();

	DWORD Wait();


	CURLcode GetCode() const { return m_retCode; }
	int GetHttpCode() const { return m_httpCode; }
	const std::string& GetBody() { return m_body; }

protected:
	virtual bool OnWrited(void* pBuffer, size_t nSize, size_t nMemByte);
	virtual int OnProgress(double dltotal, double dlnow, double ultotal, double ulnow);
	virtual void OnDone(CURLcode code);

protected:
	static
#ifdef _WIN32
		unsigned __stdcall
#else
		void*
#endif
		_DealThread(void* arg);

	static size_t _WriteDataCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
	static int _ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);

protected:
	std::string m_url;
	std::string m_userAgent;
	std::map<std::string, std::string> m_headers;
	int m_timeout;

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

	HANDLE m_finishEvent;
};

