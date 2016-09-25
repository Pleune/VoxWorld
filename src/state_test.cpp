#include "state_test.hpp"

#include <SDL.h>
#include "state_window.hpp"
#include "logger.hpp"
#include "textbox.hpp"

StateTest *StateTest::instance_ = NULL;
StateTest::StateTest()
    :fps_limit(60)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::StateTest()" << Logger::MessageStream::endl;
}

GameState::Status StateTest::init()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::init()" << Logger::MessageStream::endl;

    SDL_Color color = {0,255,0,0};
    text = new Textbox(10, 10, 400, 30, Textbox::ROBOTO_REGULAR, Textbox::MEDIUM, color, "Press ESC to quit.", Textbox::NONE);
    world = new World();

    fps_limit.mark();

	return OK;
}

void StateTest::cleanup()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::cleanup()" << Logger::MessageStream::endl;
    delete text;
    delete world;
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
    if(queue.exit)
        engine->finish();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    world->render();
    text->render();

    StateWindow::instance()->swap();

    fps_limit.delay();
}

void StateTest::event(SDL_Event *e)
{
    if(e->type == SDL_KEYDOWN)
    {
        switch(e->key.keysym.sym)
        {
        case SDLK_ESCAPE:
            queue.exit = true;
            break;
        }
    }

    if(e->type == SDL_WINDOWEVENT)
    {
        if(e->window.event == SDL_WINDOWEVENT_RESIZED)
        {
            StateWindow::instance()->update_size();
            world->update_window_size();
        }
    }
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
