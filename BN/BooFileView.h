#pragma once

class BooFileViewUI : public CContainerUI
{
public:
	BooFileViewUI()
	{
		CDialogBuilder builder;
		CContainerUI* pComputerExamine = static_cast<CContainerUI*>(builder.Create(_T("BooFileView.xml"), (UINT)0));
		if( pComputerExamine ) {
			this->Add(pComputerExamine);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass) 
	{
		if( _tcscmp(pstrClass, _T("BooFileView")) == 0 ) return new BooFileViewUI;
		return NULL;
	}
};