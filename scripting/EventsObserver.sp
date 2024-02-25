#include <EventsObserver>

public void OnClientEventListen(int iClient, const char[] szEventName, int iEventID)
{
	PrintToServer("%d %s %d", iClient, szEventName, iEventID);
}