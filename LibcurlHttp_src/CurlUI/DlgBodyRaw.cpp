// DlgBodyRaw.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgBodyRaw.h"


// CDlgBodyRaw 对话框

IMPLEMENT_DYNAMIC(CDlgBodyRaw, CDialogEx)

CDlgBodyRaw::CDlgBodyRaw(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_BODY_RAW, pParent)
{

}

CDlgBodyRaw::~CDlgBodyRaw()
{
}

CString CDlgBodyRaw::GetData()
{
	CString data;
	m_editBodyData.GetWindowText(data);
	return data;
}

void CDlgBodyRaw::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_BODY_RAW, m_editBodyData);
}

BOOL CDlgBodyRaw::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	m_scale.Init(GetSafeHwnd());

	return res;
}


BEGIN_MESSAGE_MAP(CDlgBodyRaw, CDialogEx)
END_MESSAGE_MAP()


// CDlgBodyRaw 消息处理程序
