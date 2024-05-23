#include "HttpClientFC.h"



HttpClientFC::HttpClientFC()
	: HttpClient()
	, m_progressCallback(NULL)
	, m_userDataProgress(NULL)
	, m_headerCallback(NULL)
	, m_userDataHeader(NULL)
	, m_headerWriteBuff(true)
	, m_writedCallback(NULL)
	, m_userDataWrited(NULL)
	, m_bodyWriteBuff(false)
{
}


HttpClientFC::~HttpClientFC()
{
}

void HttpClientFC::SetHeaderCallback(FN_HEADER_CALLBACK cb, void* userData, bool writeBuff)
{
	m_headerCallback = cb;
	m_userDataHeader = userData;
	m_headerWriteBuff = writeBuff;
}

void HttpClientFC::SetBodyCallback(FN_WRITED_CALLBACK cb, void* userData, bool writeBuff)
{
	m_writedCallback = cb;
	m_userDataWrited = userData;
	m_bodyWriteBuff = writeBuff;
}

void HttpClientFC::SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData)
{
	m_progressCallback = progressCallback;
	m_userDataProgress = userData;
}

bool HttpClientFC::OnHeader(const char* header)
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

bool HttpClientFC::OnWrited(void* pBuffer, size_t nSize, size_t nMemByte)
{
	if (m_writedCallback)
	{
		if (m_bodyWriteBuff)
			__super::OnWrited(pBuffer, nSize, nMemByte);
		return m_writedCallback(pBuffer, nSize, nMemByte, m_userDataWrited);
	}
	else
	{
		return __super::OnWrited(pBuffer, nSize, nMemByte);
	}
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
