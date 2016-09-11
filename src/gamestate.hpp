#ifndef BLOCKS_GAMESTATE_H
#define BLOCKS_GAMESTATE_H

#include <string>
#include <SDL.h>

class GameEngine;

class GameState {
public:
	enum Status {ERR, OK};

    virtual Status init() = 0;
    virtual void cleanup() = 0;
    virtual Status resume() = 0;
    virtual Status pause() = 0;

    virtual void run(GameEngine *) = 0;
    virtual void event(SDL_Event *) = 0;

    virtual std::string getname() = 0;

    void instances_inc() {instances_++;}
    void instances_dec() {instances_--;}
    int instances() {return instances_;}

private:
    int instances_ = 0;
};

#endif
