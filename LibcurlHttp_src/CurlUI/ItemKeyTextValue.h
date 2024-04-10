#pragma once
#include "afxdialogex.h"


// CItemKeyTextValue 对话框

class CItemKeyTextValue : public CDialogEx
{
	DECLARE_DYNAMIC(CItemKeyTextValue)

public:
	CItemKeyTextValue(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CItemKeyTextValue();

// 对话框数据
	enum { IDD = IDD_DIALOG_KEY_TEXT_VALUE };

	int m_rowIdx;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnDel();
};
