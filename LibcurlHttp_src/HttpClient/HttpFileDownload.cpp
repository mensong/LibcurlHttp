#include "HttpFileDownload.h"
#include "pystring.h"
#include "../HttpHelper/Convertor.h"
#include "../HttpHelper/UrlCoding.h"
#include "../HttpHelper/url.hpp"
#include <thread>
#include <sstream>

HttpFileDownload::HttpFileDownload()
	: HttpClient()
	, m_fd(NULL)
{
	
}

HttpFileDownload::~HttpFileDownload()
{
	closeFile();
}

bool HttpFileDownload::SetFile(const std::string& fileName)
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

int HttpFileDownload::OnProgress(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow)
{
	//if (uploadTotal != 0)
	//{
	//	int nPos = (int)((uploadNow / uploadTotal) * 100);
	//	//通知进度条更新上传进度
	//	::PostMessage(hDlgWnd, WM_USER + 110, nPos, 0);
	//}
	//if (downloadTotal != 0)
	//{
	//	int nPos = (int)((downloadNow / downloadTotal) * 100);
	//	//通知进度条更新下载进度
	//	::PostMessage(hDlgWnd, WM_USER + 111, nPos, 0);
	//}
	
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
	std::stringstream ss;
	ss << std::this_thread::get_id();
	ss << "-";
	ss << ::GetTickCount64();
	ss << ".download";
	return ss.str();
}

bool HttpFileDownload::createDirs(const std::string & dir)
{
	std::string strDir(dir);//存放要创建的目录字符串

	 //确保以'\'结尾以创建最后一个目录
	char c = strDir[strDir.length() - 1];
	if (c != '\\' && c != '/')
	{
		strDir += '\\';
	}
	std::vector<std::string> vPath;//存放每一层目录字符串
	std::string strTemp;//一个临时变量,存放目录字符串
	bool bSuccess = false;//成功标志
	//遍历要创建的字符串
	for (int i = 0; i < strDir.length(); ++i)
	{
		c = strDir[i];
		if (c != '\\' && c != '/')
		{//如果当前字符不是'\\'
			strTemp += strDir[i];
		}
		else
		{//如果当前字符是'\\'
			vPath.push_back(strTemp);//将当前层的字符串添加到数组中
			strTemp += '\\';
		}
	}

	//遍历存放目录的数组,创建每层目录
	std::vector<std::string>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); ++vIter)
	{
		//如果CreateDirectory执行成功,返回true,否则返回false
		bSuccess = CreateDirectoryA(vIter->c_str(), NULL) ? true : false;
	}

	return bSuccess;
}

std::string HttpFileDownload::generateSaveFile(const std::string& dir/* = ""*/)
{
	std::string saveFileName;
	std::string url = GetUrl();

	//从url的path部分找文件名
	Url uri(url);
	std::string urlPath = uri.path();
	size_t idx = urlPath.find_last_of("/\\");
	if (idx != std::string::npos)
	{
		saveFileName = urlPath.substr(idx + 1);
	}

	//直接截取url最后的
	if (pystring::iscempty(saveFileName))
	{
		size_t idx = url.find_last_of("/\\");
		if (idx != std::string::npos)
		{
			saveFileName = url.substr(idx + 1);
		}
	}

	//生成时间戳文件名
	if (pystring::iscempty(saveFileName))
	{
		saveFileName = getTimesampFileName();
	}

	return os_path::normpath(os_path::join(dir, saveFileName));
}



bool HttpFileDownload::Do()
{
	bool notSureName = false;
	std::string saveFileName;

	//如果没有设置保存文件
	if (m_file.empty())
	{
		saveFileName = generateSaveFile("");
		notSureName = true;
	}	
	else
	{
		std::string dir;

		//只设置了目录
		DWORD dwAttr = ::GetFileAttributesA(m_file.c_str());
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{// 如果是目录
			dir = m_file;
			saveFileName = generateSaveFile(dir);
			//saveFileName = os_path::normpath(os_path::join(m_file, getTimesampFileName()));
			notSureName = true;
		}
		else
		{//设置了具体文件路径
			//再次判断是否是目录
			char c = m_file[pystring::length(m_file) - 1];
			if (c == '\\' || c == '/')
			{
				dir = m_file;
				saveFileName = generateSaveFile(dir);
				//saveFileName = os_path::normpath(os_path::join(m_file, getTimesampFileName()));
				notSureName = true;
			}
			else
			{
				dir = os_path::dirname(m_file);
				saveFileName = os_path::normpath(m_file);
			}
		}

		//确保目录存在
		if (dir.c_str()[0] != '\0')
			createDirs(dir);
	}

	if (!SetFile(saveFileName))
		return false;
	
	//执行下载
	bool ret = __super::Do();

	//从response中获得文件名
	if (notSureName && ret)
	{
		std::vector<std::string> ContentDisposition = this->GetResponseHeaders("Content-Disposition", false);
		if (ContentDisposition.size() == 0)
			ContentDisposition = this->GetResponseHeaders("Content-Disposition", true);
		if (ContentDisposition.size() > 0)
		{
			//获取文件名编码
			bool bIsUtf8 = false;
			for (size_t i = 0; i < ContentDisposition.size(); i++)
			{
				const std::string& line = pystring::upper(ContentDisposition[i]);
				if (line.find("*=\"UTF-8''") != std::string::npos || line.find("*=\"UTF8''") != std::string::npos)
				{
					bIsUtf8 = true;
					break;
				}
			}			
			if (!bIsUtf8)
			{
				std::vector<std::string> ContentType = this->GetResponseHeaders("Content-Type", false);
				if (ContentType.size() == 0)
					ContentType = this->GetResponseHeaders("Content-Type", true);
				if (ContentType.size() > 0)
				{
					for (size_t i = 0; i < ContentType.size(); i++)
					{
						const std::string& line = pystring::upper(ContentType[i]);
						if (line.find("UTF-8") != std::string::npos || line.find("UTF8") != std::string::npos)
						{
							bIsUtf8 = true;
							break;
						}
					}
				}
			}

			//获取文件名
			std::string fileName;
			for (size_t i = 0; i < ContentDisposition.size(); i++)
			{
				const std::string& line = ContentDisposition[i];
				std::string lineLower = pystring::lower(line);
				size_t idx = lineLower.find("filename=");
				if (idx != std::string::npos)
				{
					fileName = pystring::strip(line.substr(idx + 9), "\"");
					break;
				}
				else if ((idx = lineLower.find("filename*=")) != std::string::npos)
				{
					fileName = pystring::strip(line.substr(idx + 10), "\"");
					idx = fileName.find("''");
					if (idx != std::string::npos)
					{
						fileName = fileName.substr(idx + 2);
						break;
					}
				}
			}

			//执行重命名
			if (!fileName.empty())
			{
				if (bIsUtf8)
					fileName = GL::Utf82Ansi(fileName.c_str());
				std::string targetFileName = os_path::dirname(saveFileName);
				if (!targetFileName.empty())
					targetFileName = os_path::join(targetFileName, fileName);
				else
					targetFileName = fileName;
				//执行重命名
				if (::MoveFileA(saveFileName.c_str(), targetFileName.c_str()) == TRUE)
					m_file = targetFileName;
			}

		}
	}

	return ret;
}
