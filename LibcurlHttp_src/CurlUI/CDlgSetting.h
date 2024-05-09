#pragma once
#include "afxdialogex.h"


// CDlgSetting 对话框

class CDlgSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSetting)

public:
	CDlgSetting(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDlgSetting();

// 对话框数据
	enum { IDD = IDD_DLG_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
