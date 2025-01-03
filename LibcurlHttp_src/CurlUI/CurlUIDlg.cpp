﻿
// CurlUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CurlUI.h"
#include "CurlUIDlg.h"
#include "afxdialogex.h"
#include "DlgQueryParams.h"
#include "DlgHeader.h"
#include "DlgBodyMultipart.h"
#include "DlgBody_x_www_form_urlencoded.h"
#include "DlgBodyRaw.h"
#include "DlgBodyFile.h"

#include "../HttpHelper/url.hpp"
#include "../pystring/pystring.h"
#include "../HttpHelper/Convertor.h"
#include "../HttpHelper/UrlCoding.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CCurlUIDlg 对话框



CCurlUIDlg::CCurlUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CURLUI_DIALOG, pParent)
	, m_running(true)
	, m_isResponseBodyUtf8(true)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CCurlUIDlg::~CCurlUIDlg()
{
	for (size_t i = 0; i < m_pageScales.size(); i++)
	{
		delete m_pageScales[i];
	}
	m_pageScales.clear();

	for (size_t i = 0; i < m_pages.size(); i++)
	{
		delete m_pages[i];
	}
	m_pages.clear();
}

void CCurlUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CMB_METHOD, m_cmbMethod);
	DDX_Control(pDX, IDC_EDIT_URL, m_editUrl);
	DDX_Control(pDX, IDC_EDIT_RESBODY, m_editResponseBody);
	DDX_Control(pDX, IDC_EDIT_RESHEADER, m_editResponseHeader);
	DDX_Control(pDX, IDC_PROG_PROGRESS, m_progress);
	DDX_Control(pDX, IDC_TAB1, m_tabParams);
	DDX_Control(pDX, IDC_CHK_SAVE_TO_FILE, m_chkSaveBodyToFile);
}

BEGIN_MESSAGE_MAP(CCurlUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_RUN, &CCurlUIDlg::OnBnClickedBtnRun)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CCurlUIDlg::OnTcnSelchangeTab1)
	ON_EN_CHANGE(IDC_EDIT_URL, &CCurlUIDlg::OnEnChangeEditUrl)
	ON_NOTIFY(NM_CLICK, IDC_btnSetting, &CCurlUIDlg::OnNMClickbtnsetting)
	ON_BN_CLICKED(IDC_CHK_SAVE_TO_FILE, &CCurlUIDlg::OnBnClickedChkSaveToFile)
END_MESSAGE_MAP()


// CCurlUIDlg 消息处理程序

BOOL CCurlUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_scale.SetAnchor(GetDlgItem(IDC_CMB_METHOD)->GetSafeHwnd(),
		CCtrlScale::AnchorLeftToWinLeft |
		CCtrlScale::AnchorRightToWinLeft |
		CCtrlScale::AnchorTopToWinTop |
		CCtrlScale::AnchorBottomToWinTop);
	m_scale.SetAnchor(GetDlgItem(IDC_EDIT_URL)->GetSafeHwnd(),
		CCtrlScale::AnchorLeftToWinLeft |
		CCtrlScale::AnchorRightToWinRight |
		CCtrlScale::AnchorTopToWinTop |
		CCtrlScale::AnchorBottomToWinTop);
	m_scale.SetAnchor(GetDlgItem(IDC_BTN_RUN)->GetSafeHwnd(),
		CCtrlScale::AnchorLeftToWinRight |
		CCtrlScale::AnchorRightToWinRight |
		CCtrlScale::AnchorTopToWinTop |
		CCtrlScale::AnchorBottomToWinTop);
	m_scale.SetAnchor(GetDlgItem(IDC_TAB1)->GetSafeHwnd(),
		CCtrlScale::AnchorLeftToWinLeft |
		CCtrlScale::AnchorRightToWinRight |
		CCtrlScale::AnchorTopToWinTop);
	m_scale.SetAnchor(GetDlgItem(IDC_EDIT_RESHEADER)->GetSafeHwnd(),
		CCtrlScale::AnchorLeftToWinLeft |
		CCtrlScale::AnchorBottomToWinBottom);
	m_scale.SetAnchor(GetDlgItem(IDC_EDIT_RESBODY)->GetSafeHwnd(),
		CCtrlScale::AnchorRightToWinRight |
		CCtrlScale::AnchorBottomToWinBottom);
	m_scale.SetAnchor(GetDlgItem(IDC_PROG_PROGRESS)->GetSafeHwnd(),
		CCtrlScale::AnchorRightToWinRight |
		CCtrlScale::AnchorTopToWinBottom |
		CCtrlScale::AnchorBottomToWinBottom);
	m_scale.Init(GetSafeHwnd());

	// TODO: 在此添加额外的初始化代码
	m_cmbMethod.AddString(_T("GET"));
	m_cmbMethod.AddString(_T("POST"));
	m_cmbMethod.AddString(_T("PUT"));
	m_cmbMethod.AddString(_T("DELETE"));
	m_cmbMethod.AddString(_T("HEAD"));
	m_cmbMethod.AddString(_T("OPTIONS"));

	addPage<DlgQueryParams>(_T("Query参数"));
	addPage<DlgHeader>(_T("Header"));
	addPage<CDlgBodyMultipart>(_T("*Body-Multipart"));
	addPage<CDlgBody_x_www_form_urlencoded>(_T("*Body-x-www-form-encoded"));
	addPage<CDlgBodyRaw>(_T("*Body-Raw"));
	addPage<CDlgBodyFile>(_T("*Body-File"));

	m_pages[0]->ShowWindow(SW_SHOW);
	m_scaleTab.Init(m_tabParams.GetSafeHwnd());

	m_cmbMethod.SetCurSel(0);
	m_progress.SetRange(0, 100);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCurlUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

void CCurlUIDlg::OnOK()
{
}

void CCurlUIDlg::OnCancel()
{
	m_running = false;
	__super::OnCancel();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCurlUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CCurlUIDlg::generateUrlByQueryParams(const CString& srcUrl)
{
	DlgQueryParams* dlg = dynamic_cast<DlgQueryParams*>(m_pages[0]);
	if (!dlg)
		return CString();

	auto kvs = dlg->GetQueryParams();

	std::string sUrl = CW2A(srcUrl.GetString());

	try
	{
		Url url(sUrl);
		url.set_query().clear();

		for (size_t i = 0; i < kvs.size(); i++)
		{
			CString k = kvs[i].first;
			if (k.IsEmpty())
				continue;
			CString v = kvs[i].second;

			std::string s;
			GL::Unicode2Ansi(s, k.GetString());
			std::string sKey = UrlCoding::UrlUTF8Encode(s.c_str());
			GL::Unicode2Ansi(s, v.GetString());
			std::string sValue = UrlCoding::UrlUTF8Encode(s.c_str());
			url.add_query(sKey.c_str(), sValue.c_str());
		}

		sUrl = url.str();
	}
	catch (const std::exception&)
	{

	}
	
	return CA2W(sUrl.c_str());
}

void CCurlUIDlg::refreshQueryParams()
{
	DlgQueryParams* dlg = dynamic_cast<DlgQueryParams*>(m_pages[0]);
	if (!dlg)
		return;
	dlg->Clear();

	CString csUrl;
	m_editUrl.GetWindowText(csUrl);
	if (csUrl.IsEmpty())
		return;

	std::string sUrl = CW2A(csUrl.GetString());
	try
	{
		Url url(sUrl);
		auto query = url.query();
		for (size_t i = 0; i < query.size(); i++)
		{
			std::string k = query[i].key();
			std::string v = query[i].val();

			CString key, val;
			key = CA2W(UrlCoding::UrlUTF8Decode(k.c_str()).c_str());
			val = CA2W(UrlCoding::UrlUTF8Decode(v.c_str()).c_str());

			dlg->AddQueryParam(key, val);
		}
	}
	catch (const std::exception&)
	{

	}
}

std::vector<std::pair<CString, CString>> CCurlUIDlg::getHeaders()
{
	std::vector<std::pair<CString, CString>> ret;

	DlgHeader* dlg = dynamic_cast<DlgHeader*>(m_pages[1]);
	if (!dlg)
		return ret;

	auto kvs = dlg->GetHeaders();
	for (size_t i = 0; i < kvs.size(); i++)
	{
		if (kvs[i].first.IsEmpty())
			continue;
		ret.push_back(std::make_pair(kvs[i].first, kvs[i].second));
	}

	return ret;
}

void CCurlUIDlg::fillDefaultSetting(LibcurlHttp* http)
{
	GlobalSetting& setting = m_setting.GetSetting();

	if (setting.timeout > -1)
		http->setTimeout(setting.timeout);
	
	if (!setting.userAgent.IsEmpty())
		http->setUserAgent(CW2A(setting.userAgent));
	
	http->setAutoRedirect(setting.autoRedirect);
	if (setting.autoRedirect)
		http->setMaxRedirect(setting.autoRedirectMaxCount);

	http->setResponseBodyAutoDecode(setting.autoDecodeResponse);
}

bool CCurlUIDlg::_PROGRESS_CALLBACK(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow,
	void* userData)
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		::DispatchMessage(&msg);
		::TranslateMessage(&msg);
	}
		
	CCurlUIDlg* This = (CCurlUIDlg*)userData;

	if (!This->m_running)
		return false;

	if (uploadTotal != 0)
	{
		int pos = uploadNow / uploadTotal * 100;
		This->m_progress.SetPos(pos);
	}

	if (downloadTotal != 0)
	{
		int pos = downloadNow / downloadTotal * 100;
		This->m_progress.SetPos(pos);
	}

	return true;
}

bool CCurlUIDlg::_HEADER_CALLBACK(const char* header, void* userData)
{
	CCurlUIDlg* This = (CCurlUIDlg*)userData;
	
	CString sText;
	This->m_editResponseHeader.GetWindowText(sText);
	sText += CA2W(header);
	This->m_editResponseHeader.SetWindowText(sText);

	std::string sHeader = pystring::lower(header);
	if (pystring::startswith(sHeader, "content-type:"))
	{
		if (sHeader.find("utf-8") != std::string::npos ||
			sHeader.find("utf8") != std::string::npos)
		{
			This->m_isResponseBodyUtf8 = true;
		}
		else
		{
			This->m_isResponseBodyUtf8 = false;
		}
	}

	//std::string headerKV = header;
	//size_t seperator = headerKV.find_first_of(':');
	//if (std::string::npos != seperator)
	//{
	//	std::string key = headerKV.substr(0, seperator);
	//	key = pystring::strip(key);
	//	if (pystring::equal(key, "Content-Length"))
	//	{
	//		std::string value = headerKV.substr(seperator + 1);
	//		value = pystring::strip(value);
	//		auto length = atoll(value.c_str());
	//		if (length > 1024000)
	//			g_isBodyTooLarge = true;
	//		else
	//			g_isBodyTooLarge = false;
	//	}
	//}
	return true;
}

bool CCurlUIDlg::_WRITED_CALLBACK(void* pBuffer, size_t nSize, size_t nMemByte, void* userData)
{
	CCurlUIDlg* This = (CCurlUIDlg*)userData;

	if (This->isFileSaveBodyValid())
	{
		This->m_fileSaveBody.Write(pBuffer, nSize * nMemByte);
	}
	else
	{
		CString sText;
		This->m_editResponseBody.GetWindowText(sText);
		int maxLen = This->m_setting.GetSetting().showBodyMaxLength;
		if (maxLen > 0 && sText.GetLength() > maxLen)
			return true;

		std::wstring ws;
		if (This->m_isResponseBodyUtf8)
			GL::Utf82Unicode(ws, std::string((const char*)pBuffer, nSize * nMemByte));
		else
			GL::Ansi2Unicode(ws, std::string((const char*)pBuffer, nSize * nMemByte));
		sText += ws.c_str();
		This->m_editResponseBody.SetWindowText(sText);
	}

	return true;
}

void CCurlUIDlg::dumpResponse(LibcurlHttp* http)
{
	//std::string headers;
	//int headerCount = http->getResponseHeaderKeysCount();
	//for (int i = 0; i < headerCount; i++)
	//{
	//	const char* key = http->getResponseHeaderKey(i);
	//	if (!key)
	//		continue;

	//	std::string value;
	//	int valueCount = http->getResponseHeadersCount(key);
	//	for (int j = 0; j < valueCount; j++)
	//	{
	//		const char* pval = http->getResponseHeader(key, j);
	//		if (!pval)
	//			continue;

	//		if (!value.empty())
	//			value += ';';
	//		value += pval;
	//	}

	//	if (pystring::startswith(key, "HTTP/"))
	//		headers = key + std::string("\r\n") + headers;
	//	else
	//		headers += key + std::string(":") + value + "\r\n";
	//}
	//m_editResponseHeader.SetWindowText(CA2W(headers.c_str()));

	//size_t len = 0;
	//const char* pBody = http->getBody(len);
	//int showBodyMaxLength = m_setting.GetSetting().showBodyMaxLength;
	//bool bOver = false;
	//if (showBodyMaxLength >0 && len > showBodyMaxLength)
	//{
	//	len = m_setting.GetSetting().showBodyMaxLength;
	//	bOver = true;
	//}
	//const wchar_t* wBody = http->UTF8ToUnicode(pBody, len);
	//CString sBody(wBody, len);
	//if (bOver)
	//	sBody += L"...<内容超出>...";
	//m_editResponseBody.SetWindowTextW(sBody);
}

void CCurlUIDlg::OnBnClickedBtnRun()
{
	m_progress.SetPos(0);

	CString sMethod;
	m_cmbMethod.GetWindowText(sMethod);
	if (sMethod.IsEmpty())
		return;
	CString sUrl;
	m_editUrl.GetWindowText(sUrl);
	if (sUrl.IsEmpty())
		return;
		
	if (m_chkSaveBodyToFile.GetCheck() && !m_saveBodyFilePath.IsEmpty())
	{
		if (isFileSaveBodyValid())
			m_fileSaveBody.Close();
		if (!m_fileSaveBody.Open(m_saveBodyFilePath,
			CFile::OpenFlags::modeCreate |
			CFile::OpenFlags::modeWrite |
			CFile::OpenFlags::typeBinary))
		{
			return;
		}
	}

	sUrl = generateUrlByQueryParams(sUrl);
	auto headers = getHeaders();
	
	m_editResponseHeader.SetWindowText(_T(""));
	m_editResponseBody.SetWindowText(_T(""));

	LibcurlHttp* http = HTTP_CLIENT::Ins().CreateHttp();

	fillDefaultSetting(http);
	http->setCustomMethod(CW2A(sMethod));
	http->setProgress(_PROGRESS_CALLBACK, this);
	http->setResponseHeaderCallback(_HEADER_CALLBACK, this, false);
	http->setResponseBodyCallback(_WRITED_CALLBACK, this, false);
	
	for (size_t i = 0; i < headers.size(); i++)
	{
		http->setRequestHeader(CW2A(headers[i].first), CW2A(headers[i].second));
	}

	int curTab = m_tabParams.GetCurSel();
	switch (curTab)
	{
	case 0:
	{
		http->get(CW2A(sUrl));
		break;
	}
	case 1:
	{
		http->get(CW2A(sUrl));
		break;
	}
	case 2:
	{
		CDlgBodyMultipart* pDlg = (CDlgBodyMultipart*)m_pages[curTab];
		std::vector<std::pair<CString, std::pair<CString, bool>>> values = pDlg->GetValues();
		std::vector<MultipartField*> multiparts;
		for (size_t i = 0; i < values.size(); i++)
		{
			CString key = values[i].first;
			CString val = values[i].second.first;

			std::string name = CW2A(key);
			std::string content = CW2A(val);

			MultipartField* part = new MultipartField;
			if (values[i].second.second)
			{
				part->Fill(NULL, 0, content.c_str(), NULL, name.c_str(), NULL);
			}
			else
			{
				part->Fill(content.c_str(), content.size(), NULL, NULL, name.c_str(), NULL);
			}
			multiparts.push_back(part);
		}
		http->postMultipart(CW2A(sUrl), multiparts.data(), values.size());

		for (size_t i = 0; i < multiparts.size(); i++)
		{
			delete multiparts[i];
		}
		break;
	}
	case 3:
	{
		CDlgBody_x_www_form_urlencoded* pDlg = (CDlgBody_x_www_form_urlencoded*)m_pages[curTab];
		std::string content = pDlg->GetTextValue();
		http->post(CW2A(sUrl), content.c_str(), content.size());
		break;
	}
	case 4:
	{
		CDlgBodyRaw* pDlg = (CDlgBodyRaw*)m_pages[curTab];
		CString bodyData = pDlg->GetData();
		std::string sBodyData = CW2A(bodyData);
		if (sMethod.CompareNoCase(_T("PUT")) == 0)
		{
			http->putData(CW2A(sUrl), sBodyData.c_str(), sBodyData.size());
		}
		else
		{
			http->post(CW2A(sUrl), sBodyData.c_str(), sBodyData.size());
		}
		break;
	}
	case 5:
	{
		CDlgBodyFile* pDlg = (CDlgBodyFile*)m_pages[curTab];
		std::string filepath = CW2A(pDlg->GetFilePath());
		http->putFile(CW2A(sUrl), filepath.c_str());
		break;
	}
	default:
		break;
	}

	dumpResponse(http);

	if (isFileSaveBodyValid())
	{
		m_editResponseBody.SetWindowText(_T("结果已保存到:") + m_saveBodyFilePath);
		m_fileSaveBody.Close();
	}

	HTTP_CLIENT::Ins().ReleaseHttp(http);
}


bool CCurlUIDlg::isFileSaveBodyValid()
{
	return m_fileSaveBody.m_hFile && m_fileSaveBody.m_hFile != INVALID_HANDLE_VALUE;
}

void CCurlUIDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	int sel = m_tabParams.GetCurSel();
	if (sel < 0 || sel >= m_pages.size())
		return;

	for (size_t i = 0; i < m_pages.size(); i++)
	{
		m_pages[i]->ShowWindow(SW_HIDE);
	}

	m_pages[sel]->ShowWindow(SW_SHOW);
}


void CCurlUIDlg::OnEnChangeEditUrl()
{
	refreshQueryParams();
	m_editUrl.SetFocus();
}


void CCurlUIDlg::OnNMClickbtnsetting(NMHDR* pNMHDR, LRESULT* pResult)
{
	m_setting.DoModal();
	*pResult = 0;
}


void CCurlUIDlg::OnBnClickedChkSaveToFile()
{
	if (!m_chkSaveBodyToFile.GetCheck())
	{
		m_saveBodyFilePath.Empty();
		m_editResponseBody.SetWindowText(_T(""));
		return;
	}

	OPENFILENAME ofn = { 0 };
	TCHAR lpstrFilename[MAX_PATH * 20] = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	ofn.lpstrFile = lpstrFilename;
	ofn.nMaxFile = MAX_PATH * 20;
	//ofn.lpstrInitialDir = TEXT("C:\\"); //初始化一个路径
	ofn.Flags = OFN_HIDEREADONLY | OFN_FORCESHOWHIDDEN | OFN_PATHMUSTEXIST | OFN_EXPLORER/* | OFN_ALLOWMULTISELECT*/;

	if (!GetOpenFileName(&ofn))
	{
		m_chkSaveBodyToFile.SetCheck(FALSE);
		return;
	}

	TCHAR szDir[MAX_PATH] = { 0 };
	TCHAR szName[128] = { 0 };
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR* pos = lpstrFilename;

	if (0 == lpstrFilename[_tcslen(lpstrFilename) + 1])// 只有一个文件
	{
		_tcscpy_s(szPath, MAX_PATH, lpstrFilename);
	}
	else
	{
		_tcscpy_s(szDir, MAX_PATH, lpstrFilename);
		_tcscat_s(szDir, MAX_PATH, _T("\\"));

		do
		{
			pos = &pos[_tcslen(pos)] + 1;
			if (0 == *pos)
				break;

			_tcscpy_s(szPath, MAX_PATH, szDir);
			_tcscat_s(szPath, MAX_PATH, pos);

		} while (1);
	}

	m_saveBodyFilePath = szPath;
	m_editResponseBody.SetWindowText(m_saveBodyFilePath);
}
