#include "state_initalize.hpp"

#include "logger.hpp"
#include "textbox.hpp"
#include "state_test.hpp"

StateInitalize *StateInitalize::instance_ = NULL;
StateInitalize::StateInitalize()
{
}

GameState::Status StateInitalize::init()
{
	return OK;
}

void StateInitalize::cleanup()
{
}

GameState::Status StateInitalize::resume()
{
	return OK;
}

GameState::Status StateInitalize::pause()
{
	return OK;
}

void StateInitalize::run(GameEngine *engine)
{
    engine->queue_change(StateTest::instance());
}

void StateInitalize::event(SDL_Event *e)
{
}

std::string StateInitalize::getname()
{
    return "initalize";
}

StateInitalize *StateInitalize::instance()
{
	if(instance_ == NULL)
		instance_ = new StateInitalize();
	return instance_;
}
