#ifndef BLOCKS_GAMEENGINE_H
#define BLOCKS_GAMEENGINE_H

#include <vector>
#include "gamewindow.hpp"
#include "gamestate.hpp"

class GameEngine {
public:
    GameEngine(GameState *inital_state);

	typedef void (*event_hook_func)(SDL_Event *);
	void set_event_hook(event_hook_func);

    void queue_push(GameState *state);
    void queue_pop();
    void queue_change(GameState *state);

    bool run_once();
    void loop();

    void cleanup();
    void reset(GameState *state);

private:
	void push(GameState *state);
	void pop();
	void change(GameState *state);

	int flush_events(GameState *state);

    GameWindow window;

	/*
	 * Instead of directly allowing push/pop/change from
     * any outside caller, queue the changes instead.
     * This serves two purposes:
	 *  -Thread saftey
	 *  -A state can only queue one operation, so it
	 *   does not have the ability to effect other states.
     */
	enum {NONE, PUSH, POP, CHANGE} queue_state_action = NONE;
	GameState *queue_state = NULL;

	event_hook_func event_hook = 0;

    bool done = false;
    GameState *current_state;
    std::vector<GameState *> state_stack;
};

#endif
