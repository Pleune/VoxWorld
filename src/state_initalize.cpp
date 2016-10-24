#include "state_initalize.hpp"

#include <cstdlib>
#include <ctime>
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
    std::srand(std::time(0));
    Logger::standard.log(Logger::LOG_DEBUG) << "rand() seeded from time" << Logger::MessageStream::endl;
    TTF_Init();
    Logger::standard.log(Logger::LOG_DEBUG) << "SDL_ttf initalized" << Logger::MessageStream::endl;
    Textbox::init();
    Logger::standard.log(Logger::LOG_DEBUG) << "Textbox initaized" << Logger::MessageStream::endl;
	return OK;
}

void StateInitalize::cleanup()
{
    TTF_Quit();
    Logger::standard.log(Logger::LOG_DEBUG) << "SDL_ttf de-initaized" << Logger::MessageStream::endl;
    Textbox::cleanup();
    Logger::standard.log(Logger::LOG_DEBUG) << "Textbox de-initaized" << Logger::MessageStream::endl;
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
