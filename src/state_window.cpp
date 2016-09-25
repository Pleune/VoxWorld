#include "state_window.hpp"

#include <SDL.h>
#include <GL/glew.h>
#include "logger.hpp"
#include "state_initalize.hpp"

StateWindow *StateWindow::instance_ = NULL;
StateWindow::StateWindow()
{
}

GameState::Status StateWindow::init()
{
	Logger::stdout.log(Logger::DEBUG) << "GameWindow::GameWindow() creating window w/ gl context" << Logger::MessageStream::endl;

	if(SDL_Init(SDL_INIT_EVERYTHING))
        Logger::stdout.log(Logger::ERROR) << "GameWindow::GameWindow(): SDL_Init failed: (" << SDL_GetError() << ')' << Logger::MessageStream::endl;

    win = SDL_CreateWindow("VoxWorld", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    if(win == NULL)
        Logger::stdout.log(Logger::ERROR) << "GameWindow::GameWindow(): SDL_CreateWindow failed: (" << SDL_GetError() << ')' << Logger::MessageStream::endl;

    glcontext = SDL_GL_CreateContext(win);
    if(glcontext == NULL)
        Logger::stdout.log(Logger::ERROR) << "GameWindow::GameWindow(): SDL_GL_CreateContext failed: (" << SDL_GetError() << ')' << Logger::MessageStream::endl;

    if(glewInit() != GLEW_OK)
        Logger::stdout.log(Logger::ERROR) << "GameWindow::GameWindow(): glewInit failed" << Logger::MessageStream::endl;

    SDL_GetWindowSize(win, &windoww, &windowh);
    SDL_GL_SetSwapInterval(0);

    glClearColor(0,0,0,1);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(win);

	return OK;
}

void StateWindow::cleanup()
{
	Logger::stdout.log(Logger::DEBUG) << "GameWindow::cleanup() destroying window" << Logger::MessageStream::endl;

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    win = NULL;
}

GameState::Status StateWindow::resume()
{
	return OK;
}

GameState::Status StateWindow::pause()
{
	return OK;
}

void StateWindow::run(GameEngine *engine)
{
    //TODO: set engine event hook for window resize
    engine->queue_push(StateInitalize::instance());
}

void StateWindow::event(SDL_Event *e)
{
    if(e->type == SDL_WINDOWEVENT)
    {
        if(e->window.event == SDL_WINDOWEVENT_RESIZED)
            update_size();
    }
}

std::string StateWindow::getname()
{
    return "window";
}

StateWindow *StateWindow::instance()
{
	if(instance_ == NULL)
		instance_ = new StateWindow();
	return instance_;
}

void StateWindow::update_size()
{
    SDL_GetWindowSize(win, &windoww, &windowh);
    glViewport(0,0, windoww, windowh);
}

void StateWindow::swap()
{
    SDL_GL_SwapWindow(win);
}

void StateWindow::get_dimensions(int *windoww, int *windowh)
{
    *windoww = this->windoww;
    *windowh = this->windowh;
}
