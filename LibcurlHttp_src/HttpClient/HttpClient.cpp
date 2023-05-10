#include "HttpClient.h"
#include <process.h>


HttpClient::HttpClient()
	: m_timeout(0)
	, m_retCode(CURL_LAST)
	, m_httpCode(0)
	, m_autoRedirect(true)
	, m_maxRedirect(5)
	, m_isInnerPost(false)
{
	m_userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/9999.9999.9999.9999 Safari/9999.9999";
}

HttpClient::~HttpClient()
{
}

const std::string& HttpClient::GetCustomMothod(const std::string& mothodDef /*= "GET"*/) const
{
	if (m_customMethod.empty() || m_customMethod == "")
	{
		return mothodDef;
	}
	return m_customMethod;
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
	m_body.clear();

	HttpClient* _THIS = this;

	std::string url = _THIS->GetUrl().c_str();
	if (url == "" || url.empty())
		return false;

	//初始化curl，这个是必须的
	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	//设置接收数据的回调
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _WriteDataCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, _THIS);
	//设置进度回调函数
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);//设为false 下面才能设置进度响应函数
	curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, _ProgressCallback);
	curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, _THIS);
	//设置headers回调函数
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, _THIS);
	//支持https
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	
	// 设置301、302跳转跟随location
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, m_autoRedirect? 1L : 0L);
	// 设置重定向的最大次数
	curl_easy_setopt(curl, CURLOPT_MAXREDIRS, m_maxRedirect);

	/** set user agent */
	if (_THIS->GetUserAgent().size() > 0)
		curl_easy_setopt(curl, CURLOPT_USERAGENT, _THIS->GetUserAgent().c_str());

	/** set headers */
	curl_slist* headerList = NULL;
	std::string headerString;
	for (std::map<std::string, std::string>::iterator it = _THIS->m_headers.begin();
		it != _THIS->m_headers.end(); ++it)
	{
		if (it->first.empty())
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
		//接收数据时超时设置，如果GetTimeout()秒内数据未接收完，直接退出
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, _THIS->GetTimeout());
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
	}
	
	/** set body */
	std::string sMethod;
	struct curl_httppost* post = NULL;
	curl_mime *mime = NULL;
	if (_THIS->m_formFields.size() > 0)
	{
		sMethod = _THIS->GetCustomMothod("POST");

		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		//post form
		struct curl_httppost* last = NULL;
		for (int i = 0; i < (int)_THIS->m_formFields.size(); ++i)
		{
			CURLFORMcode formCode = CURL_FORMADD_NULL;
			FormField& ff = _THIS->m_formFields[i];
			if (ff.fieldType == ftNormal)
			{
				formCode = curl_formadd(&post, &last,
					CURLFORM_COPYNAME, ff.fieldName,
					CURLFORM_COPYCONTENTS, (ff.fieldValue ? ff.fieldValue : ""),
					CURLFORM_END);
			}
			else if (ff.fieldType == ftFile)
			{
				if (!ff.fileName)
				{
					formCode = curl_formadd(&post, &last,
						CURLFORM_COPYNAME, ff.fieldName,
						CURLFORM_FILE, (ff.fieldValue ? ff.fieldValue : ""),
						CURLFORM_END);
				}
				else
				{
					formCode = curl_formadd(&post, &last,
						CURLFORM_COPYNAME, ff.fieldName,
						CURLFORM_FILE, (ff.fieldValue ? ff.fieldValue : ""),
						CURLFORM_FILENAME, ff.fileName,
						CURLFORM_END);
				}
			}
		}
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	}
	else if (_THIS->m_postData.size() > 0)
	{
		sMethod = _THIS->GetCustomMothod("POST");

		curl_easy_setopt(curl, CURLOPT_POST, 1L);

		/** set post fields */
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _THIS->m_postData.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, _THIS->m_postData.size());
	}
	else if (_THIS->m_multipartFields.size() > 0)
	{
		sMethod = _THIS->GetCustomMothod("POST");
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		
		mime = curl_mime_init(curl);
		if (mime)
		{
			for (int i = 0; i < _THIS->m_multipartFields.size(); ++i)
			{
				const MultipartField& mpf = _THIS->m_multipartFields[i];
				curl_mimepart *part = curl_mime_addpart(mime);

				//set data
				if (mpf.contenxtData)
				{
					curl_mime_data(part, mpf.contenxtData, (mpf.contenxtDataSize > 0 ? mpf.contenxtDataSize : strlen(mpf.contenxtData)));
				}
				else if (mpf.filePath)
				{
					curl_mime_filedata(part, mpf.filePath);
				}

				//set name 
				if (mpf.multipartName)
					curl_mime_name(part, mpf.multipartName);

				//set filename
				if (mpf.fileName)
					curl_mime_filename(part, mpf.fileName);

				//set mime type
				if (mpf.mimeType)
					curl_mime_type(part, mpf.mimeType);
			}

			curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
		}
		
	}
	else
	{
		if (m_isInnerPost)
			sMethod = _THIS->GetCustomMothod("POST");
		else
			sMethod = _THIS->GetCustomMothod("GET");
	}

	/** set custom method */
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sMethod.c_str());

	/** 开始执行请求 */
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
	
	if (mime)
		curl_mime_free(mime);

	//清理列表
	if (headerList)
		curl_slist_free_all(headerList);
	//清理curl，和前面的初始化匹配
	curl_easy_cleanup(curl);

	_THIS->OnDone(_THIS->m_retCode);

	return true;
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

size_t HttpClient::_HeaderCallback(void *data, size_t size, size_t nmemb, void *userdata)
{
	HttpClient* _THIS = reinterpret_cast<HttpClient*>(userdata);
	std::string header(reinterpret_cast<char*>(data), size*nmemb);
	size_t seperator = header.find_first_of(':');
	if (std::string::npos == seperator) 
	{
		// roll with non seperated headers...
		trim(header);
		if (0 == header.length()) 
		{
			return (size * nmemb);  // blank line;
		}
		_THIS->m_responseHeaders[header].push_back("");
	}
	else {
		std::string key = header.substr(0, seperator);
		trim(key);
		std::string value = header.substr(seperator + 1);
		trim(value);
		_THIS->m_responseHeaders[key].push_back(value);
	}

	return (size * nmemb);
}