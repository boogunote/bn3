#include "stdafx.h"
#include "DialogBuilderCallback.h"
#include "BooTextFieldUI.h"
#include "BooFileViewNodeUI.h"
#include "BooFileViewUI.h"

CControlUI* CDialogBuilderCallbackEx::CreateControl( LPCTSTR pstrClass )
{
	if( _tcscmp(pstrClass, _T("BooFileViewUI")) == 0 ) return new BooFileViewUI;
	if( _tcscmp(pstrClass, _T("BooFileViewNode")) == 0 ) return new BooFileViewNodeUI;
	if( _tcscmp(pstrClass, _T("BooTextFieldUI")) == 0 ) return new BooTextFieldUI;
	return NULL;
}