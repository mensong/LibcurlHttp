#include "HttpClient.h"
#include <process.h>
#include <stdio.h>

///////////////////////////////////////////////////////////
#include <openssl/err.h>
#include <openssl/crypto.h>
#include <pystring.h>

#include <zconf.h>
#include <zlib.h>
#include <zutil.h>

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
	, m_errorCode(CURL_LAST)
	, m_httpCode(-1)
	, m_autoRedirect(true)
	, m_maxRedirect(5)
	, m_isInnerPost(false)
	, m_putData(NULL)
	, m_putDataLen(0)
	, m_responseBodyDecode(true)
{
	m_userAgent = 
		"Mozilla/999.0 (Windows NT 999) "
		"AppleWebKit/999.999 (KHTML, like Gecko) "
		"Chrome/9999.9999.9999.9999 "
		"Safari/9999.9999";
}

HttpClient::~HttpClient()
{
}

const std::string& HttpClient::GetCustomMethod(const std::string& methodDef /*= "GET"*/) const
{
	if (m_customMethod.empty() || m_customMethod == "")
	{
		return methodDef;
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
	HttpClient* _THIS = this;

	m_body.clear();
	bool res = false;
	m_httpCode = -1;
	m_errorCode = CURLcode::CURL_LAST;

	const std::string& url = GetUrl();
	if (url.c_str()[0] == '\0')
	{
		m_errorCode = CURLcode::CURLE_LDAP_INVALID_URL;
		return false;
	}
		
	CURL* curl = NULL;
	struct curl_httppost* post = NULL;
	curl_mime *mime = NULL;
	curl_slist* headerList = NULL;
	put_upload_ctx* upload_ctx = NULL;
	FILE* putFile = NULL;
	
	do 
	{
		//初始化curl，这个是必须的
		curl = curl_easy_init();
		//设置url
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		//自动进行解码
		if (m_responseBodyDecode)
			curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
		else
			curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, NULL);
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
		if (m_autoRedirect && m_maxRedirect > 0)
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
				m_errorCode = code;
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

#if 0
		if (m_formFields.size() > 0)
		{
			sMethod = GetCustomMethod("POST");
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
					m_errorCode = code;
					break;//return
				}
			}
			else
			{
				m_errorCode = CURLcode::CURLE_HTTP_POST_ERROR;
				break;//return
			}
		}
#endif
		
		if (m_postData.size() > 0)
		{
			sMethod = GetCustomMethod("POST");
			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			/** set post fields */
			CURLcode code = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, m_postData.c_str());
			if (code != CURLcode::CURLE_OK)
			{
				m_errorCode = code;
				break;//return
			}
			code = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, m_postData.size());
			if (code != CURLcode::CURLE_OK)
			{
				m_errorCode = code;
				break;//return
			}
		}
		else if (m_multipartFields.size() > 0)
		{
			sMethod = GetCustomMethod("POST");
			curl_easy_setopt(curl, CURLOPT_POST, 1L);

			mime = curl_mime_init(curl);
			if (mime)
			{
				for (int i = 0; i < m_multipartFields.size(); ++i)
				{
					const MultipartField* mpf = m_multipartFields[i];
					curl_mimepart *part = curl_mime_addpart(mime);

					//set data
					if (mpf->contenxtData)
					{
						curl_mime_data(part, mpf->contenxtData, 
							(mpf->contenxtDataSize > 0 ? mpf->contenxtDataSize : strlen(mpf->contenxtData)));
					}
					else if (mpf->filePath)
					{
						curl_mime_filedata(part, mpf->filePath);
					}

					//set name 
					if (mpf->multipartName)
						curl_mime_name(part, mpf->multipartName);

					//set filename
					if (mpf->fileName)
						curl_mime_filename(part, mpf->fileName);

					//set mime type
					if (mpf->mimeType)
						curl_mime_type(part, mpf->mimeType);
				}

				CURLcode code = curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
				if (code != CURLcode::CURLE_OK)
				{
					m_errorCode = code;
					break;//return
				}
			}
			else
			{
				m_errorCode = CURLcode::CURLE_HTTP_POST_ERROR;
				break;//return
			}
		}
		else if (m_putData || m_putFile.size() > 0)
		{
			sMethod = GetCustomMethod("PUT");
			curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

			if (m_putData)
			{
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, _put_read_data_callback);
				upload_ctx = new put_upload_ctx(m_putData, m_putDataLen);
				curl_easy_setopt(curl, CURLOPT_READDATA, upload_ctx);
				curl_off_t size = (curl_off_t)(m_putDataLen);
				if (size <= 2 * 1024 * 1024 * 1024)
					curl_easy_setopt(curl, CURLOPT_INFILESIZE, (LONG)size);
				else
					curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, size);
			}
			else if (m_putFile.size() > 0)
			{
				putFile = fopen(m_putFile.c_str(), "rb");
				if (!putFile)
				{
					m_errorCode = CURLcode::CURLE_FILE_COULDNT_READ_FILE;
					break;//return
				}

				//获取文件大小
				_fseeki64(putFile, 0, SEEK_END);
				curl_off_t fsize = _ftelli64(putFile);
				_fseeki64(putFile, 0, SEEK_SET);

				/* we want to use our own read function */
				curl_easy_setopt(curl, CURLOPT_READFUNCTION, _put_read_file_callback);
				/* now specify which pointer to pass to our callback */
				curl_easy_setopt(curl, CURLOPT_READDATA, putFile);
				/* Set the size of the file to upload */
				curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)fsize);
			}
		}
		else
		{
			if (m_isInnerPost)
				sMethod = GetCustomMethod("POST");
			else
				sMethod = GetCustomMethod("GET");
		}

		if (sMethod.c_str()[0] != '\0')
		{
			/** set custom method */
			CURLcode code = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, sMethod.c_str());
			if (code != CURLcode::CURLE_OK)
			{
				m_errorCode = code;
				break;//return
			}
		}

		/** 开始执行请求 */
		m_errorCode = curl_easy_perform(curl);
				
		if (m_errorCode != CURLE_OK)
		{
			//查看是否有出错信息
			const char* pError = curl_easy_strerror(m_errorCode);
			if (pError)
				m_body = pError;
		}
		else
		{
			////处理GZIP
			//bool isGzip = false;
			//if (m_responseBodyDecode && m_body.size() > 0)
			//{
			//	std::vector<std::string> ContentEncoding = GetResponseHeaders("Content-Encoding", true);
			//	for (size_t i = 0; i < ContentEncoding.size(); i++)
			//	{
			//		std::string v = pystring::lower(ContentEncoding[i]);
			//		if (v.find("gzip") != std::string::npos)
			//		{
			//			isGzip = true;
			//			break;
			//		}
			//	}
			//	if (isGzip)
			//	{
			//		std::string writeData;
			//		if (tryDecompressGzip((const char*)&m_body[0], m_body.size(), writeData))
			//		{
			//			m_body = writeData;
			//		}
			//	}
			//}

			int64_t http_code = 0;
			CURLcode code = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			if (code == CURLcode::CURLE_OK)
				m_httpCode = static_cast<int>(http_code);
			else
				m_errorCode = code;
		}

		OnDone(m_errorCode);
		res = true;

	} while (0);

	//善后
	if (upload_ctx)
		delete upload_ctx;
	if (putFile)
		fclose(putFile);
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

bool HttpClient::OnHeader(const char* header)
{
	std::string headerKV = header;
	size_t seperator = headerKV.find_first_of(':');
	if (std::string::npos == seperator)
	{
		// roll with non seperated headers...
		trim(headerKV);
		if (0 == headerKV.length())
		{
			return true;  // blank line;
		}
		m_responseHeaders[header].push_back("");
	}
	else
	{
		std::string key = headerKV.substr(0, seperator);
		trim(key);
		std::string value = headerKV.substr(seperator + 1);
		trim(value);
		m_responseHeaders[key].push_back(value);
	}

	return true;
}

bool HttpClient::OnWrited(void* pBuffer, size_t nSize, size_t nMemByte)
{
	size_t totalSize = nSize * nMemByte;
	m_body.append(reinterpret_cast<char*>(pBuffer), totalSize);
	return true;
}

bool HttpClient::OnProgress(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow)
{
	return true;
}

void HttpClient::OnDone(CURLcode code)
{
	
}

size_t HttpClient::_HeaderCallback(void* data, size_t size, size_t nmemb, void* userdata)
{
	//curl中，返回size * nmemb继续，其它则中断

	HttpClient* _THIS = reinterpret_cast<HttpClient*>(userdata);
	if (!_THIS)
		return 0;
	std::string header(reinterpret_cast<char*>(data), size * nmemb);
	if (_THIS->OnHeader(header.c_str()))
		return (size * nmemb);
	return 0;
}

size_t HttpClient::_WriteDataCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam)
{
	//curl中，返回nSize * nMemByte继续，其它则中断

	HttpClient* _THIS = (HttpClient*)pParam;
	if (!_THIS)
		return 0;
	if (_THIS->OnWrited(pBuffer, nSize, nMemByte))
		return nSize * nMemByte;
	return 0;
}

int HttpClient::_ProgressCallback(void * userData, 
	double downloadTotal, double downloadNow, 
	double uploadTotal, double uploadNow)
{
	//curl中，返回0继续，非0则中断

	HttpClient* _THIS = (HttpClient*)userData;
	if (!_THIS)
		return 1;
	if (_THIS->OnProgress(downloadTotal, downloadNow, uploadTotal, uploadNow))
		return 0;
	return 1;
}

size_t HttpClient::_put_read_file_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	FILE* src = (FILE*)userdata;
	/* copy as much data as possible into the 'ptr' buffer, but no more than
	   'size' * 'nmemb' bytes */
	size_t retcode = fread(ptr, size, nmemb, src);
	return retcode;
}

bool HttpClient::gzDecompress(const unsigned char* src, size_t srcLen, const unsigned char* dst, size_t dstLen, size_t* outLen)
{
	z_stream strm;
	strm.zalloc = NULL;
	strm.zfree = NULL;
	strm.opaque = NULL;

	strm.avail_in = srcLen;
	strm.avail_out = dstLen;
	strm.next_in = (Bytef*)src;
	strm.next_out = (Bytef*)dst;

	int err = -1;
	*outLen = 0;
	err = inflateInit2(&strm, MAX_WBITS + 16); /*zlib解压gz数据*/
	if (err == Z_OK)
	{
		err = inflate(&strm, Z_FINISH);
		if (err == Z_STREAM_END)
		{
			*outLen = strm.total_out; /* 解压成功 */
		}
	}

	inflateEnd(&strm);
	return err == Z_STREAM_END;
}

bool HttpClient::tryDecompressGzip(const char* src, size_t srcLen, std::string& outData)
{
	bool success = false;

	for (int i = 1; i <= 3; i++)
	{
		size_t buffSize = (srcLen * 10 + 3000) * i; /* 解压缓冲区大小，自动扩容 */
		size_t decompressedSize = 0;
		char* buffData = (char*)malloc(buffSize);
		memset(buffData, 0, buffSize);
		bool ret = gzDecompress(
			(const unsigned char*)src, srcLen,
			(const unsigned char*)buffData, buffSize, &decompressedSize);
		if (ret)
		{ /* 解压成功 */
			outData = buffData;
			free(buffData);
			success = true;
			break;
		}
		else
		{
			free(buffData); /* 解压失败，扩容后再次尝试 */
		}
	}

	return success;
}

size_t HttpClient::_put_read_data_callback(void* ptr, size_t size, size_t nmemb, void* stream)
{
	put_upload_ctx* ctx = (put_upload_ctx*)stream;
	size_t len = 0;

	if (ctx->pos >= ctx->end) {
		return 0;
	}

	if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1)) {
		return 0;
	}

	len = ctx->end - ctx->pos;
	if (len > size * nmemb) {
		len = size * nmemb;
	}

	memcpy(ptr, ctx->pos, len);
	ctx->pos += len;

	//printf("send len=%zu\n", len);

	return len;
}
