#include "HttpFileDownload.h"


HttpFileDownload::HttpFileDownload()
	: m_fd(NULL)
{
	
}

HttpFileDownload::~HttpFileDownload()
{
	closeFile();
}

bool HttpFileDownload::SetFileName(const std::string& fileName)
{
	closeFile();

	m_file = fileName;
	fopen_s(&m_fd, m_file.c_str(), "wb+");

	return (m_fd != NULL);
}

void HttpFileDownload::OnDone(CURLcode code)
{
	closeFile();
}

bool HttpFileDownload::OnWrited(void* pBuffer, size_t nSize, size_t nMemByte)
{
	if (!m_fd)
		return false;
	size_t nWrite = fwrite(pBuffer, nSize, nMemByte, m_fd);
	return (nWrite == nSize * nMemByte);
}

int HttpFileDownload::OnProgress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	/*if (dltotal > -0.1 && dltotal < 0.1)
		return 0;

	int nPos = (int)((dlnow / dltotal) * 100);
	//通知进度条更新下载进度
	::PostMessage(hDlgWnd, WM_USER + 110, nPos, 0);
	*/
	return 0;
}

void HttpFileDownload::closeFile()
{
	if (m_fd)
	{
		fclose(m_fd);
		m_fd = NULL;
	}
}

bool HttpFileDownload::Do()
{
	if (m_file.empty())
	{
		std::string url = GetUrl();

		size_t idx = url.find_last_of("/\\");
		if (idx == std::string::npos)
			return false;

		std::string fileName = url.substr(idx + 1);
		if (strlen(fileName.c_str()) == 0)
			return false;

		if (!SetFileName(fileName))
			return false;
	}

	return __super::Do();
}
