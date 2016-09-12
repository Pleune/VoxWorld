#include "state_test.hpp"

#include <SDL.h>
#include "state_window.hpp"
#include "logger.hpp"
#include "textbox.hpp"

StateTest *StateTest::instance_ = NULL;
StateTest::StateTest()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::StateTest()" << Logger::MessageStream::endl;
}

GameState::Status StateTest::init()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::init()" << Logger::MessageStream::endl;

    SDL_Color color = {0,255,0,0};
    text = new Textbox(10, 10, 400, 30, Textbox::ROBOTO_REGULAR, Textbox::MEDIUM, color, "Press ESC to quit.", Textbox::NONE);

	return OK;
}

void StateTest::cleanup()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::cleanup()" << Logger::MessageStream::endl;
    delete text;
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

    static int r=0;
    static int g=0;
    static int b=0;

    static int r_sign = 1;
    static int g_sign = 1;
    static int b_sign = 1;

    r += r_sign*2;
    g += g_sign*3;
    b += b_sign*5;

    if(r > 255) { r=255; r_sign = -1; }
    if(g > 255) { g=255; g_sign = -1; }
    if(b > 255) { b=255; b_sign = -1; }

    if(r < 0) { r=0; r_sign = 1; }
    if(g < 0) { g=0; g_sign = 1; }
    if(b < 0) { b=0; b_sign = 1; }

    SDL_Color c = {(Uint8)r, (Uint8)g, (Uint8)b, 0};

    text->set_color(c);
    text->render();
    StateWindow::instance()->swap();

    SDL_Delay(25);
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
