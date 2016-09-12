#include "state_close.hpp"

#include "logger.hpp"
#include "textbox.hpp"

StateClose *StateClose::instance_ = NULL;
StateClose::StateClose()
{
}

GameState::Status StateClose::init()
{
	return OK;
}

void StateClose::cleanup()
{
}

GameState::Status StateClose::resume()
{
	return OK;
}

GameState::Status StateClose::pause()
{
	return OK;
}

void StateClose::run(GameEngine *engine)
{
    engine->queue_pop();
}

void StateClose::event(SDL_Event *e)
{
}

std::string StateClose::getname()
{
    return "close";
}

StateClose *StateClose::instance()
{
	if(instance_ == NULL)
		instance_ = new StateClose();
	return instance_;
}
