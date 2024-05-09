#pragma once
#include "afxdialogex.h"
#include "ListCtrlCustom.h"
#include "CtrlScale.h"


// CDlgBodyMultipart 对话框

class CDlgBodyMultipart : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBodyMultipart)

public:
	CDlgBodyMultipart(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgBodyMultipart();

	//<<key, <value, isFile>>>
	std::vector<std::pair<CString, std::pair<CString, bool>>> GetValues();

// 对话框数据
	enum { IDD = IDD_DLG_BODY_MULTIPART };

	CListCtrlCustom m_formData;
	CCtrlScale m_scale;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAddQueryParam();
};
