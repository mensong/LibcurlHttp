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

void CDlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgSetting, CDialogEx)
END_MESSAGE_MAP()


// CDlgSetting 消息处理程序
