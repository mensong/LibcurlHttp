#pragma once
#include <map>
#include "restclient-cpp/connection.h"
#include "restclient-cpp/restclient.h"
#include "restclient-cpp/helpers.h"
//#include <mutex>

typedef std::map<std::string, std::map<std::string, std::string> > COOKIE_INFO;

class HttpHelper
{
public:
	HttpHelper();
	~HttpHelper();

	RestClient::RequestHeaderFields headers;
	std::string userAgent;
	int timeOut;
	int maxRedirectNum;

	RestClient::Response response;

	/* 以下所有提交的数据均需要自行urlencode */
	int get(const char* url, bool dealRedirect = true);
	int get(const char* url, const std::map<std::string, std::string>& fields, bool dealRedirect = true);
	int post(const char* url, const char* content, int contentLen, bool dealRedirect = true, const char* contentType = "application/x-www-form-urlencoded");
	int post(const char* url, const std::map<std::string, std::string>& fields, bool dealRedirect = true, const char* contentType = "application/x-www-form-urlencoded");
	
	const char* getLocation();

	static COOKIE_INFO Cookies;
	static void LoadCookies(const char* cookieFile);
	static void SaveCookies(const char* cookieFile);

	static std::vector<std::string> HasHeader(const RestClient::ResponseHeaderFields& headers, const char* headerNmae, bool ignoreCase = true);
	static std::string GetUrlHost(const std::string& url);
	static std::string GetUrlPath(const std::string& url);
	static std::string LinkUrl(const std::string& sBaseUrl, const std::string& href);

protected:
	//static std::recursive_mutex _cookiesMutex;
	static void StoreCookiesFromResponse(RestClient::Response &r, const char* url);
	static void RestoreCookies(RestClient::Connection* conn, const char* url);

	std::string m_sLocation;
};

