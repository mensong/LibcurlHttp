#include "HttpFileDownloadFC.h"



HttpFileDownloadFC::HttpFileDownloadFC()
	: m_progressCallback(NULL)
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

int HttpFileDownloadFC::OnProgress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (m_progressCallback)
		return m_progressCallback(dltotal, dlnow, ultotal, ulnow, m_userData);
	return 0;
}
