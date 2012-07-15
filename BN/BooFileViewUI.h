#pragma once

#define CREATENODE_PREVIOUS	0
#define CREATENODE_NEXT		1
#define CREATENODE_CHILD	2

class BooFileViewUI : public CVerticalLayoutUI, public IMessageFilterUI
{
public:
	BooFileViewUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void DoInit();
	void SetPos(RECT rc);
	bool OnNodeNotify(void* param);

	BooFileViewNodeUI* CreateNode(int nIndent, int nInsertAt);
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void CleanSelect();
	void ToggleNodeState( BooFileViewNodeUI* pSenderNode );
	int GetFocusedNodeIndex(BooFileViewNodeUI* pNode);
	bool HasChildrenNode(BooFileViewNodeUI* pNode);
};