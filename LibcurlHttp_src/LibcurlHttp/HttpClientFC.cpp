#include "HttpClientFC.h"



HttpClientFC::HttpClientFC()
	: HttpClient()
	, m_progressCallback(NULL)
	, m_userData(NULL)
{
}


HttpClientFC::~HttpClientFC()
{
}

void HttpClientFC::SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData)
{
	m_progressCallback = progressCallback;
	m_userData = userData;
}

int HttpClientFC::OnProgress(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow)
{
	if (m_progressCallback)
		return m_progressCallback(downloadTotal, downloadNow, uploadTotal, uploadNow, m_userData);
	return 0;
}
