#pragma once
#include "..\HttpClient\HttpClient.h"

class HttpClientFC :
	public HttpClient
{
public:
	typedef int (*FN_PROGRESS_CALLBACK)(
		double downloadTotal, double downloadNow,
		double uploadTotal, double uploadNow,
		void* userData);

public:
	HttpClientFC();
	virtual ~HttpClientFC();

	void SetProgress(FN_PROGRESS_CALLBACK progressCallback, void* userData);

protected:
	virtual int OnProgress(
		double downloadTotal, double downloadNow,
		double uploadTotal, double uploadNow) override;
	FN_PROGRESS_CALLBACK m_progressCallback;
	void* m_userData;
};

