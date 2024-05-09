
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

	for (size_t i = 0; i < m_vctPages.size(); i++)
	{
		delete m_vctPages[i];
	}
	m_vctPages.clear();
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
}

BEGIN_MESSAGE_MAP(CCurlUIDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_RUN, &CCurlUIDlg::OnBnClickedBtnRun)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CCurlUIDlg::OnTcnSelchangeTab1)
	ON_EN_CHANGE(IDC_EDIT_URL, &CCurlUIDlg::OnEnChangeEditUrl)
	ON_NOTIFY(NM_CLICK, IDC_btnSetting, &CCurlUIDlg::OnNMClickbtnsetting)
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

	m_vctPages[0]->ShowWindow(SW_SHOW);
	m_scaleTab.Init(m_tabParams.GetSafeHwnd());

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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CCurlUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

CString CCurlUIDlg::generateUrlByQueryParams(const CString& srcUrl)
{
	DlgQueryParams* dlg = dynamic_cast<DlgQueryParams*>(m_vctPages[0]);
	if (!dlg)
		return CString();

	auto kvs = dlg->GetQueryParams();

	std::string sUrl = CW2A(srcUrl.GetString());
	Url url(sUrl);
	url.set_query().clear();
		
	for (size_t i = 0; i < kvs.size(); i++)
	{
		CString k = kvs[i].first;
		if (k.IsEmpty())
			continue;
		CString v = kvs[i].second;

		std::string sKey = UrlCoding::UrlUTF8Encode(GL::WideByte2Ansi(k.GetString()).c_str());
		std::string sValue = UrlCoding::UrlUTF8Encode(GL::WideByte2Ansi(v.GetString()).c_str());
		url.add_query(sKey.c_str(), sValue.c_str());
	}

	return CA2W(url.str().c_str());
}

void CCurlUIDlg::refreshQueryParams()
{
	DlgQueryParams* dlg = dynamic_cast<DlgQueryParams*>(m_vctPages[0]);
	if (!dlg)
		return;
	dlg->Clear();

	CString csUrl;
	m_editUrl.GetWindowText(csUrl);
	if (csUrl.IsEmpty())
		return;

	std::string sUrl = CW2A(csUrl.GetString());
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

std::vector<std::pair<CString, CString>> CCurlUIDlg::getHeaders()
{
	std::vector<std::pair<CString, CString>> ret;

	DlgHeader* dlg = dynamic_cast<DlgHeader*>(m_vctPages[1]);
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

int CCurlUIDlg::_PROGRESS_CALLBACK(
	double downloadTotal, double downloadNow,
	double uploadTotal, double uploadNow,
	void* userData)
{
	CCurlUIDlg* This = (CCurlUIDlg*)userData;
	if (uploadTotal != 0)
	{
		int pos = uploadNow / uploadTotal * 100;
		This->m_progress.SetPos(pos);
	}

	return 0;
}

void CCurlUIDlg::dumpResponse(LibcurlHttp* http)
{
	std::string headers;
	int headerCount = http->getResponseHeaderKeysCount();
	for (int i = 0; i < headerCount; i++)
	{
		const char* key = http->getResponseHeaderKey(i);
		if (!key)
			continue;

		std::string value;
		int valueCount = http->getResponseHeadersCount(key);
		for (int j = 0; j < valueCount; j++)
		{
			const char* pval = http->getResponseHeader(key, j);
			if (!pval)
				continue;

			if (!value.empty())
				value += ';';
			value += pval;
		}

		headers += key + std::string(":") + value + "\r\n";
	}

	m_editResponseHeader.SetWindowText(CA2W(headers.c_str()));
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

	sUrl = generateUrlByQueryParams(sUrl);
	auto headers = getHeaders();

	LibcurlHttp* http = HTTP_CLIENT::Ins().CreateHttp();

	http->setProgress(_PROGRESS_CALLBACK, this);
	http->setAutoRedirect(false);

	for (size_t i = 0; i < headers.size(); i++)
	{
		http->setRequestHeader(CW2A(headers[i].first), CW2A(headers[i].second));
	}

	int curTab = m_tabParams.GetCurSel();
	switch (curTab)
	{
	case 0:
	{

		break;
	}
	case 1:
	{

		break;
	}
	case 2:
	{
		CDlgBodyMultipart* pDlg = (CDlgBodyMultipart*)m_vctPages[curTab];
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

		break;
	}
	case 4:
	{

		break;
	}
	case 5:
	{

		break;
	}
	default:
		break;
	}

	dumpResponse(http);

	//http->post(http->WidebyteToAnsi(sUrl.GetString()), NULL, 0);
	//int len = 0;
	//const char* pBody = http->getBody(len);
	//if (len > 0)
	//{
	//	std::wstring wBody = http->UTF8ToWidebyte(pBody);
	//	m_editResponseBody.SetWindowText(wBody.c_str());
	//}
	HTTP_CLIENT::Ins().ReleaseHttp(http);
}


void CCurlUIDlg::OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;

	int sel = m_tabParams.GetCurSel();
	if (sel < 0 || sel >= m_vctPages.size())
		return;

	for (size_t i = 0; i < m_vctPages.size(); i++)
	{
		m_vctPages[i]->ShowWindow(SW_HIDE);
	}

	m_vctPages[sel]->ShowWindow(SW_SHOW);
}


void CCurlUIDlg::OnEnChangeEditUrl()
{
	refreshQueryParams();
	m_editUrl.SetFocus();
}


void CCurlUIDlg::OnNMClickbtnsetting(NMHDR* pNMHDR, LRESULT* pResult)
{
	

	*pResult = 0;
}
