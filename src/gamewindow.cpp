#include "gamewindow.hpp"

#include <SDL.h>
#include <GL/glew.h>
#include "logger.hpp"

GameWindow::GameWindow()
{
	Logger::stdout.log(Logger::DEBUG) << "GameWindow::GameWindow()" << Logger::MessageStream::endl;

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
}

GameWindow::~GameWindow()
{
	Logger::stdout.log(Logger::DEBUG) << "GameWindow::cleanup()" << Logger::MessageStream::endl;

    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void GameWindow::resize_context()
{
    SDL_GetWindowSize(win, &windoww, &windowh);
    glViewport(0,0, windoww, windowh);
}
