#pragma once
#include "afxdialogex.h"


// CItemKeyValueExt 对话框

class CItemKeyValueExt : public CDialogEx
{
	DECLARE_DYNAMIC(CItemKeyValueExt)

public:
	CItemKeyValueExt(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CItemKeyValueExt();

// 对话框数据
	enum { IDD = IDD_DIALOG_KEY_VALUE_EXT };

	int m_rowIdx;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedChkIsFile();
	CButton m_btnIsFile;
	CEdit m_editValue;
};
