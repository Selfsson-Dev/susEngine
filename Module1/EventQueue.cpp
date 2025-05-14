#include "EventQueue.h"

int EventQueue::head = 0;
int EventQueue::tail = 0;
EventDescription* EventQueue::eventsQueued[EventQueue::MAX_PENDING] = { nullptr };

void EventQueue::add_event(entt::entity entity, Event event)
{
	assert(tail < MAX_PENDING);

	eventsQueued[tail]->Entity = entity;
	eventsQueued[tail]->EventType = event;
	tail = (tail + 1) % MAX_PENDING;
}

void EventQueue::process_events()
{
	while (!empty()) {
		Subject::notify(eventsQueued[head]->Entity, eventsQueued[head]->EventType);
		head = (head + 1) % MAX_PENDING;
	}
}
