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

int HttpClientFC::OnProgress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (m_progressCallback)
		return m_progressCallback(dltotal, dlnow, ultotal, ulnow, m_userData);
	return 0;
}
