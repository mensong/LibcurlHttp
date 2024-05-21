#pragma once
#include "..\HttpClient\HttpClient.h"

class HttpClientFC :
	public HttpClient
{
public:
	typedef bool (*FN_HEADER_CALLBACK)(const char* header, void* userData);
	typedef bool (*FN_WRITED_CALLBACK)(void* pBuffer, size_t nSize, size_t nMemByte, void* userData);
	typedef bool (*FN_PROGRESS_CALLBACK)(double downloadTotal, double downloadNow, 
		double uploadTotal, double uploadNow, void* userData);

public:
	HttpClientFC();
	virtual ~HttpClientFC();

	void SetHeaderCallback(FN_HEADER_CALLBACK cb, void* userData);
	void SetBodyCallback(FN_WRITED_CALLBACK cb, void* userData);
	void SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData);

protected:
	virtual bool OnHeader(const char* header) override;
	virtual bool OnWrited(void* pBuffer, size_t nSize, size_t nMemByte) override;
	virtual bool OnProgress(double downloadTotal, double downloadNow, 
		double uploadTotal, double uploadNow) override;

private:
	FN_HEADER_CALLBACK m_headerCallback;
	void* m_userDataHeader;

	FN_WRITED_CALLBACK m_writedCallback;
	void* m_userDataWrited;

	FN_PROGRESS_CALLBACK m_progressCallback;
	void* m_userDataProgress;

};

