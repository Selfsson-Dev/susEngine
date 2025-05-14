#pragma once
#include "Observer.h"
#include <vector>

const int MAX_OBSERVERS = 1024;

class Subject
{
private:
	//std::vector<Observer> observers;
	Observer* observers[MAX_OBSERVERS];
	int numObservers = 0;

public:
	void add_observer(Observer* observer);

	void remove_observer(Observer* observer);

protected:
	void notify(const entt::entity& entity, Event event);
};

