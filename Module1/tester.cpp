#include "tester.h"

tester::tester(Game &game)
{
	print(game);
}

void tester::print(Game game)
{
	game.printHello();
}
