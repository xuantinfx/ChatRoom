#pragma once
#include "stdafx.h"

struct MySocket
{
	SOCKET* socket;
	CSocket *csocket;

};

class ChatterInfo
{
public:
	CSocket *sv_connectorReceive,*sv_connectorSend;
	SOCKET socketSendAttach;
	WCHAR username[50];
	BOOL status;
	ChatterInfo() {}
	ChatterInfo(WCHAR *_username) {
		StrCpyW(username, _username);
		status = FALSE;
		sv_connectorReceive = NULL;
		sv_connectorSend = NULL;
	}
	~ChatterInfo() {
		if (sv_connectorReceive != NULL)
		{
			sv_connectorReceive->Close();
			delete sv_connectorReceive;
		}
		if (sv_connectorSend != NULL) {
			sv_connectorSend->Close();
			delete sv_connectorSend;
		}
	}
};

int ID = 0;
class CGroupChat
{
public:
	vector<ChatterInfo *>	lMember;
	WCHAR				nameGroup[50];
	int					id;
	CGroupChat() {
		wsprintf(nameGroup, L"Group name");
		id = ID++;
	}
	CGroupChat(WCHAR * _groupName) {
		wsprintf(nameGroup, _groupName);
		id = ID++;
	}
	~CGroupChat() {

	}
};