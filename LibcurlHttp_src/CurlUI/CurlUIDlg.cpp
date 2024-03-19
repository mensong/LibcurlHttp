
// CurlUIDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "CurlUI.h"
#include "CurlUIDlg.h"
#include "afxdialogex.h"
#include "DlgQueryParams.h"
#include "DlgHeader.h"
#include "DlgBody.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCurlUIDlg 对话框



CCurlUIDlg::CCurlUIDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CURLUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
END_MESSAGE_MAP()


// CCurlUIDlg 消息处理程序

BOOL CCurlUIDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_cmbMethod.AddString(_T("GET"));
	m_cmbMethod.AddString(_T("POST"));
	m_cmbMethod.AddString(_T("PUT"));
	m_cmbMethod.AddString(_T("DELETE"));
	m_cmbMethod.AddString(_T("HEAD"));
	m_cmbMethod.AddString(_T("OPTIONS"));

	//获取tab control位置和大小
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;
	m_tabParams.GetClientRect(&tabRect);
	m_tabParams.GetItemRect(0, &itemRect);
	nX = itemRect.left;
	nY = itemRect.bottom + 1;
	nXc = tabRect.right - itemRect.left - 2;
	nYc = tabRect.bottom - nY - 2;

	// 添加对话框1
	m_tabParams.InsertItem(0, _T("Query参数"));
	DlgQueryParams* dlg1 = new DlgQueryParams();
	m_vctPages.push_back(dlg1);
	dlg1->Create(DlgQueryParams::IDD, &m_tabParams);
	//设置对话框1的显示位置
	dlg1->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	dlg1->ShowWindow(SW_HIDE);

	//添加对话框2
	m_tabParams.InsertItem(1, _T("Header"));
	DlgHeader* dlg2 = new DlgHeader();
	m_vctPages.push_back(dlg2);
	dlg2->Create(DlgHeader::IDD, &m_tabParams);
	//设置对话框1的显示位置
	dlg2->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	dlg2->ShowWindow(SW_HIDE);

	//添加对话框3
	m_tabParams.InsertItem(2, _T("Body"));
	DlgBody* dlg3 = new DlgBody();
	m_vctPages.push_back(dlg3);
	dlg3->Create(DlgBody::IDD, &m_tabParams);
	//设置对话框1的显示位置
	dlg3->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	dlg3->ShowWindow(SW_HIDE);

	m_vctPages[0]->ShowWindow(SW_SHOW);

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



void CCurlUIDlg::OnBnClickedBtnRun()
{
	CString sMethod;
	m_cmbMethod.GetWindowText(sMethod);
	if (sMethod.IsEmpty())
		return;
	CString sUrl;
	m_editUrl.GetWindowText(sUrl);
	if (sUrl.IsEmpty())
		return;

	LibcurlHttp* http = HTTP_CLIENT::Ins().CreateHttp();
	http->post(http->WidebyteToAnsi(sUrl.GetString()), NULL, 0);
	int len = 0;
	const char* pBody = http->getBody(len);
	if (len > 0)
	{
		std::wstring wBody = http->UTF8ToWidebyte(pBody);
		m_editResponseBody.SetWindowText(wBody.c_str());
	}
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
