#pragma once
#include <map>
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>
#include <functional>
#include <curl/curl.h>
#include "../LibcurlHttp/LibcurlHttp.h"

///////////////////////////////////////////////////////////
//openssl 1.0.2 不是线程安全的，若要启动线程安全机制，需要自定义实现 openssl 用到的全局锁（大概是41个），并提供一个锁定/释放的 callback 函数
//https://curl.se/libcurl/c/threadsafe.html
#include <mutex>
class SslCurlWrapper
{
private:
	static std::vector<std::mutex> vectorOfSslMutex;

	static unsigned long id_function();
	static void locking_function(int mode, int n, const char * file, int line);

public:
	SslCurlWrapper();
	~SslCurlWrapper();
};
///////////////////////////////////////////////////////////

typedef std::map< std::string, std::vector<std::string> > ResponseHeaderFields;

class HttpClient
{
public:
	HttpClient();
	virtual ~HttpClient();

	//设置请求方法，一般为：OPTIONS GET POST PUT HEAD DELETE TRACE CONNECT，还可以为其它任意字符串
	virtual void SetCustomMethod(const std::string& method) { m_customMethod = method; }
	virtual const std::string& GetCustomMethod(const std::string& methodDef = "GET") const;

	virtual void SetUrl(const std::string& url) { m_url = url; }
	virtual const std::string& GetUrl() const { return m_url; }

	virtual void SetUserAgent(const std::string& val) { m_userAgent = val; }
	virtual const std::string GetUserAgent() const { return m_userAgent; }

	virtual void SetAutoRedirect(bool autoRedirect) { m_autoRedirect = autoRedirect; }
	virtual bool GetAutoRedirect() { return m_autoRedirect; }

	virtual void SetResponseBodyAutoDecode(bool decode) { m_responseBodyDecode = decode; }
	virtual bool GetResponseBodyAutoDecode() { return m_responseBodyDecode; }

	virtual void SetMaxRedirect(int maxRedirect) { m_maxRedirect = maxRedirect; }
	virtual int GetMaxRedirect() { return m_maxRedirect; }

	virtual void SetHeaders(const std::map<std::string, std::string>& headers) { m_headers = headers; }
	virtual void SetHeader(const std::string& key, const std::string& val);
	virtual const std::string& GetHeader(const std::string& key) const;

	//单位秒
	virtual void SetTimtout(int t) { m_timeout = t; }
	virtual int GetTimeout() const { return m_timeout; }

	//normal post
	virtual void ResetNormalPost() { m_postData.clear(); m_isInnerPost = false; }
	virtual void SetNormalPostData(const std::string& data) { m_postData = data; m_isInnerPost = true; }
	virtual const std::string& GetNormalPostData() { return m_postData; }

	//multipart post
	virtual void ResetMultipartFields() { 
		m_multipartFields.clear();
		m_isInnerPost = false; 
	}
	virtual void SetMultipartFields(const std::vector<MultipartField*>& fields) 
	{
		m_multipartFields = fields;
		m_isInnerPost = true; 
	}
	virtual const std::vector<MultipartField*>& GetMultipartFields() { return m_multipartFields; }

	//put 
	virtual void ResetPutData() { m_putData = NULL; m_putDataLen = 0; }
	virtual void SetPutData(const unsigned char* data, size_t dataLen) {
		m_putData = const_cast<unsigned char*>(data); 
		m_putDataLen = dataLen;
	}
	virtual const unsigned char* GetPutData(size_t& outDataLen) { 
		outDataLen = m_putDataLen; return m_putData;
	}

	virtual void ResetPutFile() { m_putFile.clear(); }
	virtual void SetPutFile(const std::string& filePath) { m_putFile = filePath; }
	virtual const std::string& GetPutFile() { return m_putFile; }

	//开始处理
	virtual bool Do();

	virtual CURLcode GetErrorCode() const { return m_errorCode; }
	virtual int GetHttpCode() const { return m_httpCode; }
	virtual const std::string& GetBody() { return m_body; }
	virtual const ResponseHeaderFields& GetResponseHeaders() { return m_responseHeaders; };
	virtual const std::vector<std::string>& GetResponseHeaders(const std::string& key, bool ignoreCase = true);

protected:
	//读取header，返回false则中断
	virtual bool OnHeader(const char* header);
	//读取数据，返回false则中断
	virtual bool OnWrited(void* pBuffer, size_t nSize, size_t nMemByte);
	//进度，返回false则中断
	virtual bool OnProgress(double downloadTotal, double downloadNow, double uploadTotal, double uploadNow);
	virtual void OnDone(CURLcode code);

protected:
	static size_t _HeaderCallback(void* data, size_t size, size_t nmemb, void* userdata);
	static size_t _WriteDataCallback(void* pBuffer, size_t nSize, size_t nMemByte, void* pParam);
	static int _ProgressCallback(void * userData, 
		double downloadTotal, double downloadNow, double uploadTotal, double uploadNow);

protected:
	typedef struct put_upload_ctx
	{
		unsigned char* start;
		unsigned char* pos;
		unsigned char* end;

		put_upload_ctx(unsigned char* data, size_t dataLen)
		{
			if (!data)
			{
				this->start = NULL;
				this->pos = NULL;
				this->end = NULL;
				return;
			}

			this->start = data;
			this->pos = this->start;
			this->end = this->start + dataLen;
		}
	} put_upload_ctx;
	static size_t _put_read_data_callback(void* ptr, size_t size, size_t nmemb, void* stream);

	static size_t _put_read_file_callback(char* ptr, size_t size, size_t nmemb, void* userdata);

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

	static bool gzDecompress(const unsigned char* src, size_t srcLen, const unsigned char* dst, size_t dstLen, size_t* outLen);
	static bool tryDecompressGzip(const char* src, size_t srcLen, std::string& outData);

protected:
	int m_timeout;
	std::string m_customMethod;
	bool m_isInnerPost;
	std::string m_url;
	std::string m_userAgent;
	bool m_autoRedirect;
	int m_maxRedirect;
	bool m_responseBodyDecode;

	std::map<std::string, std::string> m_headers;

	/**
		优先使用m_postData；
	*/
	std::string m_postData;
	std::vector<MultipartField*> m_multipartFields;

	//put m_putData与m_putFile二选一
	unsigned char* m_putData;
	size_t m_putDataLen;
	std::string m_putFile;

	CURLcode m_errorCode;
	/* 错误码如下:
		CURLE_UNSUPPORTED_PROTOCOL,		1 
		CURLE_FAILED_INIT,				2 
		CURLE_URL_MALFORMAT,            3 
		CURLE_NOT_BUILT_IN,             4 - [was obsoleted in August 2007 for
										  7.17.0, reused in April 2011 for 7.21.5] 
		CURLE_COULDNT_RESOLVE_PROXY,    5 
		CURLE_COULDNT_RESOLVE_HOST,     6 
		CURLE_COULDNT_CONNECT,          7 
		CURLE_WEIRD_SERVER_REPLY,       8 
		CURLE_REMOTE_ACCESS_DENIED,     9 a service was denied by the server
										  due to lack of access - when login fails
										  this is not returned. 
		CURLE_FTP_ACCEPT_FAILED,        10 - [was obsoleted in April 2006 for
										  7.15.4, reused in Dec 2011 for 7.24.0]
		CURLE_FTP_WEIRD_PASS_REPLY,     11 
		CURLE_FTP_ACCEPT_TIMEOUT,       12 - timeout occurred accepting server
										  [was obsoleted in August 2007 for 7.17.0,
										  reused in Dec 2011 for 7.24.0]
		CURLE_FTP_WEIRD_PASV_REPLY,     13 
		CURLE_FTP_WEIRD_227_FORMAT,     14 
		CURLE_FTP_CANT_GET_HOST,        15 
		CURLE_HTTP2,                    16 - A problem in the http2 framing layer.
										  [was obsoleted in August 2007 for 7.17.0,
										  reused in July 2014 for 7.38.0] 
		CURLE_FTP_COULDNT_SET_TYPE,     17 
		CURLE_PARTIAL_FILE,             18 
		CURLE_FTP_COULDNT_RETR_FILE,    19 
		CURLE_OBSOLETE20,               20 - NOT USED 
		CURLE_QUOTE_ERROR,              21 - quote command failure 
		CURLE_HTTP_RETURNED_ERROR,      22 
		CURLE_WRITE_ERROR,              23 
		CURLE_OBSOLETE24,               24 - NOT USED 
		CURLE_UPLOAD_FAILED,            25 - failed upload "command" 
		CURLE_READ_ERROR,               26 - couldn't open/read from file 
		CURLE_OUT_OF_MEMORY,            27 
		 Note: CURLE_OUT_OF_MEMORY may sometimes indicate a conversion error
				 instead of a memory allocation error if CURL_DOES_CONVERSIONS
				 is defined
		
		CURLE_OPERATION_TIMEDOUT,       28 - the timeout time was reached 
		CURLE_OBSOLETE29,               29 - NOT USED 
		CURLE_FTP_PORT_FAILED,          30 - FTP PORT operation failed 
		CURLE_FTP_COULDNT_USE_REST,     31 - the REST command failed 
		CURLE_OBSOLETE32,               32 - NOT USED 
		CURLE_RANGE_ERROR,              33 - RANGE "command" didn't work 
		CURLE_HTTP_POST_ERROR,          34 
		CURLE_SSL_CONNECT_ERROR,        35 - wrong when connecting with SSL 
		CURLE_BAD_DOWNLOAD_RESUME,      36 - couldn't resume download 
		CURLE_FILE_COULDNT_READ_FILE,   37 
		CURLE_LDAP_CANNOT_BIND,         38 
		CURLE_LDAP_SEARCH_FAILED,       39 
		CURLE_OBSOLETE40,               40 - NOT USED 
		CURLE_FUNCTION_NOT_FOUND,       41 - NOT USED starting with 7.53.0 
		CURLE_ABORTED_BY_CALLBACK,      42 
		CURLE_BAD_FUNCTION_ARGUMENT,    43 
		CURLE_OBSOLETE44,               44 - NOT USED 
		CURLE_INTERFACE_FAILED,         45 - CURLOPT_INTERFACE failed 
		CURLE_OBSOLETE46,               46 - NOT USED 
		CURLE_TOO_MANY_REDIRECTS,       47 - catch endless re-direct loops 
		CURLE_UNKNOWN_OPTION,           48 - User specified an unknown option 
		CURLE_TELNET_OPTION_SYNTAX,     49 - Malformed telnet option 
		CURLE_OBSOLETE50,               50 - NOT USED 
		CURLE_PEER_FAILED_VERIFICATION,  51 - peer's certificate or fingerprint
										   wasn't verified fine 
		CURLE_GOT_NOTHING,              52 - when this is a specific error 
		CURLE_SSL_ENGINE_NOTFOUND,      53 - SSL crypto engine not found 
		CURLE_SSL_ENGINE_SETFAILED,     54 - can not set SSL crypto engine as
										  default 
		CURLE_SEND_ERROR,               55 - failed sending network data 
		CURLE_RECV_ERROR,               56 - failure in receiving network data 
		CURLE_OBSOLETE57,               57 - NOT IN USE 
		CURLE_SSL_CERTPROBLEM,          58 - problem with the local certificate 
		CURLE_SSL_CIPHER,               59 - couldn't use specified cipher 
		CURLE_SSL_CACERT,               60 - problem with the CA cert (path?) 
		CURLE_BAD_CONTENT_ENCODING,     61 - Unrecognized/bad encoding 
		CURLE_LDAP_INVALID_URL,         62 - Invalid LDAP URL 
		CURLE_FILESIZE_EXCEEDED,        63 - Maximum file size exceeded 
		CURLE_USE_SSL_FAILED,           64 - Requested FTP SSL level failed 
		CURLE_SEND_FAIL_REWIND,         65 - Sending the data requires a rewind
										  that failed 
		CURLE_SSL_ENGINE_INITFAILED,    66 - failed to initialise ENGINE 
		CURLE_LOGIN_DENIED,             67 - user, password or similar was not
										  accepted and we failed to login 
		CURLE_TFTP_NOTFOUND,            68 - file not found on server 
		CURLE_TFTP_PERM,                69 - permission problem on server 
		CURLE_REMOTE_DISK_FULL,         70 - out of disk space on server 
		CURLE_TFTP_ILLEGAL,             71 - Illegal TFTP operation 
		CURLE_TFTP_UNKNOWNID,           72 - Unknown transfer ID 
		CURLE_REMOTE_FILE_EXISTS,       73 - File already exists 
		CURLE_TFTP_NOSUCHUSER,          74 - No such user 
		CURLE_CONV_FAILED,              75 - conversion failed 
		CURLE_CONV_REQD,                76 - caller must register conversion
										  callbacks using curl_easy_setopt options
										  CURLOPT_CONV_FROM_NETWORK_FUNCTION,
										  CURLOPT_CONV_TO_NETWORK_FUNCTION, and
										  CURLOPT_CONV_FROM_UTF8_FUNCTION 
		CURLE_SSL_CACERT_BADFILE,       77 - could not load CACERT file, missing
										  or wrong format 
		CURLE_REMOTE_FILE_NOT_FOUND,    78 - remote file not found 
		CURLE_SSH,                      79 - error from the SSH layer, somewhat
										  generic so the error message will be of
										  interest when this has happened 

		CURLE_SSL_SHUTDOWN_FAILED,      80 - Failed to shut down the SSL
										  connection 
		CURLE_AGAIN,                    81 - socket is not ready for send/recv,
										  wait till it's ready and try again (Added
										  in 7.18.2) 
		CURLE_SSL_CRL_BADFILE,          82 - could not load CRL file, missing or
										  wrong format (Added in 7.19.0) 
		CURLE_SSL_ISSUER_ERROR,         83 - Issuer check failed.  (Added in
										  7.19.0) 
		CURLE_FTP_PRET_FAILED,          84 - a PRET command failed 
		CURLE_RTSP_CSEQ_ERROR,          85 - mismatch of RTSP CSeq numbers 
		CURLE_RTSP_SESSION_ERROR,       86 - mismatch of RTSP Session Ids 
		CURLE_FTP_BAD_FILE_LIST,        87 - unable to parse FTP file list 
		CURLE_CHUNK_FAILED,             88 - chunk callback reported error 
		CURLE_NO_CONNECTION_AVAILABLE,  89 - No connection available, the
										  session will be queued 
		CURLE_SSL_PINNEDPUBKEYNOTMATCH,  90 - specified pinned public key did not
										   match 
		CURLE_SSL_INVALIDCERTSTATUS,    91 - invalid certificate status 
		CURLE_HTTP2_STREAM,             92 - stream error in HTTP/2 framing layer
										  
		CURLE_RECURSIVE_API_CALL,       93 - an api function was called from
										  inside a callback 
	*/
	int m_httpCode;
	std::string m_body;
	ResponseHeaderFields m_responseHeaders;

private:
	//多线程时防止崩溃
	static SslCurlWrapper s_sslObject;
};

