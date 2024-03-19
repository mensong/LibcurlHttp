#pragma once
#include "afxdialogex.h"


// DlgQueryParams 对话框

class DlgQueryParams : public CDialogEx
{
	DECLARE_DYNAMIC(DlgQueryParams)

public:
	DlgQueryParams(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgQueryParams();

// 对话框数据
	enum { IDD = IDD_DLG_QUERYPARAMS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
