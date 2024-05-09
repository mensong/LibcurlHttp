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
	//	//֪ͨ�����������ϴ�����
	//	::PostMessage(hDlgWnd, WM_USER + 110, nPos, 0);
	//}
	//if (downloadTotal != 0)
	//{
	//	int nPos = (int)((downloadNow / downloadTotal) * 100);
	//	//֪ͨ�������������ؽ���
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
	std::string strDir(dir);//���Ҫ������Ŀ¼�ַ���

	 //ȷ����'\'��β�Դ������һ��Ŀ¼
	char c = strDir[strDir.length() - 1];
	if (c != '\\' && c != '/')
	{
		strDir += '\\';
	}
	std::vector<std::string> vPath;//���ÿһ��Ŀ¼�ַ���
	std::string strTemp;//һ����ʱ����,���Ŀ¼�ַ���
	bool bSuccess = false;//�ɹ���־
	//����Ҫ�������ַ���
	for (int i = 0; i < strDir.length(); ++i)
	{
		c = strDir[i];
		if (c != '\\' && c != '/')
		{//�����ǰ�ַ�����'\\'
			strTemp += strDir[i];
		}
		else
		{//�����ǰ�ַ���'\\'
			vPath.push_back(strTemp);//����ǰ����ַ�����ӵ�������
			strTemp += '\\';
		}
	}

	//�������Ŀ¼������,����ÿ��Ŀ¼
	std::vector<std::string>::const_iterator vIter;
	for (vIter = vPath.begin(); vIter != vPath.end(); ++vIter)
	{
		//���CreateDirectoryִ�гɹ�,����true,���򷵻�false
		bSuccess = CreateDirectoryA(vIter->c_str(), NULL) ? true : false;
	}

	return bSuccess;
}

std::string HttpFileDownload::generateSaveFile(const std::string& dir/* = ""*/)
{
	std::string saveFileName;
	std::string url = GetUrl();

	//��url��path�������ļ���
	Url uri(url);
	std::string urlPath = uri.path();
	size_t idx = urlPath.find_last_of("/\\");
	if (idx != std::string::npos)
	{
		saveFileName = urlPath.substr(idx + 1);
	}

	//ֱ�ӽ�ȡurl����
	if (pystring::iscempty(saveFileName))
	{
		size_t idx = url.find_last_of("/\\");
		if (idx != std::string::npos)
		{
			saveFileName = url.substr(idx + 1);
		}
	}

	//����ʱ����ļ���
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

	//���û�����ñ����ļ�
	if (m_file.empty())
	{
		saveFileName = generateSaveFile("");
		notSureName = true;
	}	
	else
	{
		std::string dir;

		//ֻ������Ŀ¼
		DWORD dwAttr = ::GetFileAttributesA(m_file.c_str());
		if (dwAttr & FILE_ATTRIBUTE_DIRECTORY)
		{// �����Ŀ¼
			dir = m_file;
			saveFileName = generateSaveFile(dir);
			//saveFileName = os_path::normpath(os_path::join(m_file, getTimesampFileName()));
			notSureName = true;
		}
		else
		{//�����˾����ļ�·��
			//�ٴ��ж��Ƿ���Ŀ¼
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

		//ȷ��Ŀ¼����
		if (dir.c_str()[0] != '\0')
			createDirs(dir);
	}

	if (!SetFile(saveFileName))
		return false;
	
	//ִ������
	bool ret = __super::Do();

	//��response�л���ļ���
	if (notSureName && ret)
	{
		std::vector<std::string> ContentDisposition = this->GetResponseHeaders("Content-Disposition", false);
		if (ContentDisposition.size() == 0)
			ContentDisposition = this->GetResponseHeaders("Content-Disposition", true);
		if (ContentDisposition.size() > 0)
		{
			//��ȡ�ļ�������
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

			//��ȡ�ļ���
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

			//ִ��������
			if (!fileName.empty())
			{
				if (bIsUtf8)
					fileName = GL::Utf82Ansi(fileName.c_str());
				std::string targetFileName = os_path::dirname(saveFileName);
				if (!targetFileName.empty())
					targetFileName = os_path::join(targetFileName, fileName);
				else
					targetFileName = fileName;
				//ִ��������
				if (::MoveFileA(saveFileName.c_str(), targetFileName.c_str()) == TRUE)
					m_file = targetFileName;
			}

		}
	}

	return ret;
}
