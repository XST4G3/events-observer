#include "extension.h"

EventsObserver g_EventsObserver;
SMEXT_LINK(&g_EventsObserver);

CDetour* pDetour{ nullptr };
IGameConfig* pGameConfig{ nullptr };
IForward* fwdClientEventListen{ nullptr };

DETOUR_DECL_MEMBER3(ListenEvents, bool, void*, listener, CGameEventDescriptor*, descriptor, eEvent, nListenerType)
{
	if (nListenerType != eEvent::CLIENTSTUB)
		return DETOUR_MEMBER_CALL(ListenEvents)(listener, descriptor, nListenerType);

	int client = (reinterpret_cast<CBaseClient*>(listener))->GetPlayerSlot() + 1;
	auto player = playerhelpers->GetGamePlayer(client);

	if (player->IsFakeClient())
		return DETOUR_MEMBER_CALL(ListenEvents)(listener, descriptor, nListenerType);

	fwdClientEventListen->PushCell(client);
	fwdClientEventListen->PushString(descriptor->GetName());
	fwdClientEventListen->PushCell(descriptor->GetEventID());
	fwdClientEventListen->Execute();

	return DETOUR_MEMBER_CALL(ListenEvents)(listener, descriptor, nListenerType);
}

bool EventsObserver::SDK_OnLoad(char* error, size_t maxlen, bool late)
{
	if (!gameconfs->LoadGameConfigFile("EventsObserver.games", &pGameConfig, error, maxlen))
	{
		smutils->Format(error, maxlen - 1, "Failed to load gamedata");
		return false;
	}

	CDetourManager::Init(smutils->GetScriptingEngine(), pGameConfig);

	const char* pNameFunc = "CGameEventManager::AddListener";
	pDetour = DETOUR_CREATE_MEMBER(ListenEvents, pNameFunc);

	if (!pDetour)
	{
		smutils->Format(error, maxlen, "Failed to create CDetour pointer - %s", pNameFunc);
		return false;
	}

	pDetour->EnableDetour();

	fwdClientEventListen = forwards->CreateForward("OnClientEventListen", ET_Ignore, 3, NULL, Param_Cell, Param_String, Param_Cell);
	sharesys->RegisterLibrary(myself, "EventsObserver");

	return true;
}

void EventsObserver::SDK_OnUnload()
{
	gameconfs->CloseGameConfigFile(pGameConfig);
	pDetour->DisableDetour();
	forwards->ReleaseForward(fwdClientEventListen);
}