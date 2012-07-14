#pragma once

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
