// DlgHeader.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgHeader.h"


// DlgHeader 对话框

IMPLEMENT_DYNAMIC(DlgHeader, CDialogEx)

DlgHeader::DlgHeader(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_HEADER, pParent)
{

}

DlgHeader::~DlgHeader()
{
}

void DlgHeader::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(DlgHeader, CDialogEx)
END_MESSAGE_MAP()


// DlgHeader 消息处理程序
