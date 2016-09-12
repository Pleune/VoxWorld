#ifndef BLOCKS_STATE_CLOSE_H
#define BLOCKS_STATE_CLOSE_H

#include "gamestate.hpp"
#include "gameengine.hpp"

class StateClose : public GameState {
public:
	Status init();
	void cleanup();
	Status resume();
	Status pause();

	void run(GameEngine *);
	void event(SDL_Event *e);

    std::string getname();

	static StateClose *instance();

private:
	StateClose();
	static StateClose *instance_;
};

#endif
