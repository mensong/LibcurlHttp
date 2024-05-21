// DlgBody_x_www_form_urlencoded.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgBody_x_www_form_urlencoded.h"
#include "ItemKeyTextValue.h"
#include "../LibcurlHttp/LibcurlHttp.h"


// CDlgBody_x_www_form_urlencoded 对话框

IMPLEMENT_DYNAMIC(CDlgBody_x_www_form_urlencoded, CDialogEx)

CDlgBody_x_www_form_urlencoded::CDlgBody_x_www_form_urlencoded(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_BODY_X_WWW_FORM_URLENCODED, pParent)
{

}

CDlgBody_x_www_form_urlencoded::~CDlgBody_x_www_form_urlencoded()
{
}

std::vector<std::pair<CString, CString>> CDlgBody_x_www_form_urlencoded::GetValues()
{
	std::vector<std::pair<CString, CString>> ret;
	int nCount = m_bodyData.GetItemCount();
	for (int i = 0; i < nCount; i++)
	{
		CItemKeyTextValue* item = dynamic_cast<CItemKeyTextValue*>(m_bodyData.GetCtrl(i, 0));
		if (!item)
			continue;
		if (!item->IsEnable())
			continue;
		std::pair<CString, CString> v;
		auto kv = item->GetKeyValue();
		v.first = kv.first;
		v.second = kv.second;
		ret.push_back(v);
	}
	return ret;
}

std::string CDlgBody_x_www_form_urlencoded::GetTextValue()
{
	std::string content;

	LibcurlHttp* http = HTTP_CLIENT::Ins().CreateHttp();

	auto values = GetValues();
	for (size_t i = 0; i < values.size(); i++)
	{
		std::string key = CW2A(values[i].first);
		std::string value = CW2A(values[i].second);
		if (!content.empty())
			content += "&";
		content += http->UrlUTF8Encode(key.c_str());
		content += "=";
		content += http->UrlUTF8Encode(value.c_str());
	}

	HTTP_CLIENT::Ins().ReleaseHttp(http);
	return content;
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
