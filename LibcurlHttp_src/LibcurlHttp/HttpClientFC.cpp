#include "HttpClientFC.h"



HttpClientFC::HttpClientFC()
	: HttpClient()
	, m_progressCallback(NULL)
	, m_userDataProgress(NULL)
	, m_headerCallback(NULL)
	, m_userDataHeader(NULL)
	, m_writedCallback(NULL)
	, m_userDataWrited(NULL)
{
}


HttpClientFC::~HttpClientFC()
{
}

void HttpClientFC::SetHeaderCallback(FN_HEADER_CALLBACK cb, void* userData)
{
	m_headerCallback = cb;
	m_userDataHeader = userData;
}

void HttpClientFC::SetBodyCallback(FN_WRITED_CALLBACK cb, void* userData)
{
	m_writedCallback = cb;
	m_userDataWrited = userData;
}

void HttpClientFC::SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData)
{
	m_progressCallback = progressCallback;
	m_userDataProgress = userData;
}

bool HttpClientFC::OnHeader(const char* header)
{
	__super::OnHeader(header);

	if (m_headerCallback)
		return m_headerCallback(header, m_userDataHeader);
	return true;
}

bool HttpClientFC::OnWrited(void* pBuffer, size_t nSize, size_t nMemByte)
{
	__super::OnWrited(pBuffer, nSize, nMemByte);

	if (m_writedCallback)
		return m_writedCallback(pBuffer, nSize, nMemByte, m_userDataWrited);
	return true;
}

bool HttpClientFC::OnProgress(
	double downloadTotal, double downloadNow, double uploadTotal, double uploadNow)
{
	__super::OnProgress(downloadTotal, downloadNow, uploadTotal, uploadNow);

	if (m_progressCallback)
	{
		return m_progressCallback(downloadTotal, downloadNow,
			uploadTotal, uploadNow, m_userDataProgress);
	}
	return true;
}
