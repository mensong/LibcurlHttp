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

std::vector<std::pair<CString, CString>> DlgQueryParams::GetQueryParams()
{
	std::vector<std::pair<CString, CString>> ret;
	int nCount = m_queryParams.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CItemKeyTextValue* item = dynamic_cast<CItemKeyTextValue*>(m_queryParams.GetCtrl(i, 0));
		if (!item)
			continue;
		if (!item->IsEnable())
			continue;
		auto kv = item->GetKeyValue();
		ret.push_back(kv);
	}
	return ret;
}

void DlgQueryParams::Clear()
{
	m_queryParams.DeleteAllItems();
}

void DlgQueryParams::AddQueryParam(const CString& key, const CString& value)
{
	int nRow = m_queryParams.InsertItem(m_queryParams.GetItemCount(), _T(""));
	CItemKeyTextValue* pItemKV = new CItemKeyTextValue();
	pItemKV->Create(CItemKeyTextValue::IDD, &m_queryParams);
	pItemKV->SetKeyValue(key, value);
	CRect rc;
	pItemKV->GetWindowRect(rc);
	m_queryParams.SetColumnWidth(0, rc.Width());
	m_queryParams.SetRowHeight(rc.Height());
	m_queryParams.SetItemEx(nRow, 0, pItemKV);
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
	AddQueryParam(_T(""), _T(""));
}
