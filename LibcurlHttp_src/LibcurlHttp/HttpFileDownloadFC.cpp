#include "HttpFileDownloadFC.h"



HttpFileDownloadFC::HttpFileDownloadFC()
	: HttpFileDownload()
	, m_progressCallback(NULL)
	, m_userDataProgress(NULL)
{
}


HttpFileDownloadFC::~HttpFileDownloadFC()
{
}

void HttpFileDownloadFC::SetHeaderCallback(FN_HEADER_CALLBACK cb, void* userData)
{
	m_headerCallback = cb;
	m_userDataHeader = userData;
}

void HttpFileDownloadFC::SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData)
{
	m_progressCallback = progressCallback;
	m_userDataProgress = userData;
}

bool HttpFileDownloadFC::OnHeader(const char* header)
{
	__super::OnHeader(header);

	if (m_headerCallback)
		return m_headerCallback(header, m_userDataHeader);
	return true;
}

bool HttpFileDownloadFC::OnProgress(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow)
{
	__super::OnProgress(downloadTotal, downloadNow, uploadTotal, uploadNow);

	if (m_progressCallback)
		return m_progressCallback(downloadTotal, downloadNow, uploadTotal, uploadNow, m_userDataProgress);
	return true;
}
