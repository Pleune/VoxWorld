#ifndef BLOCKS_STATE_WINDOW_H
#define BLOCKS_STATE_WINDOW_H

#include "gamestate.hpp"
#include "gameengine.hpp"

#include <SDL.h>

class StateWindow : public GameState {
public:
	Status init();
	void cleanup();
	Status resume();
	Status pause();

	void run(GameEngine *);
	void event(SDL_Event *e);

    std::string getname();

    void update_size();
    void swap();
    void get_dimensions(int *windoww, int *windowh);

    SDL_GLContext create_shared_gl_context();
    void make_gl_context_current(SDL_GLContext);

	static StateWindow *instance();

private:
	StateWindow();

    SDL_Window *win;
    SDL_GLContext glcontext;

    int windoww, windowh;

	static StateWindow *instance_;
};

#endif
