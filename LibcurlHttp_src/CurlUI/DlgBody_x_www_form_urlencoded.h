#pragma once
#include "afxdialogex.h"
#include "ListCtrlCustom.h"
#include "CtrlScale.h"
#include <string>

// CDlgBody_x_www_form_urlencoded 对话框

class CDlgBody_x_www_form_urlencoded : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBody_x_www_form_urlencoded)

public:
	CDlgBody_x_www_form_urlencoded(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgBody_x_www_form_urlencoded();

// 对话框数据
	enum { IDD = IDD_DLG_BODY_X_WWW_FORM_URLENCODED };

	CListCtrlCustom m_bodyData;
	CCtrlScale m_scale;

	//<<key, value>>
	std::vector<std::pair<CString, CString>> GetValues();
	//
	std::string GetTextValue();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnAdd();
};
