#ifndef BLOCKS_GAMESTATE_H
#define BLOCKS_GAMESTATE_H

#include <SDL.h>

class GameEngine;

class GameState {
public:
	enum Status {ERR, OK};

    virtual Status init(GameEngine *engine) = 0;
    virtual void cleanup(GameEngine *engine) = 0;
    virtual Status resume(GameEngine *engine) = 0;
    virtual Status pause(GameEngine *engine) = 0;

    virtual void run(GameEngine *engine) = 0;
    virtual void event(GameEngine *engine, SDL_Event *e) = 0;

    void instances_inc() {instances_++;}
    void instances_dec() {instances_--;}
    int instances() {return instances_;}

private:
    int instances_ = 0;
};

#endif
