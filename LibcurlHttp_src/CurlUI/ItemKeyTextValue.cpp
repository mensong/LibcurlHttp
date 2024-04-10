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
	, m_rowIdx(-1)
{

}

CItemKeyTextValue::~CItemKeyTextValue()
{
}

void CItemKeyTextValue::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
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
