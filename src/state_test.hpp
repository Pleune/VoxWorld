#ifndef BLOCKS_STATE_TEST_H
#define BLOCKS_STATE_TEST_H

#include "gamestate.hpp"
#include "gameengine.hpp"

class StateTest : public GameState {
public:
	Status init();
	void cleanup();
	Status resume();
	Status pause();

	void run(GameEngine *);
	void event(SDL_Event *e);

    std::string getname();

	static StateTest *instance();

private:
	StateTest();
	static StateTest *instance_;
};

#endif
