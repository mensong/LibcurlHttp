// DlgHeader.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgHeader.h"
#include "ItemKeyTextValue.h"


// DlgHeader 对话框

IMPLEMENT_DYNAMIC(DlgHeader, CDialogEx)

DlgHeader::DlgHeader(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_HEADER, pParent)
{

}

DlgHeader::~DlgHeader()
{
}

std::vector<std::pair<CString, CString>> DlgHeader::GetHeaders()
{
	std::vector<std::pair<CString, CString>> ret;
	int nCount = m_header.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CItemKeyTextValue* item = dynamic_cast<CItemKeyTextValue*>(m_header.GetCtrl(i, 0));
		if (!item)
			continue;
		if (!item->IsEnable())
			continue;
		auto kv = item->GetKeyValue();
		ret.push_back(kv);
	}
	return ret;
}

void DlgHeader::Clear()
{
	m_header.DeleteAllItems();
}

void DlgHeader::AddHeader(const CString& key, const CString& value)
{
	int nRow = m_header.InsertItem(m_header.GetItemCount(), _T(""));
	CItemKeyTextValue* pItemKV = new CItemKeyTextValue();
	pItemKV->Create(CItemKeyTextValue::IDD, &m_header);
	pItemKV->SetKeyValue(key, value);
	CRect rc;
	pItemKV->GetWindowRect(rc);
	m_header.SetColumnWidth(0, rc.Width());
	m_header.SetRowHeight(rc.Height());
	m_header.SetItemEx(nRow, 0, pItemKV);
}

void DlgHeader::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_HEADERS, m_header);
}

BOOL DlgHeader::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	m_scale.Init(GetSafeHwnd());

	int colIdx = m_header.InsertColumn(0, _T(""));

	return res;
}


BEGIN_MESSAGE_MAP(DlgHeader, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_ADD_HEADER, &DlgHeader::OnBnClickedBtnAddHeader)
END_MESSAGE_MAP()


// DlgHeader 消息处理程序


void DlgHeader::OnBnClickedBtnAddHeader()
{
	int nRow = m_header.InsertItem(m_header.GetItemCount(), _T(""));
	CItemKeyTextValue* pItemKV = new CItemKeyTextValue();
	pItemKV->Create(CItemKeyTextValue::IDD, &m_header);
	CRect rc;
	pItemKV->GetWindowRect(rc);
	m_header.SetColumnWidth(0, rc.Width());
	m_header.SetRowHeight(rc.Height());
	m_header.SetItemEx(nRow, 0, pItemKV);
}
