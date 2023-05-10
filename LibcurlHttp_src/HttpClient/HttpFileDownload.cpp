#include "HttpFileDownload.h"
#include "pystring.h"
#include "../HttpHelper/Convertor.h"
#include "../HttpHelper/UrlCoding.h"

HttpFileDownload::HttpFileDownload()
	: HttpClient()
	, m_fd(NULL)
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

std::string HttpFileDownload::getTimesampFileName()
{
	return std::to_string(::GetTickCount64()) + ".tmp";
}



bool HttpFileDownload::Do()
{
	bool needRename = false;
	std::string saveFileName;

	//如果没有设置保存文件
	if (m_file.empty())
	{
		std::string url = GetUrl();
		size_t idx = url.find_last_of("/\\");
		if (idx != std::string::npos)
		{
			saveFileName = url.substr(idx + 1);
		}
		if (saveFileName.size() == 0)
		{
			saveFileName = getTimesampFileName();
		}		

		needRename = true;
	}	
	else
	{
		//只设置了目录
		DWORD dwAttr = ::GetFileAttributesA(m_file.c_str());
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{// 如果是目录
			saveFileName = pystring::os::path::join(m_file, getTimesampFileName());
			needRename = true;
		}
		else
		{//设置了具体文件路径
			saveFileName = m_file;
		}
	}

	if (!SetFileName(saveFileName))
		return false;
	
	//执行下载
	bool ret = __super::Do();

	//从response中获得文件名
	if (needRename && ret)
	{
		//获取文件名编码
		bool bIsUtf8 = false;
		auto ContentType = this->GetResponseHeaders().find("Content-Type");
		if (ContentType != this->GetResponseHeaders().end())
		{
			for (size_t i = 0; i < ContentType->second.size(); i++)
			{
				const std::string& line = pystring::upper(ContentType->second[i]);
				if (line.find("UTF-8") != std::string::npos || line.find("UTF8") != std::string::npos)
					bIsUtf8 = true;
			}
		}

		//获取文件名
		auto ContentDisposition = this->GetResponseHeaders().find("Content-Disposition");
		if (ContentDisposition != this->GetResponseHeaders().end())
		{
			for (size_t i = 0; i < ContentDisposition->second.size(); i++)
			{
				const std::string& line = ContentDisposition->second[i];
				size_t idx = line.find("filename=");
				if (idx != std::string::npos)
				{
					std::string fileName = pystring::strip(line.substr(idx + 9), "\"");
					if (bIsUtf8)
					{
						fileName = GL::Utf82Ansi(fileName.c_str());
					}

					std::string targetFileName = pystring::os::path::dirname(saveFileName);
					if (!targetFileName.empty())
						targetFileName = pystring::os::path::join(targetFileName, fileName);
					else
						targetFileName = fileName;
					//执行重命名
					::MoveFileA(saveFileName.c_str(), targetFileName.c_str());
				}
			}
		}		
	}

	return ret;
}
