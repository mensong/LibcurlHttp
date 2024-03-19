#pragma once
#include "afxdialogex.h"


// DlgHeader 对话框

class DlgHeader : public CDialogEx
{
	DECLARE_DYNAMIC(DlgHeader)

public:
	DlgHeader(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgHeader();

// 对话框数据
	enum { IDD = IDD_DLG_HEADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
