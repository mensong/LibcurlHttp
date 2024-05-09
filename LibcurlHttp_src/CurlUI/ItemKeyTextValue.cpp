// ItemKeyTextValue.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "ItemKeyTextValue.h"
#include "ListCtrlCustom.h"


// CItemKeyTextValue 对话框

IMPLEMENT_DYNAMIC(CItemKeyTextValue, CDialogEx)

CItemKeyTextValue::CItemKeyTextValue(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_KEY_TEXT_VALUE, pParent)
{

}

CItemKeyTextValue::~CItemKeyTextValue()
{
}

bool CItemKeyTextValue::IsEnable()
{
	return m_chkEnable.GetCheck() == TRUE;
}

std::pair<CString, CString> CItemKeyTextValue::GetKeyValue()
{
	CString key, value;
	m_editKey.GetWindowText(key);
	m_editValue.GetWindowText(value);
	return std::make_pair(key, value);
}

void CItemKeyTextValue::SetKeyValue(const CString& key, const CString& value)
{
	m_editKey.SetWindowText(key);
	m_editValue.SetWindowText(value);
}

void CItemKeyTextValue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_ENABLE, m_chkEnable);
	DDX_Control(pDX, IDC_EDIT_QUERY_KEY, m_editKey);
	DDX_Control(pDX, IDC_EDIT2, m_editValue);
}

BOOL CItemKeyTextValue::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	m_chkEnable.SetCheck(TRUE);

	return res;
}


BEGIN_MESSAGE_MAP(CItemKeyTextValue, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_DEL, &CItemKeyTextValue::OnBnClickedBtnDel)
END_MESSAGE_MAP()


// CItemKeyTextValue 消息处理程序


void CItemKeyTextValue::OnBnClickedBtnDel()
{
	if (!this->IsKindOf(&CListCtrlCustom::classCWnd))
		return;
	CListCtrlCustom* parent = dynamic_cast<CListCtrlCustom*>(GetParent());
	if (!parent)
		return;

	int row, col;
	parent->GetItemMouseAbove(row, col);
	if (row > -1)
		parent->DeleteItem(row);
}
