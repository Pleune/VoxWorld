#ifndef BLOCKS_STATE_TEST_H
#define BLOCKS_STATE_TEST_H

#include "gamestate.hpp"
#include "gameengine.hpp"
#include "world.hpp"
#include "limiter.hpp"

class Textbox;

class StateTest : public GameState {
public:
	Status init();
	void cleanup();
	Status resume();
	Status pause();

    void input(uint32_t dt);

	void run(GameEngine *);
	void event(SDL_Event *e);

    std::string getname();

	static StateTest *instance();

private:
	StateTest();

    struct {
        bool exit = false;
    } queue;

    bool takeinput = true;
    Camera camera;
    struct {float x,y;} rot;

    Textbox *text;
    Textbox *fps;
    World *world;
    const Uint8 *keyboard;

    Limiter fps_limit;
    uint32_t tick;

    static StateTest *instance_;

};

#endif
