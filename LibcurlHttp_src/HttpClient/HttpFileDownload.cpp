#include "HttpFileDownload.h"
#include "pystring.h"
#include "../HttpHelper/Convertor.h"
#include "../HttpHelper/UrlCoding.h"
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
	std::stringstream ss;
	ss << std::this_thread::get_id();
	ss << "-";
	ss << ::GetTickCount64();
	ss << ".tmp";
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
		std::string dir;

		//只设置了目录
		DWORD dwAttr = ::GetFileAttributesA(m_file.c_str());
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{// 如果是目录
			dir = m_file;
			saveFileName = pystring::os::path::normpath(pystring::os::path::join(m_file, getTimesampFileName()));
			needRename = true;
		}
		else
		{//设置了具体文件路径
			//再次判断是否是目录
			char c = m_file[pystring::length(m_file) - 1];
			if (c == '\\' || c == '/')
			{
				dir = m_file;
				saveFileName = pystring::os::path::normpath(pystring::os::path::join(m_file, getTimesampFileName()));
				needRename = true;
			}
			else
			{
				dir = pystring::os::path::dirname(m_file);
				saveFileName = pystring::os::path::normpath(m_file);
			}
		}

		//确保目录存在
		if (dir.c_str()[0] != '\0')
			createDirs(dir);
	}

	if (!SetFileName(saveFileName))
		return false;
	
	//执行下载
	bool ret = __super::Do();

	//从response中获得文件名
	if (needRename && ret)
	{
		auto ContentDisposition = this->GetResponseHeaders().find("Content-Disposition");
		if (ContentDisposition != this->GetResponseHeaders().end())
		{
			//获取文件名编码
			bool bIsUtf8 = false;
			for (size_t i = 0; i < ContentDisposition->second.size(); i++)
			{
				const std::string& line = pystring::upper(ContentDisposition->second[i]);
				if (line.find("*=\"UTF-8''") != std::string::npos || line.find("*=\"UTF8''") != std::string::npos)
				{
					bIsUtf8 = true;
					break;
				}
			}			
			if (!bIsUtf8)
			{
				auto ContentType = this->GetResponseHeaders().find("Content-Type");
				if (ContentType != this->GetResponseHeaders().end())
				{
					for (size_t i = 0; i < ContentType->second.size(); i++)
					{
						const std::string& line = pystring::upper(ContentType->second[i]);
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
			for (size_t i = 0; i < ContentDisposition->second.size(); i++)
			{
				const std::string& line = ContentDisposition->second[i];
				size_t idx = line.find("filename=");
				if (idx != std::string::npos)
				{
					fileName = pystring::strip(line.substr(idx + 9), "\"");
					break;
				}
				else if ((idx = line.find("filename*=")) != std::string::npos)
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
				std::string targetFileName = pystring::os::path::dirname(saveFileName);
				if (!targetFileName.empty())
					targetFileName = pystring::os::path::join(targetFileName, fileName);
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
