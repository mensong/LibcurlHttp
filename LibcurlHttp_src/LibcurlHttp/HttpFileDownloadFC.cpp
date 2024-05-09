#include "HttpFileDownloadFC.h"



HttpFileDownloadFC::HttpFileDownloadFC()
	: HttpFileDownload()
	, m_progressCallback(NULL)
	, m_userData(NULL)
{
}


HttpFileDownloadFC::~HttpFileDownloadFC()
{
}

void HttpFileDownloadFC::SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData)
{
	m_progressCallback = progressCallback;
	m_userData = userData;
}

int HttpFileDownloadFC::OnProgress(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow)
{
	if (m_progressCallback)
		return m_progressCallback(downloadTotal, downloadNow, uploadTotal, uploadNow, m_userData);
	return 0;
}
