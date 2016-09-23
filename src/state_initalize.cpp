#include "state_initalize.hpp"

#include <SDL_ttf.h>
#include "logger.hpp"
#include "textbox.hpp"
#include "state_test.hpp"
#include "chunk.hpp"

StateInitalize *StateInitalize::instance_ = NULL;
StateInitalize::StateInitalize()
{
}

GameState::Status StateInitalize::init()
{
    TTF_Init();
    Textbox::init();
    Logger::stdout.log(Logger::DEBUG) << "Textbox initaized" << Logger::MessageStream::endl;
    Chunk::init();
    Logger::stdout.log(Logger::DEBUG) << "Chunk initaized" << Logger::MessageStream::endl;
	return OK;
}

void StateInitalize::cleanup()
{
    Textbox::cleanup();
    Logger::stdout.log(Logger::DEBUG) << "Textbox de-initaized" << Logger::MessageStream::endl;
    TTF_Quit();
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
    engine->queue_push(StateTest::instance());
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
