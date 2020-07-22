#pragma once
#include "..\HttpClient\HttpFileDownload.h"

class HttpFileDownloadFC
	: public HttpFileDownload
{
public:
	typedef int(*FN_PROGRESS_CALLBACK)(
		double dltotal,
		double dlnow,
		double ultotal,
		double ulnow,
		void* userData);

public:
	HttpFileDownloadFC();
	virtual ~HttpFileDownloadFC();

	void SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData);

protected:
	virtual int OnProgress(double dltotal, double dlnow, double ultotal, double ulnow) override;
	FN_PROGRESS_CALLBACK m_progressCallback;
	void* m_userData;
};

