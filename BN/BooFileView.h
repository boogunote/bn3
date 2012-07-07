#pragma once

class BooFileViewNode : public CHorizontalLayoutUI
{
public:
	BooFileViewNode();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

private:
	CControlUI* m_indent;
	CButtonUI* m_button;
	CRichEditUI* m_text;
};

class BooFileViewUI : public CVerticalLayoutUI
{
public:
	BooFileViewUI();
//	{
// 		CDialogBuilder builder;
// 		CContainerUI* pComputerExamine = static_cast<CContainerUI*>(builder.Create(_T("BooFileView.xml"), (UINT)0));
// 		if( pComputerExamine ) {
// 			this->Add(pComputerExamine);
// 		}
// 		else {
// 			this->RemoveAll();
// 			return;
// 		}
//	}

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetPos(RECT rc);
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if( _tcscmp(pstrClass, _T("BooFileViewUI")) == 0 ) return new BooFileViewUI;
		if( _tcscmp(pstrClass, _T("BooFileViewNode")) == 0 ) return new BooFileViewNode;
		return NULL;
	}
};