#pragma once
#include "afxdialogex.h"
#include "CtrlScale.h"


// CDlgBodyRaw 对话框

class CDlgBodyRaw : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgBodyRaw)

public:
	CDlgBodyRaw(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgBodyRaw();

// 对话框数据
	enum { IDD = IDD_DLG_BODY_RAW };

	CString GetData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CEdit m_editBodyData;
	CCtrlScale m_scale;
};
