#include "state_test.hpp"

#include "logger.hpp"

StateTest *StateTest::instance_ = NULL;
StateTest::StateTest()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::StateTest()" << Logger::MessageStream::endl;
}

GameState::Status StateTest::init(GameEngine *engine)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::init()" << Logger::MessageStream::endl;
	return OK;
}

void StateTest::cleanup(GameEngine *engine)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::cleanup()" << Logger::MessageStream::endl;
}

GameState::Status StateTest::resume(GameEngine *engine)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::resume()" << Logger::MessageStream::endl;
	return OK;
}

GameState::Status StateTest::pause(GameEngine *engine)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::pause()" << Logger::MessageStream::endl;
	return OK;
}

void StateTest::run(GameEngine *engine)
{
	static int i = 0;
	i++;
	if(i > 10)
	    engine->queue_pop();

	Logger::stdout.log(Logger::DEBUG) << "StateTest::run()" << Logger::MessageStream::endl;
}

void StateTest::event(GameEngine *engine, SDL_Event *e)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::event()" << Logger::MessageStream::endl;
}

StateTest *StateTest::instance()
{
	if(instance_ == NULL)
		instance_ = new StateTest();
	return instance_;
}
