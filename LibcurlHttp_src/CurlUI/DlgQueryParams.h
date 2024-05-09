#pragma once
#include "afxdialogex.h"
#include "ListCtrlCustom.h"
#include "CtrlScale.h"
#include <vector>

// DlgQueryParams 对话框

class DlgQueryParams : public CDialogEx
{
	DECLARE_DYNAMIC(DlgQueryParams)

public:
	DlgQueryParams(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~DlgQueryParams();

// 对话框数据
	enum { IDD = IDD_DLG_QUERYPARAMS };

	std::vector<std::pair<CString, CString>> GetQueryParams();
	void Clear();
	void AddQueryParam(const CString& key, const CString& value);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CListCtrlCustom m_queryParams;
	CCtrlScale m_scale;
	afx_msg void OnBnClickedBtnAddQueryParam();
};
