#include "HttpHelper.h"
#include "url.hpp"
#include <pystring.h>

COOKIE_INFO HttpHelper::Cookies;
//std::recursive_mutex HttpHelper::_cookiesMutex;

static std::string ReadText(const char * path)
{
	FILE *f = NULL;
	long sz;

	if (!path)
	{
		return "";
	}

	std::string sRet;

	f = fopen(path, "rb");
	if (!f)
	{
		return "";
	}

	do
	{
		if (fseek(f, 0, SEEK_END) < 0)
		{
			break;
		}

		sz = ftell(f);
		if (sz < 0)
		{
			break;
		}

		if (fseek(f, 0, SEEK_SET) < 0)
		{
			break;
		}

		sRet.resize((size_t)sz + 1, '\0');

		if ((size_t)fread(const_cast<char*>(sRet.c_str()), 1, (size_t)sz, f) != (size_t)sz)
		{
			sRet = "";
			break;
		}
	} while (0);

	fclose(f);

	return sRet;
}

static void StringSplitA(const std::string & s, const std::string & delim, std::vector<std::string>& ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret.push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last > 0)
	{
		ret.push_back(s.substr(last, index - last));
	}
}

void StringTrimA(std::string& str)
{
	if (!str.empty())
	{
		str.erase(0, str.find_first_not_of(" \n\r\t"));
		str.erase(str.find_last_not_of(" \n\r\t") + 1);
	}
}

HttpHelper::HttpHelper()
	: timeOut(0)
	, maxRedirectNum(10)
{
	userAgent = "Mozilla/5.0 (Windows NT 10.0; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.139 Safari/537.36";
	headers["Accept"] = "*/*";
	headers["Accept-Language"] = "zh-CN,zh;q=0.9";
	//headers["Connection"] = "keep-alive";
}

HttpHelper::~HttpHelper()
{
}

int HttpHelper::get(const char* url, bool dealRedirect/* = true*/)
{
	// initialize RestClient
	RestClient::init();

	// get a connection object
	RestClient::Connection* conn = new RestClient::Connection("");

	// set connection timeout to 5s
	conn->SetTimeout(timeOut);

	// set custom user agent
	conn->SetUserAgent(userAgent);

	// set headers
	conn->SetHeaders(headers);

	m_sLocation = url;

	RestoreCookies(conn, url);

	response = conn->get(url);

	//cookies
	StoreCookiesFromResponse(response, url);

	//do redirect
	if (dealRedirect)
	{
		int count = 1;
		int mark = (int)response.code / (int)100;
		while (mark == 3)
		{
			std::vector<std::string> urls = HasHeader(response.headers, "Location");
			if (urls.size() < 1)
				break;
			m_sLocation = LinkUrl(m_sLocation, urls[0]);

			RestoreCookies(conn, m_sLocation.c_str());
			response = conn->get(m_sLocation);
			//cookies
			StoreCookiesFromResponse(response, m_sLocation.c_str());
			mark = (int)response.code / (int)100;

			if (count >= maxRedirectNum)
				break;
			++count;
		}
	}
	
	// deinit RestClient. After calling this you have to call RestClient::init()
	// again before you can use it
	RestClient::disable();
	delete conn;

	return response.code;
}

int HttpHelper::get(const char* url, const std::map<std::string, std::string>& fields, bool dealRedirect /*= true*/)
{
	std::string sUrl = url;
	char cLast = sUrl[sUrl.length() - 1];
	if (cLast != '?' && cLast != '&')
	{
		Url uri(url);
		if (uri.query().size() > 0)
		{//url中存在参数
			sUrl += '&';
		}
		else
		{
			sUrl += '?';
		}
	}

	//构造参数
	for (std::map<std::string, std::string>::const_iterator cit = fields.cbegin(); cit != fields.cend(); ++cit)
	{
		if (cit != fields.cbegin())
			sUrl += '&';
		sUrl += cit->first + "=" + cit->second;
	}

	return get(sUrl.c_str(), dealRedirect);
}

int HttpHelper::post(const char* url, const char* content, int contentLen, bool dealRedirect/* = true*/,
	const char* contentType/*="application/x-www-form-urlencoded"*/)
{
	// initialize RestClient
	RestClient::init();

	// get a connection object
	RestClient::Connection* conn = new RestClient::Connection("");

	// set connection timeout to 5s
	conn->SetTimeout(timeOut);

	// set custom user agent
	conn->SetUserAgent(userAgent);

	// set headers
	conn->SetHeaders(headers);
	conn->AppendHeader("Content-Type", contentType);
	char sContentLen[10] = { 0 };
	sprintf(sContentLen, "%d", contentLen);
	conn->AppendHeader("Content-Length", sContentLen);

	m_sLocation = url;

	//set cookies
	RestoreCookies(conn, url);

	//do 
	response = conn->post(url, content);

	//cookies
	StoreCookiesFromResponse(response, url);
	
	//do redirect
	if (dealRedirect)
	{
		int count = 1;
		int mark = (int)response.code / (int)100;
		while (mark == 3)
		{
			std::vector<std::string> urls = HasHeader(response.headers, "Location");
			if (urls.size() < 1)
				break;
			m_sLocation = LinkUrl(m_sLocation, urls[0]);

			RestoreCookies(conn, m_sLocation.c_str());
			response = conn->get(m_sLocation);
			//cookies
			StoreCookiesFromResponse(response, m_sLocation.c_str());
			mark = (int)response.code / (int)100;

			if (count >= maxRedirectNum)
				break;
			++count;
		}
	}

	// deinit RestClient. After calling this you have to call RestClient::init()
	// again before you can use it
	RestClient::disable();
	delete conn;

	return response.code;
}

int HttpHelper::post(const char* url, const std::map<std::string, std::string>& fields, 
	bool dealRedirect /*= true*/, const char* contentType /*= "application/x-www-form-urlencoded"*/)
{
	std::string sContet;
	for (std::map<std::string, std::string>::const_iterator cit = fields.cbegin();
		cit != fields.end(); ++cit)
	{
		if (sContet.empty())
			sContet = cit->first + "=" + cit->second;
		else
			sContet += "&" + cit->first + "=" + cit->second;
	}
	return post(url, sContet.c_str(), sContet.length(), dealRedirect, contentType);
}

const char* HttpHelper::getLocation()
{
	return m_sLocation.c_str();
}

void HttpHelper::RestoreCookies(RestClient::Connection* conn, const char* url)
{
	//_cookiesMutex.lock();

	Url urlParser(url);
	std::string sHost = "." + urlParser.host();
	std::string sPath = urlParser.path();
	std::transform(sHost.begin(), sHost.end(), sHost.begin(), ::toupper);
	if (sPath.empty())
		sPath = "/";
	
	std::string sCookies;
	for (COOKIE_INFO::iterator it = Cookies.begin(); it != Cookies.end(); ++it)
	{
		std::string host;
		std::string path;
		int idxPath = pystring::find(it->first, "/");
		if (idxPath < 0)
		{
			host = it->first;
			path = "/";
		}
		else
		{
			host = pystring::slice(it->first, 0, idxPath);
			path = pystring::slice(it->first, idxPath);
		}
		
		std::transform(host.begin(), host.end(), host.begin(), ::toupper);

		//检查域
		if (!pystring::endswith(sHost, host))
			continue;
		//检查路径
		if (!pystring::startswith(sPath, path))
			continue;

		for (std::map<std::string, std::string>::iterator it1 = it->second.begin();
			it1 != it->second.end(); ++it1)
		{
			if (!sCookies.empty())
				sCookies += "; ";
			sCookies += it1->first + "=" + it1->second;
		}
	}

	if (!sCookies.empty())
		conn->AppendHeader("Cookie", sCookies);

	//_cookiesMutex.unlock();
}

void HttpHelper::StoreCookiesFromResponse(RestClient::Response &r, const char* url)
{
	//_cookiesMutex.lock();

	Url urlParser(url);
	std::string sHost = urlParser.host();
	//std::transform(sHost.begin(), sHost.end(), sHost.begin(), ::toupper);

	std::vector<std::string> vctCookies = HasHeader(r.headers, "Set-Cookie");
	if (vctCookies.size() > 0)
	{
		for (int i = 0; i < vctCookies.size(); ++i)
		{
			std::string acookie = vctCookies[i];
			std::vector<std::string> fields;
			StringSplitA(acookie, ";", fields);
			if (fields.size() < 1)
				continue;

			//cookie key value
			std::string sCookieField = fields[0];
			StringTrimA(sCookieField);
			size_t idx = sCookieField.find("=");
			if (idx == std::string::npos)
				continue;
			std::string sCookieKey = sCookieField.substr(0, idx);
			std::string sCookieVal = sCookieField.substr(idx + 1);

			//Get other Cookie info
			std::string sDomain;
			std::string sPath = "/";
			for (int j = 1; j < fields.size(); ++j)
			{
				std::string sField = fields[j];
				StringTrimA(sField);
				size_t idx = sField.find("=");
				if (idx == std::string::npos)
					continue;
				std::string sKey = pystring::upper(sField.substr(0, idx));
				std::string sVal = sField.substr(idx + 1);
				if (sKey == "DOMAIN" && !sVal.empty())
				{
					std::transform(sVal.begin(), sVal.end(), sVal.begin(), ::toupper);
					sDomain = sVal;
				}
				else if (sKey == "PATH" && !sVal.empty())
				{
					sPath = sVal;
				}
			}

			if (sDomain.empty())
				sDomain = sHost;
			Cookies[sDomain + sPath].insert(std::make_pair(sCookieKey, sCookieVal));
		}
	}

	//_cookiesMutex.unlock();
}

void HttpHelper::LoadCookies(const char* cookieFile)
{
	/*
	* START
	*	Domain:a.b.com
	*	Path:/
	*	Cookies:key1=value1;key2=value2;key3=value3
	* END
	* START
	*	Domain:c.d.com
	*	Path:/Gergul
	*	Cookies:key1=value1;key2=value2;key3=value3
	* END
	*/
	std::string sCookiesContent = ReadText(cookieFile);
	std::vector<std::string> vctLines;
	pystring::splitlines(sCookiesContent, vctLines);

	bool bStarted = false;
	std::string sDomain;
	std::string sPath = "/";
	std::string sCookies;
	for (int i = 0; i < vctLines.size(); ++i)
	{
		std::string str = pystring::strip(vctLines[i]);
		std::string STR_CMD = pystring::upper(pystring::replace(str, " ", ""));
		if (STR_CMD == "START")
		{
			bStarted = true;

			continue;
		}

		if (!bStarted)
			continue;

		if (pystring::startswith(STR_CMD, "DOMAIN:"))
		{
			int idx = pystring::find(str, ":");
			sDomain = pystring::strip(pystring::slice(str, idx + 1));

			continue;
		}

		if (pystring::startswith(STR_CMD, "PATH:"))
		{
			int idx = pystring::find(str, ":");
			sPath = pystring::strip(pystring::slice(str, idx + 1));

			continue;
		}

		if (pystring::startswith(STR_CMD, "COOKIES:"))
		{
			int idx = pystring::find(str, ":");
			sCookies = pystring::slice(str, idx + 1);

			continue;
		}

		if (STR_CMD == "END")
		{
			bStarted = false;

			std::vector<std::string> vctKV;
			pystring::split(sCookies, vctKV, ";");
			for (int kv = 0; kv < vctKV.size(); ++kv)
			{
				std::string& sKV = vctKV[kv];
				int idxEQ = pystring::find(sKV, "=");
				if (idxEQ < 1)
					continue;
				std::string sKey = pystring::strip(pystring::slice(sKV, 0, idxEQ));
				std::string sVal = pystring::strip(pystring::slice(sKV, idxEQ + 1));
				HttpHelper::Cookies[sDomain + sPath][sKey] = sVal;
			}

			sDomain = "";
			sPath = "/";
			sCookies = "";

			continue;
		}
	}
}

void HttpHelper::SaveCookies(const char* cookieFile)
{
	FILE *f = NULL;
	//文件不存在，则新建一个空的
	if ((f = fopen(cookieFile, "wb")) == NULL)
	{
		return;
	}

	const char* START = "START";
	const char* END = "\nEND\n";

	const char* DOMAIN_ = "\n  Domain:";
	const char* PATH = "\n  Path:";
	const char* COOKIES = "\n  Cookies:";	

	for (COOKIE_INFO::iterator it = Cookies.begin(); it != Cookies.end(); ++it)
	{
		std::string host;
		std::string path;
		int idxPath = pystring::find(it->first, "/");
		if (idxPath < 0)
		{
			host = it->first;
			path = "/";
		}
		else
		{
			host = pystring::slice(it->first, 0, idxPath);
			path = pystring::slice(it->first, idxPath);
		}

		fwrite(START, strlen(START), 1, f);

		fwrite(DOMAIN_, strlen(DOMAIN_), 1, f);
		fwrite(host.c_str(), host.length(), 1, f);

		fwrite(PATH, strlen(PATH), 1, f);
		fwrite(path.c_str(), path.length(), 1, f);

		fwrite(COOKIES, strlen(COOKIES), 1, f);
		std::string sCookies;
		for (std::map<std::string, std::string>::iterator itC = it->second.begin(); itC != it->second.end(); ++itC)
		{
			if (!sCookies.empty())
				sCookies += "; ";
			sCookies += itC->first + "=" + itC->second;
		}
		fwrite(sCookies.c_str(), sCookies.length(), 1, f);

		fwrite(END, strlen(END), 1, f);
	}

	fclose(f);
}

std::vector<std::string> HttpHelper::HasHeader(const RestClient::ResponseHeaderFields& headers, 
	const char* headerName, bool ignoreCase /*= true*/)
{
	if (!ignoreCase)
	{
		RestClient::ResponseHeaderFields::const_iterator itFinder = headers.find(headerName);
		if (itFinder == headers.cend() || itFinder->second.size() == 0)
			return std::vector<std::string>();
		return itFinder->second;
	}

	std::string sHeaderName = pystring::upper(headerName);
	for (RestClient::ResponseHeaderFields::const_iterator it = headers.begin();
		it != headers.end(); ++it)
	{
		if (pystring::upper(it->first) == sHeaderName)
			return it->second;
	}

	return std::vector<std::string>();
}

std::string HttpHelper::GetUrlHost(const std::string& url)
{
	if (url.empty())
		return "";

	Url urlParser(url);
	
	std::string sRet = urlParser.scheme() + "://" + urlParser.host();

	std::string sPort = urlParser.port();
	if (!sPort.empty())
		sRet += ":" + sPort;

	return sRet;
}

std::string HttpHelper::GetUrlPath(const std::string& url)
{
	if (url.empty())
		return "";

	Url urlParser(url);

	std::string sHost = urlParser.scheme() + "://" + urlParser.host();

	std::string sPathUrl = urlParser.path();
	int n = pystring::rfind(sPathUrl, "/");
	if (n < 0)
		sPathUrl = "";
	else
		sPathUrl = pystring::slice(sPathUrl, 0, n);
	if (!pystring::startswith(sPathUrl, "/"))
		sPathUrl = sHost + "/" + sPathUrl;
	else
		sPathUrl = sHost + sPathUrl;

	return sPathUrl;
}

std::string HttpHelper::LinkUrl(const std::string& sBaseUrl, const std::string& href)
{
	std::string sHost = GetUrlHost(sBaseUrl);
	std::string sPathUrl = GetUrlPath(sBaseUrl);
	sPathUrl = pystring::strip(sPathUrl, "/");

	std::string sUrl = pystring::upper(href);

	if (pystring::startswith(sUrl, "/") || pystring::startswith(sUrl, "\\"))
		sUrl = sHost + href;//连接根
	else if (pystring::startswith(sUrl, "HTTP://") || pystring::startswith(sUrl, "HTTPS://"))
		sUrl = href;//完整的url
	else if (!pystring::startswith(sUrl, "JAVASCRIPT:"))
		sUrl = sPathUrl + "/" + href;//当为路径
	else
		sUrl = "";

	return sUrl;
}
