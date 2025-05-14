#include "Subject.h"
Observer* Subject::observers[Subject::MAX_OBSERVERS] = { nullptr };
int Subject::numObservers = 0;

void Subject::init() {

	numObservers = 0;
	for (int i = 0; i < MAX_OBSERVERS; ++i) {
		observers[i] = nullptr;
	}
};

void Subject::add_observer(Observer* observer)
{
	assert(observer != nullptr);
	assert(numObservers < MAX_OBSERVERS);

	for (int i = 0; i < numObservers; ++i) {
		assert(observer != observers[i]);
	}

	observers[numObservers] = observer;
	++numObservers;
}

void Subject::remove_observer(Observer* observer)
{
	assert(observer != nullptr);
	assert(numObservers > 0);

	for (int i = 0; i < numObservers; ++i) {
		if (observers[i] == observer) {
			// Swap with the last observer
			observers[i] = observers[numObservers - 1];
			observers[numObservers - 1] = nullptr;

			--numObservers;
			break;
		}
	}
}

void Subject::notify(const entt::entity& entity, Event event)
{
	for (int i = 0; i < numObservers; ++i)
	{
		assert(observers[i] != nullptr);
		observers[i]->onNotify(entity, event);
	}
}


