#pragma once
#include "..\HttpClient\HttpFileDownload.h"

class HttpFileDownloadFC
	: public HttpFileDownload
{
public:
	typedef bool (*FN_HEADER_CALLBACK)(const char* header, void* userData);
	typedef bool (*FN_PROGRESS_CALLBACK)(double downloadTotal, double downloadNow,
		double uploadTotal, double uploadNow, void* userData);

public:
	HttpFileDownloadFC();
	virtual ~HttpFileDownloadFC();

	void SetHeaderCallback(FN_HEADER_CALLBACK cb, void* userData);
	void SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData);

protected:
	virtual bool OnHeader(const char* header) override;
	virtual bool OnProgress(double downloadTotal, double downloadNow,
		double uploadTotal, double uploadNow) override;

private:
	FN_HEADER_CALLBACK m_headerCallback;
	void* m_userDataHeader;

	FN_PROGRESS_CALLBACK m_progressCallback;
	void* m_userDataProgress;
};

