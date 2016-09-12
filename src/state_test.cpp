#include "state_test.hpp"

#include <SDL.h>
#include "logger.hpp"
#include "state_close.hpp"

StateTest *StateTest::instance_ = NULL;
StateTest::StateTest()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::StateTest()" << Logger::MessageStream::endl;
}

GameState::Status StateTest::init()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::init()" << Logger::MessageStream::endl;
	return OK;
}

void StateTest::cleanup()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::cleanup()" << Logger::MessageStream::endl;
}

GameState::Status StateTest::resume()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::resume()" << Logger::MessageStream::endl;
	return OK;
}

GameState::Status StateTest::pause()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::pause()" << Logger::MessageStream::endl;
	return OK;
}

void StateTest::run(GameEngine *engine)
{
	static int i = 0;
	i++;
	if(i > 10)
        engine->queue_change(StateClose::instance());

	Logger::stdout.log(Logger::DEBUG) << "StateTest::run()" << Logger::MessageStream::endl;

    SDL_Delay(300);
}

void StateTest::event(SDL_Event *e)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::event()" << Logger::MessageStream::endl;
}

std::string StateTest::getname()
{
    return "test";
}

StateTest *StateTest::instance()
{
	if(instance_ == NULL)
		instance_ = new StateTest();
	return instance_;
}
