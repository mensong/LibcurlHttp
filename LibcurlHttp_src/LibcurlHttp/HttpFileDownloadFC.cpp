#include "HttpFileDownloadFC.h"



HttpFileDownloadFC::HttpFileDownloadFC()
	: HttpFileDownload()
	, m_progressCallback(NULL)
	, m_userDataProgress(NULL)
	, m_headerWriteBuff(false)
{
}


HttpFileDownloadFC::~HttpFileDownloadFC()
{
}

void HttpFileDownloadFC::SetHeaderCallback(FN_HEADER_CALLBACK cb, void* userData, bool writeBuff)
{
	m_headerCallback = cb;
	m_userDataHeader = userData;
	m_headerWriteBuff = writeBuff;
}

void HttpFileDownloadFC::SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData)
{
	m_progressCallback = progressCallback;
	m_userDataProgress = userData;
}

bool HttpFileDownloadFC::OnHeader(const char* header)
{
	if (m_headerCallback)
	{
		if (m_headerWriteBuff)
			__super::OnHeader(header);
		return m_headerCallback(header, m_userDataHeader);
	}
	else
	{
		return __super::OnHeader(header);
	}
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
