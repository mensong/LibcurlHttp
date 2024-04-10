// CDlgBodyMultipart.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgBodyMultipart.h"
#include "ItemKeyValueExt.h"


// CDlgBodyMultipart 对话框

IMPLEMENT_DYNAMIC(CDlgBodyMultipart, CDialogEx)

CDlgBodyMultipart::CDlgBodyMultipart(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD, pParent)
{

}

CDlgBodyMultipart::~CDlgBodyMultipart()
{
}

void CDlgBodyMultipart::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FROM_DATA, m_formData);
}

BOOL CDlgBodyMultipart::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	m_scale.Init(GetSafeHwnd());

	int colIdx = m_formData.InsertColumn(0, _T(""));

	return res;
}


BEGIN_MESSAGE_MAP(CDlgBodyMultipart, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD_QUERY_PARAM, &CDlgBodyMultipart::OnBnClickedBtnAddQueryParam)
END_MESSAGE_MAP()


// CDlgBodyMultipart 消息处理程序


void CDlgBodyMultipart::OnBnClickedBtnAddQueryParam()
{
	int nRow = m_formData.InsertItem(m_formData.GetItemCount(), _T(""));
	CItemKeyValueExt* pItemKV = new CItemKeyValueExt();
	pItemKV->m_rowIdx = nRow;
	pItemKV->Create(CItemKeyValueExt::IDD, &m_formData);
	CRect rc;
	pItemKV->GetWindowRect(rc);
	m_formData.SetColumnWidth(0, rc.Width());
	m_formData.SetRowHeight(rc.Height());
	m_formData.SetItemEx(nRow, 0, pItemKV);
}
