#if defined _EventsObserver_included
  #endinput
#endif
#define _EventsObserver_included

forward void OnClientEventListen(int iClient, const char[] szEventName, int iEventID);

public Extension __ext_EventsObserver = 
{
	name = "EventsObserver",
	file = "EventsObserver.ext",
#if defined AUTOLOAD_EXTENSIONS
	autoload = 1,
#else
	autoload = 0,
#endif
#if defined REQUIRE_EXTENSIONS
	required = 1,
#else
	required = 0,
#endif
};