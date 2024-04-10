#pragma once
#include "afxdialogex.h"
#include "CtrlScale.h"


// CDlgBodyFile 对话框

class CDlgBodyFile : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBodyFile)

public:
	CDlgBodyFile(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgBodyFile();

// 对话框数据
	enum { IDD = IDD_DLG_BODY_FILE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editFilePath;
	CCtrlScale m_scale;
	afx_msg void OnBnClickedBtnBroswer();
};
