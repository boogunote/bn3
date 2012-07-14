#pragma once

#define CREATENODE_PREVIOUS	0
#define CREATENODE_NEXT		1
#define CREATENODE_CHILD	2

class BooFileViewUI : public CVerticalLayoutUI
{
public:
	BooFileViewUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void DoInit();
	void SetPos(RECT rc);
	bool OnNodeNotify(void* param);

	void ToggleNodeState( BooFileViewNodeUI* pSenderNode );
	int GetFocusedNodeIndex(BooFileViewNodeUI* pNode);
	bool HasChildrenNode(BooFileViewNodeUI* pNode);
};