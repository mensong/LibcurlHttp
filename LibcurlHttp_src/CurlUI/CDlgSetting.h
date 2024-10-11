#pragma once
#include "afxdialogex.h"
#include "CtrlScale.h"
#include <string>

struct GlobalSetting
{
	int timeout = 30;
	CString userAgent =
		_T("Mozilla/999.0 (Windows NT 999) ")
		_T("AppleWebKit/999.999 (KHTML, like Gecko) ")
		_T("Chrome/9999.9999.9999.9999 ")
		_T("Safari/9999.9999");
	bool autoRedirect = true;
	int autoRedirectMaxCount = 3;
	bool autoDecodeResponse = true;
	int showBodyMaxLength = 4096;
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
