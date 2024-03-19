
// CurlUIDlg.h: 头文件
//

#pragma once
#include "..\LibcurlHttp\LibcurlHttp.h"
#include <vector>

// CCurlUIDlg 对话框
class CCurlUIDlg : public CDialogEx
{
// 构造
public:
	CCurlUIDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CURLUI_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedBtnRun();
	CComboBox m_cmbMethod;
	CEdit m_editUrl;
	CEdit m_editResponseBody;
	CEdit m_editResponseHeader;
	CProgressCtrl m_progress;

	std::vector<CDialogEx*> m_vctPages;
	CTabCtrl m_tabParams;
	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
};
