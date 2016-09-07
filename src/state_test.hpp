#ifndef BLOCKS_STATE_TEST_H
#define BLOCKS_STATE_TEST_H

#include "gamestate.hpp"
#include "gameengine.hpp"

class StateTest : public GameState {
public:
	Status init(GameEngine *engine);
	void cleanup(GameEngine *engine);
	Status resume(GameEngine *engine);
	Status pause(GameEngine *engine);

	void run(GameEngine *engine);
	void event(GameEngine *engine, SDL_Event *e);

	static StateTest *instance();

private:
	StateTest();
	static StateTest *instance_;
};

#endif
