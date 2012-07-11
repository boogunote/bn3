#pragma once

#define CREATENODE_PREVIOUS	0
#define CREATENODE_NEXT		1
#define CREATENODE_CHILD	2

class BooTextFieldUI : public CRichEditUI
{
public:
	BooTextFieldUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	void DoInit();

	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);
	void OnTxNotify(DWORD iNotify, void *pv);
	void DoEvent(TEventUI& event);

private:
	SIZE m_szRequest;
};

class BooFileViewNode : public CHorizontalLayoutUI
{
public:
	BooFileViewNode();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	bool OnTextFeildNotify(void* param);
	void DoInit();

private:
	CControlUI* m_indent;
	CButtonUI* m_button;
	CRichEditUI* m_text;
	int	m_nIndent;
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

	void DoInit();
	void SetPos(RECT rc);
	bool OnNodeNotify(void* param);
	int GetFocusedNodeIndex( CControlUI* pControl );
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