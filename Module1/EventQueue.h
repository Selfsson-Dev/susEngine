#pragma once
#include "EventTypes.h"
#include <entt/fwd.hpp>
#include "Subject.h"

class EventQueue
{
private:
	static const int MAX_PENDING = 32;
	static int head;
	static int tail;
	static EventDescription* eventsQueued[MAX_PENDING];
	static bool empty() { return head == tail; };

public:
	static void init()
	{
		head = 0;
		tail = 0;
		for (int i = 0; i < MAX_PENDING; ++i) {
			eventsQueued[i] = new EventDescription();
		}
	}

	static void add_event(entt::entity, Event);
	static void process_events();
};

