// DlgBody_x_www_form_urlencoded.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgBody_x_www_form_urlencoded.h"
#include "ItemKeyTextValue.h"


// CDlgBody_x_www_form_urlencoded 对话框

IMPLEMENT_DYNAMIC(CDlgBody_x_www_form_urlencoded, CDialogEx)

CDlgBody_x_www_form_urlencoded::CDlgBody_x_www_form_urlencoded(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_BODY_X_WWW_FORM_URLENCODED, pParent)
{

}

CDlgBody_x_www_form_urlencoded::~CDlgBody_x_www_form_urlencoded()
{
}

void CDlgBody_x_www_form_urlencoded::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_BODY_DATA, m_bodyData);
}

BOOL CDlgBody_x_www_form_urlencoded::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	m_scale.Init(GetSafeHwnd());

	int colIdx = m_bodyData.InsertColumn(0, _T(""));

	return res;
}


BEGIN_MESSAGE_MAP(CDlgBody_x_www_form_urlencoded, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD, &CDlgBody_x_www_form_urlencoded::OnBnClickedBtnAdd)
END_MESSAGE_MAP()


// CDlgBody_x_www_form_urlencoded 消息处理程序


void CDlgBody_x_www_form_urlencoded::OnBnClickedBtnAdd()
{
	int nRow = m_bodyData.InsertItem(m_bodyData.GetItemCount(), _T(""));
	CItemKeyTextValue* pItemKV = new CItemKeyTextValue();
	pItemKV->Create(CItemKeyTextValue::IDD, &m_bodyData);
	CRect rc;
	pItemKV->GetWindowRect(rc);
	m_bodyData.SetColumnWidth(0, rc.Width());
	m_bodyData.SetRowHeight(rc.Height());
	m_bodyData.SetItemEx(nRow, 0, pItemKV);
}
