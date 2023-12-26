#include "HttpClient.h"
#include <process.h>

///////////////////////////////////////////////////////////
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <pystring.h>

std::vector<std::mutex> SslCurlWrapper::vectorOfSslMutex(CRYPTO_num_locks());

unsigned long SslCurlWrapper::id_function()
{
	return *(unsigned int*)&(std::this_thread::get_id());
}

void SslCurlWrapper::locking_function(int mode, int n, const char * file, int line)
{
	if (mode & CRYPTO_LOCK)
		vectorOfSslMutex[n].lock();
	else
		vectorOfSslMutex[n].unlock();
}

SslCurlWrapper::SslCurlWrapper()
{
	CRYPTO_set_id_callback(id_function);
	CRYPTO_set_locking_callback(locking_function);
}

SslCurlWrapper::~SslCurlWrapper()
{
	CRYPTO_set_id_callback(nullptr);
	CRYPTO_set_locking_callback(nullptr);
}

SslCurlWrapper HttpClient::s_sslObject;
///////////////////////////////////////////////////////////

HttpClient::HttpClient()
	: m_timeout(0)
	, m_retCode(CURL_LAST)
	, m_httpCode(-99999)
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

	const std::string& url = GetUrl();
	if (url.c_str()[0] == '\0')
	{
		m_httpCode = HttpDoErrorCode::ecUrlError;
		return false;
	}

	bool res = false;
	CURL* curl = NULL;
	struct curl_httppost* post = NULL;
	curl_mime *mime = NULL;
	curl_slist* headerList = NULL;

	do 
	{

		//初始化curl，这个是必须的
		curl = curl_easy_init();
		//设置url
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
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, m_autoRedirect ? 1L : 0L);
		// 设置重定向的最大次数
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, m_maxRedirect);

		//多线程情况下必须开启
		//https://curl.se/libcurl/c/threadsafe.html
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

		/** set user agent */
		if (GetUserAgent().c_str()[0] != '\0')
			curl_easy_setopt(curl, CURLOPT_USERAGENT, GetUserAgent().c_str());

		/** set headers */
		std::string headerString;
		for (std::map<std::string, std::string>::iterator it = m_headers.begin();
		it != m_headers.end(); ++it)
		{
			if (it->first.empty())
				continue;
			headerString = it->first;
			headerString += ": ";
			headerString += it->second;
			headerList = curl_slist_append(headerList, headerString.c_str());
		}
		if (headerList)
		{
			CURLcode code = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
			if (code != CURLcode::CURLE_OK)
			{
				m_httpCode = code;
				break;//return
			}
		}

		/** time out */
		if (GetTimeout() > 0)
		{
			//连接超时，这个数值如果设置太短可能导致数据请求不到就断开了
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, GetTimeout());
			//接收数据时超时设置，如果GetTimeout()秒内数据未接收完，直接退出
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, GetTimeout());
			//服务器返回超时
			curl_easy_setopt(curl, CURLOPT_SERVER_RESPONSE_TIMEOUT, GetTimeout());
			//100 continue等待超时
			curl_easy_setopt(curl, CURLOPT_EXPECT_100_TIMEOUT_MS, GetTimeout());						
		}

		/** set body */
		std::string sMethod;		
		if (m_formFields.size() > 0)
		{
			sMethod = GetCustomMothod("POST");
			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			//post form
			struct curl_httppost* last = NULL;
			for (int i = 0; i < (int)m_formFields.size(); ++i)
			{
				CURLFORMcode formCode = CURL_FORMADD_OK;
				FormField& ff = m_formFields[i];
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
			if (post)
			{
				CURLcode code = curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
				if (code != CURLcode::CURLE_OK)
				{
					m_httpCode = code;
					break;//return
				}
			}
			else
			{
				m_httpCode = HttpDoErrorCode::ecDataError;
				break;//return
			}
		}
		else if (m_postData.size() > 0)
		{
			sMethod = GetCustomMothod("POST");
			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			/** set post fields */
			CURLcode code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_postData.c_str());
			if (code != CURLcode::CURLE_OK)
			{
				m_httpCode = code;
				break;//return
			}
			code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, m_postData.size());
			if (code != CURLcode::CURLE_OK)
			{
				m_httpCode = code;
				break;//return
			}
		}
		else if (m_multipartFields.size() > 0)
		{
			sMethod = GetCustomMothod("POST");
			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			mime = curl_mime_init(curl);
			if (mime)
			{
				for (int i = 0; i < m_multipartFields.size(); ++i)
				{
					const MultipartField& mpf = m_multipartFields[i];
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

				CURLcode code = curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
				if (code != CURLcode::CURLE_OK)
				{
					m_httpCode = code;
					break;//return
				}
			}
			else
			{
				m_httpCode = HttpDoErrorCode::ecDataError;
				break;//return
			}
		}
		else
		{
			if (m_isInnerPost)
				sMethod = GetCustomMothod("POST");
			else
				sMethod = GetCustomMothod("GET");
		}

		if (sMethod.c_str()[0] != '\0')
		{
			/** set custom method */
			CURLcode code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sMethod.c_str());
			if (code != CURLcode::CURLE_OK)
			{
				m_httpCode = code;
				break;//return
			}
		}

		/** 开始执行请求 */
		m_retCode = curl_easy_perform(curl);

		if (m_retCode != CURLE_OK)
		{
			m_httpCode = m_retCode;
			//查看是否有出错信息
			const char* pError = curl_easy_strerror(m_retCode);
			if (pError)
				m_body = pError;
		}
		else
		{
			int64_t http_code = 0;
			CURLcode code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			if (code == CURLcode::CURLE_OK)
				m_httpCode = static_cast<int>(http_code);
			else
				m_httpCode = code;
		}

		OnDone(m_retCode);
		res = true;

	} while (0);

	//善后
	if (post)
		curl_formfree(post);
	if (mime)
		curl_mime_free(mime);
	if (headerList)
		curl_slist_free_all(headerList);
	if (curl)
		curl_easy_cleanup(curl);

	return res;
}

const std::vector<std::string>& HttpClient::GetResponseHeaders(const std::string& key, bool ignoreCase)
{
	ResponseHeaderFields::iterator itFinder;
	if (!ignoreCase)
	{
		itFinder = m_responseHeaders.find(key);
	}
	else
	{
		for (itFinder = m_responseHeaders.begin(); itFinder != m_responseHeaders.end(); ++itFinder)
		{
			if (pystring::equal(itFinder->first, key, true))
			{
				break;
			}
		}
	}

	if (itFinder == m_responseHeaders.end())
	{
		static std::vector<std::string> emptyVct;
		return emptyVct;
	}

	return itFinder->second;
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