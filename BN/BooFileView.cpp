#include "stdafx.h"
#include "BooFileView.h"

BooTextFieldUI::BooTextFieldUI()
{
}

LPCTSTR BooTextFieldUI::GetClass() const
{
	return _T("BooTextFieldUI");
}

LPVOID BooTextFieldUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("BooTextField")) == 0 ) return static_cast<CRichEditUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void BooTextFieldUI::DoInit()
{
	__super::DoInit();
	this->SetEventMask(ENM_REQUESTRESIZE|ENM_LINK);
	this->SetText(L".");
	this->SetText(L"");
}

void BooTextFieldUI::OnTxNotify(DWORD iNotify, void *pv)
{
	if( iNotify == EN_REQUESTRESIZE ) {
		REQRESIZE *preqsz = (REQRESIZE *)pv;
		if (m_szRequest.cx != preqsz->rc.right ||
			m_szRequest.cy != preqsz->rc.bottom)
		{
			m_szRequest.cx = preqsz->rc.right;
			m_szRequest.cy = preqsz->rc.bottom;
			WCHAR szHeight[BUF_128B];
			_itow_s(m_szRequest.cy, szHeight, BUF_128B, 10);
			this->SetAttribute(L"height", szHeight);
			this->GetParent()->SetAttribute(L"height", szHeight);
		}
	}
}

BooFileViewNode::BooFileViewNode()
{
	m_indent = new CControlUI;
	m_indent->ApplyAttributeList(_T("float=\"false\" bordersize=\"0\" width=\"100\" height=\"0\""));
	this->Add(m_indent);

	m_text = new BooTextFieldUI;
	m_text->ApplyAttributeList(_T("width=\"0\" height=\"0\" bkcolor=\"#FFFFFFFF\" bordercolor=\"#FF0000FF\" bordersize=\"1\""));
	this->Add(m_text);
}

LPCTSTR BooFileViewNode::GetClass() const
{
	return _T("BooFileViewNode");
}

LPVOID BooFileViewNode::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("BooFileViewNode")) == 0 ) return static_cast<BooFileViewNode*>(this);
	return CContainerUI::GetInterface(pstrName);
}




BooFileViewUI::BooFileViewUI()
{
}

LPCTSTR BooFileViewUI::GetClass() const
{
	return _T("BooFileViewUI");
}

LPVOID BooFileViewUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("BooFileViewUI")) == 0 ) return static_cast<BooFileViewUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void BooFileViewUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
}