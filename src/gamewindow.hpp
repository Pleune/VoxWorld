#ifndef BLOCKS_GAMEWINDOW_H
#define BLOCKS_GAMEWINDOW_H

#include <SDL.h>

class GameWindow {
public:
    GameWindow();
    ~GameWindow();

    void resize_context();

private:
    SDL_Window *win;
    SDL_GLContext glcontext;

    int windoww, windowh;
};

#endif
