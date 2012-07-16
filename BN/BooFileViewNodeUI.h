#pragma once

class BooFileViewNodeUI : public CHorizontalLayoutUI
{
public:
	BooFileViewNodeUI();

	void SetIconIndex(int nIconIndex);
	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	bool OnButtonNotify(void* param);

	void SetOneLine(bool bOneLine, bool bIsInit = false);
	bool OnTextFeildNotify(void* param);

	void SetSelect(bool bSelect);
	void DoInit();
	void SetFocus();
	void UpdateStateButton();
	void SetContent(LPCTSTR lpcstrContent);
	void SetHasChildren(bool bHasChildren);
	void SetBold(bool bBold);
	void SetTextColor(LPCTSTR lpcstrColor);
	DWORD GetTextFieldTextColor();
	void SetBkColor(LPCTSTR lpcstrColor);
	DWORD GetTextFieldBkColor();
	void GetContent(CStdString& strContent);
private:
	CControlUI* m_indent;
	CButtonUI* m_button;
	CControlUI* m_icon;
	CRichEditUI* m_text;

public:
	int	m_nIndent;
	bool m_bExpand;
	bool m_bHasChild;
	bool m_bSelected;
	bool m_bOneLine;
	bool m_bBold;
	int m_nIconIndex;
	CStdString m_strContent;
};
