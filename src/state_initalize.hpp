#ifndef BLOCKS_STATE_INIT_H
#define BLOCKS_STATE_INIT_H

#include "gamestate.hpp"
#include "gameengine.hpp"

class StateInitalize : public GameState {
public:
	Status init();
	void cleanup();
	Status resume();
	Status pause();

	void run(GameEngine *);
	void event(SDL_Event *e);

    std::string getname();

	static StateInitalize *instance();

private:
	StateInitalize();
	static StateInitalize *instance_;
};

#endif
