#include "stdafx.h"
#include "BooFileView.h"

BooFileViewNode::BooFileViewNode()
{
	m_indent = new CControlUI;
	m_indent->ApplyAttributeList(_T("float=\"false\" bordersize=\"0\" width=\"100\" height=\"0\""));
	this->Add(m_indent);

	m_text = new CRichEditUI;
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