#pragma once
#include "HttpClient.h"

class HttpFileDownload
	: public HttpClient
{
public:
	HttpFileDownload();
	virtual ~HttpFileDownload();

	virtual bool SetFile(const std::string& fileName);
	virtual std::string GetFile() const { return m_file; }

	virtual bool Do() override;

protected:
	virtual void OnDone(CURLcode code) override;
	virtual bool OnWrited(void* pBuffer, size_t nSize, size_t nMemByte) override;
	virtual int OnProgress(double dltotal, double dlnow, double ultotal, double ulnow) override;
		
protected:
	virtual void closeFile();

protected:
	std::string getTimesampFileName();
	bool createDirs(const std::string& dir);
	std::string generateSaveFile(const std::string& dir = "");

protected:
	std::string m_file;
private:
	FILE* m_fd;
};

