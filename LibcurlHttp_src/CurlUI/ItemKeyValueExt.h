#pragma once
#include "afxdialogex.h"
#include <utility>

// CItemKeyValueExt 对话框

class CItemKeyValueExt : public CDialogEx
{
	DECLARE_DYNAMIC(CItemKeyValueExt)

public:
	CItemKeyValueExt(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CItemKeyValueExt();

// 对话框数据
	enum { IDD = IDD_DIALOG_KEY_VALUE_EXT };

	bool IsEnable();
	bool IsFile();
	std::pair<CString, CString> GetKeyValue();
	void SetKeyValue(const CString& key, const CString& value);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnDel();
	afx_msg void OnBnClickedChkIsFile();
	CButton m_btnIsFile;
	CEdit m_editValue;
	CButton m_chkEnable;
	CEdit m_editKey;
};
