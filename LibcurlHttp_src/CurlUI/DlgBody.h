#pragma once
#include "afxdialogex.h"


// DlgBody 对话框

class DlgBody : public CDialogEx
{
	DECLARE_DYNAMIC(DlgBody)

public:
	DlgBody(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgBody();

// 对话框数据
	enum { IDD = IDD_DLG_BODY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
