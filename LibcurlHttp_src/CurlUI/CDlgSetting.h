#pragma once
#include "afxdialogex.h"
#include "CtrlScale.h"
#include <string>

struct GlobalSetting
{
	int timeout = 0;
	CString userAgent = _T("LibcurlHttp");
	bool autoRedirect = true;
	int autoRedirectMaxCount = 3;
	bool gzip = true;

};

// CDlgSetting 对话框

class CDlgSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgSetting();

// 对话框数据
	enum { IDD = IDD_DLG_SETTING };

	CCtrlScale m_scale;

	void initProperties();

	GlobalSetting& GetSetting() { return m_setting; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	CMFCPropertyGridCtrl m_property;
	GlobalSetting m_setting;
};
