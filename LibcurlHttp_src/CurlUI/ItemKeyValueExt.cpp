// CItemKeyValueExt.cpp: 实现文件
//

#include "pch.h"
#include "CurlUI.h"
#include "afxdialogex.h"
#include "ItemKeyValueExt.h"
#include "ListCtrlCustom.h"


// CItemKeyValueExt 对话框

IMPLEMENT_DYNAMIC(CItemKeyValueExt, CDialogEx)

CItemKeyValueExt::CItemKeyValueExt(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_KEY_VALUE_EXT, pParent)
	, m_rowIdx(-1)
{

}

CItemKeyValueExt::~CItemKeyValueExt()
{
}

void CItemKeyValueExt::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHK_IS_FILE, m_btnIsFile);
	DDX_Control(pDX, IDC_EDIT2, m_editValue);
}


BEGIN_MESSAGE_MAP(CItemKeyValueExt, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_DEL, &CItemKeyValueExt::OnBnClickedBtnDel)
	ON_BN_CLICKED(IDC_CHK_IS_FILE, &CItemKeyValueExt::OnBnClickedChkIsFile)
END_MESSAGE_MAP()


// CItemKeyValueExt 消息处理程序

void CItemKeyValueExt::OnBnClickedBtnDel()
{
	if (!this->IsKindOf(&CListCtrlCustom::classCWnd))
		return;
	CListCtrlCustom* parent = dynamic_cast<CListCtrlCustom*>(GetParent());
	if (!parent)
		return;

	int row, col;
	parent->GetItemMouseAbove(row, col);
	if (row > -1)
		parent->DeleteItem(row);
}

void CItemKeyValueExt::OnBnClickedChkIsFile()
{
	if (!m_btnIsFile.GetCheck())
		return;

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
	
	m_editValue.SetWindowText(szPath);
}
