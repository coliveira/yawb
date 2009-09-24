// (c) 2009 by Carlos Oliveira

#include "stdafx.h"
#include "Events.h"

using namespace std;

static Events *g_events = 0;

Events *Events::GetEvents()
{
    if (!g_events) 
        g_events = new Events();
    return g_events;
}

void Events::AddEvent(const char *msg, Severity type) 
{
    events.push_back(make_pair(string(msg), type));
}
