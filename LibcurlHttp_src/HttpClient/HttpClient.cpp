#include "HttpClient.h"
#include <process.h>


HttpClient::HttpClient()
	: m_timeout(0)
	, m_retCode(CURL_LAST)
{
	m_userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36";

	m_finishEvent = CreateEventA(NULL, FALSE, TRUE, NULL);
	ResetEvent(m_finishEvent);
}


HttpClient::~HttpClient()
{
}

void HttpClient::SetHeader(const std::string& key, const std::string& val)
{
	if (val.empty())
	{
		m_headers.erase(key);
		return;
	}

	m_headers[key] = val;
}

const std::string& HttpClient::GetHeader(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator itFinder = m_headers.find(key);
	if (itFinder != m_headers.end())
		return itFinder->second;

	static std::string _empStr;
	return _empStr;
}

bool HttpClient::Do()
{
	if (m_url.empty())
		return false;

	m_body.clear();

	unsigned  uiThreadID = 0;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, HttpClient::_DealThread, (void*)this, 0, &uiThreadID);
	CloseHandle(hThread);

	return (hThread != NULL);
}

DWORD HttpClient::Wait()
{
	DWORD waitRet = WaitForSingleObject(m_finishEvent, INFINITE);
	ResetEvent(m_finishEvent);
	return waitRet;
}

bool HttpClient::OnWrited(void* pBuffer, size_t nSize, size_t nMemByte)
{
	m_body.append(reinterpret_cast<char*>(pBuffer), nSize*nMemByte);
	return true;
}

int HttpClient::OnProgress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	return 0;
}

void HttpClient::OnDone(CURLcode code)
{

}

#ifdef _WIN32
unsigned __stdcall
#else
void*
#endif 
HttpClient::_DealThread(void* arg)
{
	HttpClient* _THIS = (HttpClient*)arg;
	if (!_THIS)
		return 0;

#if 1
	std::string url = _THIS->GetUrl().c_str();

	//初始化curl，这个是必须的
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	//设置方法
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	//设置接收数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _WriteDataCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, _THIS);
	//支持https
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	// 设置重定向的最大次数
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5);
	// 设置301、302跳转跟随location
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
	//设置进度回调函数
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, _ProgressCallback);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, _THIS);
	/** set user agent */
	curl_easy_setopt(curl, CURLOPT_USERAGENT, _THIS->GetUserAgent().c_str());
	/** set headers */
	curl_slist* headerList = NULL;
	std::string headerString;
	for (std::map<std::string, std::string>::iterator it = _THIS->m_headers.begin();
		it != _THIS->m_headers.end(); ++it)
	{
		if (it->first.empty() || it->second.empty())
			continue;
		headerString = it->first;
		headerString += ": ";
		headerString += it->second;
		headerList = curl_slist_append(headerList, headerString.c_str());
	}
	if (headerList)
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
	/** time out */
	if (_THIS->GetTimeout() > 0)
	{
		//连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, _THIS->GetTimeout());
		//接收数据时超时设置，如果10秒内数据未接收完，直接退出
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, _THIS->GetTimeout());
	}

	struct curl_httppost* post = NULL;
	if (_THIS->m_formFields.size() > 0)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		/** Now specify we want to POST data */
		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		//post form
		struct curl_httppost* last = NULL;
		for (int i = 0; i < _THIS->m_formFields.size(); ++i)
		{
			CURLFORMcode formCode = CURL_FORMADD_NULL;
			FormField& ff = _THIS->m_formFields[i];
			if (ff.fieldType == ftNormal)
			{
				formCode = curl_formadd(&post, &last,
					CURLFORM_COPYNAME, ff.fieldName.c_str(),
					CURLFORM_COPYCONTENTS, ff.fieldValue.c_str(),
					CURLFORM_END);
			}
			else if (ff.fieldType == ftFile)
			{
				if (ff.fileName.empty() || ff.fileName == "")
				{
					formCode = curl_formadd(&post, &last,
						CURLFORM_COPYNAME, ff.fieldName.c_str(),
						CURLFORM_FILE, ff.fieldValue.c_str(),
						CURLFORM_END);
				}
				else
				{
					formCode = curl_formadd(&post, &last,
						CURLFORM_COPYNAME, ff.fieldName.c_str(),
						CURLFORM_FILE, ff.fieldValue.c_str(),
						CURLFORM_FILENAME, ff.fileName.c_str(),
						CURLFORM_END);
				}
			}
		}
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	}
	else if (_THIS->m_postData.size() > 0)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		/** Now specify we want to POST data */
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		/** set post fields */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _THIS->m_postData.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, _THIS->m_postData.size());
	}

	//开始执行请求
	_THIS->m_retCode = curl_easy_perform(curl);
	//查看是否有出错信息
	//const char* pError = curl_easy_strerror(m_retCode);

	if (_THIS->m_retCode != CURLE_OK)
	{
		switch (_THIS->m_retCode)
		{
		case CURLE_OPERATION_TIMEDOUT:
		case CURLE_SSL_CERTPROBLEM:
			_THIS->m_httpCode = _THIS->m_retCode;
			break;
		}
	}
	else
	{
		int64_t http_code = 0;
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
		_THIS->m_httpCode = static_cast<int>(http_code);
	}

	if (post)
		curl_formfree(post);

	//清理列表
	if (headerList)
		curl_slist_free_all(headerList);
	//清理curl，和前面的初始化匹配
	curl_easy_cleanup(curl);
#else
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, "http://10.8.202.210:32031/group1/upload");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_DEFAULT_PROTOCOL, "https");
		struct curl_slist *headers = NULL;
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_mime *mime;
		curl_mimepart *part;
		mime = curl_mime_init(curl);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "file");
		curl_mime_filedata(part, "/C:/Users/Administrator/Desktop/微信图片_20200515193208.png");
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "scene");
		curl_mime_data(part, "default", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "filename");
		curl_mime_data(part, "", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "output");
		curl_mime_data(part, "json2", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "path");
		curl_mime_data(part, "", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "code");
		curl_mime_data(part, "", CURL_ZERO_TERMINATED);
		part = curl_mime_addpart(mime);
		curl_mime_name(part, "auth_token");
		curl_mime_data(part, "", CURL_ZERO_TERMINATED);
		curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
		res = curl_easy_perform(curl);
		curl_mime_free(mime);
	}
	curl_easy_cleanup(curl);

#endif

	_THIS->OnDone(_THIS->m_retCode);

	SetEvent(_THIS->m_finishEvent);

	return 0;
}

size_t HttpClient::_WriteDataCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	HttpClient* _THIS = (HttpClient*)pParam;
	if (!_THIS)
		return 0;
	if (_THIS->OnWrited(pBuffer, nSize, nMemByte))
		return nSize * nMemByte;
	return 0;
}

int HttpClient::_ProgressCallback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	HttpClient* _THIS = (HttpClient*)clientp;
	if (!_THIS)
		return 0;
	return _THIS->OnProgress(dltotal, dlnow, ultotal, ulnow);
}
