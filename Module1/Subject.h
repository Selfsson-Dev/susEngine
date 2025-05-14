#pragma once
#include "Observer.h"
#include <vector>


class Subject
{
private:
	static const int MAX_OBSERVERS = 32;
	//std::vector<Observer> observers;
	static Observer* observers[MAX_OBSERVERS];
	static int numObservers;

public:
	static void init();

	static void add_observer(Observer* observer);

	static void remove_observer(Observer* observer);

	static void notify(const entt::entity& entity, Event event);
};

