// DlgQueryParams.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgQueryParams.h"


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
}

BOOL DlgQueryParams::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();



	return res;
}


BEGIN_MESSAGE_MAP(DlgQueryParams, CDialogEx)
END_MESSAGE_MAP()


// DlgQueryParams 消息处理程序
