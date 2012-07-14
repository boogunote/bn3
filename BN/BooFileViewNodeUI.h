#pragma once

class BooFileViewNodeUI : public CHorizontalLayoutUI
{
public:
	BooFileViewNodeUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	bool OnButtonNotify(void* param);
	bool OnTextFeildNotify(void* param);
	void DoInit();
	void SetFocus();
	void UpdateStateButton();

private:
	CControlUI* m_indent;
	CButtonUI* m_button;
	CRichEditUI* m_text;


public:
	int	m_nIndent;
	bool m_bExpand;
	bool m_bHasChild;

};
