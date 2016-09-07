#include "state_test.hpp"

#include <iostream>


StateTest *StateTest::instance_ = NULL;
StateTest::StateTest()
{
	std::cout << "StateTest::StateTest()" << std::endl;
}

GameState::Status StateTest::init(GameEngine *engine)
{
	std::cout << "StateTest::init()" << std::endl;
	return OK;
}

void StateTest::cleanup(GameEngine *engine)
{
	std::cout << "StateTest::cleanup()" << std::endl;
}

GameState::Status StateTest::resume(GameEngine *engine)
{
	std::cout << "StateTest::resume()" << std::endl;
	return OK;
}

GameState::Status StateTest::pause(GameEngine *engine)
{
	std::cout << "StateTest::pause()" << std::endl;
	return OK;
}

void StateTest::run(GameEngine *engine)
{
	static int i = 0;
	i++;
	if(i > 10)
	    engine->queue_pop();

	std::cout << "StateTest::run()" << std::endl;
}

void StateTest::event(GameEngine *engine, SDL_Event *e)
{
	std::cout << "StateTest::event()" << std::endl;
}

StateTest *StateTest::instance()
{
	if(instance_ == NULL)
		instance_ = new StateTest();
	return instance_;
}
