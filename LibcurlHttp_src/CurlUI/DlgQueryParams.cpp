// DlgQueryParams.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgQueryParams.h"
#include "ItemKeyTextValue.h"

// DlgQueryParams 对话框

IMPLEMENT_DYNAMIC(DlgQueryParams, CDialogEx)

DlgQueryParams::DlgQueryParams(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_QUERYPARAMS, pParent)
{

}

DlgQueryParams::~DlgQueryParams()
{
}

void DlgQueryParams::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_QUERYS, m_queryParams);
}

BOOL DlgQueryParams::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	m_scale.Init(GetSafeHwnd());

	int colIdx = m_queryParams.InsertColumn(0, _T(""));
	
	return res;
}


BEGIN_MESSAGE_MAP(DlgQueryParams, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD_QUERY_PARAM, &DlgQueryParams::OnBnClickedBtnAddQueryParam)
END_MESSAGE_MAP()


// DlgQueryParams 消息处理程序


void DlgQueryParams::OnBnClickedBtnAddQueryParam()
{
	int nRow = m_queryParams.InsertItem(m_queryParams.GetItemCount(), _T(""));
	CItemKeyTextValue* pItemKV = new CItemKeyTextValue();
	pItemKV->m_rowIdx = nRow;
	pItemKV->Create(CItemKeyTextValue::IDD, &m_queryParams);
	CRect rc;
	pItemKV->GetWindowRect(rc);
	m_queryParams.SetColumnWidth(0, rc.Width());
	m_queryParams.SetRowHeight(rc.Height());
	m_queryParams.SetItemEx(nRow, 0, pItemKV);
}
