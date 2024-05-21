// DlgBodyFile.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "DlgBodyFile.h"


// CDlgBodyFile 对话框

IMPLEMENT_DYNAMIC(CDlgBodyFile, CDialogEx)

CDlgBodyFile::CDlgBodyFile(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_BODY_FILE, pParent)
{

}

CDlgBodyFile::~CDlgBodyFile()
{
}

CString CDlgBodyFile::GetFilePath()
{
	CString filepath;
	m_editFilePath.GetWindowText(filepath);
	return filepath;
}

void CDlgBodyFile::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_FILE_PATH, m_editFilePath);
}

BOOL CDlgBodyFile::OnInitDialog()
{
	BOOL res = __super::OnInitDialog();

	m_scale.Init(GetSafeHwnd());

	return res;
}


BEGIN_MESSAGE_MAP(CDlgBodyFile, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BROSWER, &CDlgBodyFile::OnBnClickedBtnBroswer)
END_MESSAGE_MAP()


// CDlgBodyFile 消息处理程序


void CDlgBodyFile::OnBnClickedBtnBroswer()
{
	OPENFILENAME ofn = { 0 };
	TCHAR lpstrFilename[MAX_PATH * 20] = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = _T("All Files(*.*)\0*.*\0\0");
	ofn.lpstrFile = lpstrFilename;
	ofn.nMaxFile = MAX_PATH * 20;
	//ofn.lpstrInitialDir = TEXT("C:\\"); //初始化一个路径
	ofn.Flags = OFN_HIDEREADONLY | OFN_FORCESHOWHIDDEN | OFN_PATHMUSTEXIST | OFN_EXPLORER/* | OFN_ALLOWMULTISELECT*/;

	if (!GetOpenFileName(&ofn))
		return;

	TCHAR szDir[MAX_PATH] = { 0 };
	TCHAR szName[128] = { 0 };
	TCHAR szPath[MAX_PATH] = { 0 };
	TCHAR* pos = lpstrFilename;

	if (0 == lpstrFilename[_tcslen(lpstrFilename) + 1])// 只有一个文件
	{
		_tcscpy_s(szPath, MAX_PATH, lpstrFilename);
	}
	else
	{
		_tcscpy_s(szDir, MAX_PATH, lpstrFilename);
		_tcscat_s(szDir, MAX_PATH, _T("\\"));

		do
		{
			pos = &pos[_tcslen(pos)] + 1;
			if (0 == *pos)
				break;

			_tcscpy_s(szPath, MAX_PATH, szDir);
			_tcscat_s(szPath, MAX_PATH, pos);

		} while (1);
	}

	m_editFilePath.SetWindowText(szPath);
}
