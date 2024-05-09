#pragma once
#include "afxdialogex.h"
#include <utility>

// CItemKeyTextValue 对话框

class CItemKeyTextValue : public CDialogEx
{
	DECLARE_DYNAMIC(CItemKeyTextValue)

public:
	CItemKeyTextValue(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CItemKeyTextValue();

// 对话框数据
	enum { IDD = IDD_DIALOG_KEY_TEXT_VALUE };

	bool IsEnable();
	std::pair<CString, CString> GetKeyValue();
	void SetKeyValue(const CString& key, const CString& value);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnDel();
	CButton m_chkEnable;
	CEdit m_editKey;
	CEdit m_editValue;
};
