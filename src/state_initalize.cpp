#include "state_initalize.hpp"

#include <functional>
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
    Logger::stdout.log(Logger::DEBUG) << "rand() seeded from time" << Logger::MessageStream::endl;
    TTF_Init();
    Logger::stdout.log(Logger::DEBUG) << "SDL_ttf initalized" << Logger::MessageStream::endl;
    Textbox::init();
    Logger::stdout.log(Logger::DEBUG) << "Textbox initaized" << Logger::MessageStream::endl;
	return OK;
}

void StateInitalize::cleanup()
{
    TTF_Quit();
    Logger::stdout.log(Logger::DEBUG) << "SDL_ttf de-initaized" << Logger::MessageStream::endl;
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
