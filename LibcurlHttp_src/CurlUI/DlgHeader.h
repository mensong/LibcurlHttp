#pragma once
#include "afxdialogex.h"
#include "ListCtrlCustom.h"
#include "CtrlScale.h"


// DlgHeader 对话框

class DlgHeader : public CDialogEx
{
	DECLARE_DYNAMIC(DlgHeader)

public:
	DlgHeader(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgHeader();

// 对话框数据
	enum { IDD = IDD_DLG_HEADER };

	std::vector<std::pair<CString, CString>> GetHeaders();
	void Clear();
	void AddHeader(const CString& key, const CString& value);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAddHeader();
	CListCtrlCustom m_header;
	CCtrlScale m_scale;
};
