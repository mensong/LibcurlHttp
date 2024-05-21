
// CurlUIDlg.h: 头文件
//

#pragma once
#include "..\LibcurlHttp\LibcurlHttp.h"
#include <vector>
#include "CtrlScale.h"
#include "CDlgSetting.h"

// CCurlUIDlg 对话框
class CCurlUIDlg : public CDialogEx
{
// 构造
public:
	CCurlUIDlg(CWnd* pParent = nullptr);	// 标准构造函数
	virtual ~CCurlUIDlg();

// 对话框数据
	enum { IDD = IDD_CURLUI_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	template<class TPage>
	void addPage(const CString& pageName);

	CString generateUrlByQueryParams(const CString& srcUrl);
	void refreshQueryParams();
	std::vector<std::pair<CString, CString>> getHeaders();
	
	void fillDefaultSetting(LibcurlHttp* http);

	static int _PROGRESS_CALLBACK(
		double downloadTotal, double downloadNow,
		double uploadTotal, double uploadNow,
		void* userData);

	void dumpResponse(LibcurlHttp* http);

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
	CCtrlScale m_scale;
	CCtrlScale m_scaleTab;
	std::vector<CCtrlScale*> m_pageScales;

	std::vector<CDialogEx*> m_pages;
	CTabCtrl m_tabParams;
	CDlgSetting m_setting;

	afx_msg void OnTcnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeEditUrl();
	afx_msg void OnNMClickbtnsetting(NMHDR* pNMHDR, LRESULT* pResult);
};

template<class TPage>
inline void CCurlUIDlg::addPage(const CString& pageName)
{
	//获取tab control位置和大小
	CRect tabRect, itemRect;
	int nX, nY, nXc, nYc;
	m_tabParams.GetClientRect(&tabRect);
	m_tabParams.GetItemRect(0, &itemRect);
	nX = itemRect.left;
	nY = itemRect.bottom + 1;
	nXc = tabRect.right - itemRect.left - 2;
	nYc = tabRect.bottom - nY - 2;

	m_tabParams.InsertItem(m_tabParams.GetItemCount(), pageName);
	
	TPage* dlg = new TPage();
	m_pages.push_back(dlg);
	dlg->Create(TPage::IDD, &m_tabParams);
	//设置对话框1的显示位置
	dlg->SetWindowPos(&wndTop, nX, nY, nXc, nYc, SWP_SHOWWINDOW);
	dlg->ShowWindow(SW_HIDE);

	m_scaleTab.SetAnchor(dlg->GetSafeHwnd(),
		CCtrlScale::AnchorLeftToWinLeft |
		CCtrlScale::AnchorRightToWinRight |
		CCtrlScale::AnchorTopToWinTop |
		CCtrlScale::AnchorBottomToWinBottom
	);

	CCtrlScale* scale = new CCtrlScale();
	m_pageScales.push_back(scale);
	scale->Init(dlg->GetSafeHwnd());

}
