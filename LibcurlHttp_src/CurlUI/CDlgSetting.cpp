// CDlgSetting.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "CDlgSetting.h"


// CDlgSetting 对话框

IMPLEMENT_DYNAMIC(CDlgSetting, CDialogEx)

CDlgSetting::CDlgSetting(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_SETTING, pParent)
{

}

CDlgSetting::~CDlgSetting()
{
}

void CDlgSetting::initProperties()
{
	m_property.EnableHeaderCtrl(FALSE);

	HDITEM item;
	item.cxy = 200;
	item.mask = HDI_WIDTH;
	m_property.GetHeaderCtrl().SetItem(0, new HDITEM(item));

	TCHAR buff[20];

	CMFCPropertyGridProperty* pTimeoutItem =
		new CMFCPropertyGridProperty(
			_T("网络超时"),
			_itot(m_setting.timeout, buff, 10),
			_T("单位：秒，0表示网络不超时"));
	m_property.AddProperty(pTimeoutItem);

	CMFCPropertyGridProperty* pUserAgentItem =
		new CMFCPropertyGridProperty(
			_T("UserAgent"),
			m_setting.userAgent,
			_T("用户代理，如果没有在请求头中另外设置则使用这个，否则使用请求头中的"));
	m_property.AddProperty(pUserAgentItem);

	CMFCPropertyGridProperty* pIsAutoRedirectItem = new CMFCPropertyGridProperty(
		_T("是否自动重定向"),
		m_setting.autoRedirect ? _T("自动重定向") : _T("不自动重定向"),
		_T("301/302/300时是否自动跳转"));
	pIsAutoRedirectItem->AddOption(_T("自动重定向"));
	pIsAutoRedirectItem->AddOption(_T("不自动重定向"));
	pIsAutoRedirectItem->AllowEdit(FALSE);  //不允许对选项进行编辑
	m_property.AddProperty(pIsAutoRedirectItem);

	CMFCPropertyGridProperty* pMaxAutoRedirectCountItem =
		new CMFCPropertyGridProperty(
			_T("自动重定向最大次数"),
			_itot(m_setting.autoRedirectMaxCount, buff, 10),
			_T("301/302/300时是否自动跳转的最大次数，只有在“是否自动重定向”有效时才有效"));
	m_property.AddProperty(pMaxAutoRedirectCountItem);

	CMFCPropertyGridProperty* pIsGZipItem = new CMFCPropertyGridProperty(
		_T("是否自动对结果解包"),
		m_setting.autoDecodeResponse ? _T("自动解包") : _T("不自动解包"),
		_T("内容如果经过Gzip压缩的，是否自动使用Gzip算法解压"));
	pIsGZipItem->AddOption(_T("自动解包"));
	pIsGZipItem->AddOption(_T("不自动解包"));
	pIsGZipItem->AllowEdit(FALSE);  //不允许对选项进行编辑
	m_property.AddProperty(pIsGZipItem);

	CMFCPropertyGridProperty* pShowBodyMaxLength =
		new CMFCPropertyGridProperty(
			_T("界面上显示的数据最长长度"),
			_itot(m_setting.showBodyMaxLength, buff, 10),
			_T("如果Body长度大于这个数值，则不显示超出的内容"));
	m_property.AddProperty(pShowBodyMaxLength);
}

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_gridProperty, m_property);
}

BOOL CDlgSetting::OnInitDialog()
{
	if (!__super::OnInitDialog())
		return FALSE;
	
	m_scale.SetAnchor(m_property.GetSafeHwnd(), 
		CCtrlScale::AnchorTopToWinTop | 
		CCtrlScale::AnchorLeftToWinLeft | 
		CCtrlScale::AnchorLeftToWinRight | 
		CCtrlScale::AnchorBottomToWinBottom);
	m_scale.SetAnchor(GetDlgItem(IDOK)->GetSafeHwnd(),
		CCtrlScale::AnchorTopToWinBottom | 
		CCtrlScale::AnchorBottomToWinBottom);
	m_scale.SetAnchor(GetDlgItem(IDCANCEL)->GetSafeHwnd(), 
		CCtrlScale::AnchorTopToWinBottom |
		CCtrlScale::AnchorBottomToWinBottom);
	m_scale.Init(GetSafeHwnd());

	initProperties();

	return TRUE;
}


BEGIN_MESSAGE_MAP(CDlgSetting, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgSetting::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSetting 消息处理程序


void CDlgSetting::OnBnClickedOk()
{
	for (int i = 0; i < m_property.GetPropertyCount(); i++)
	{
		CMFCPropertyGridProperty* prop = m_property.GetProperty(i);
		CString name = prop->GetName();
		if (name == _T("网络超时"))
		{
			m_setting.timeout = _ttoi(prop->GetValue().bstrVal);
		}
		else if (name == _T("UserAgent"))
		{
			m_setting.userAgent = prop->GetValue().bstrVal;
		}
		else if (name == _T("是否自动重定向"))
		{
			CString v = prop->GetValue().bstrVal;
			m_setting.autoRedirect = v == _T("自动重定向");
		}
		else if (name == _T("自动重定向最大次数"))
		{
			m_setting.autoRedirectMaxCount = _ttoi(prop->GetValue().bstrVal);
		}
		else if (name == _T("是否自动对结果解包"))
		{
			CString v = prop->GetValue().bstrVal;
			m_setting.autoDecodeResponse = v == _T("自动解包");
		}
		else if (name == _T("界面上显示的数据最长长度"))
		{
			m_setting.showBodyMaxLength = _ttoi(prop->GetValue().bstrVal);
		}
	}

	CDialogEx::OnOK();
}
