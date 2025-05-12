#pragma once
#include "Game.hpp"

class tester
{
public:
	tester() {};
	tester(Game &game);


	Game game;

	void print(Game game);
};