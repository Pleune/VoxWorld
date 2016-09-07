#include "gameengine.hpp"

#include <SDL.h>
#include <iostream>

GameEngine::GameEngine(GameState *inital_state)
{
    state_stack.push_back(inital_state);
    current_state = inital_state;
    inital_state->init(this);
	inital_state->instances_inc();
}

void GameEngine::queue_push(GameState *state)
{
	queue_state_action = PUSH;
	queue_state = state;
}

void GameEngine::queue_pop()
{
	queue_state_action = POP;
}

void GameEngine::queue_change(GameState *state)
{
	queue_state_action = CHANGE;
	queue_state = state;
}

void GameEngine::push(GameState *state)
{
	GameState::Status ret;

    ret = current_state->pause(this);

	if(ret != GameState::Status::OK)
		return;

    state_stack.push_back(state);
    current_state = state;

    if(state->instances() > 0)
	{
        ret = state->resume(this);
    } else {
        ret = state->init(this);
	}

	while(ret != GameState::Status::OK)
	{
		if(state_stack.empty())
		{
			done = true;
			return;
		}

		//TODO: log this!
		state_stack.pop_back();
		current_state = state_stack.back();
		current_state->resume(this);
	}

	state->instances_inc();
}

void GameEngine::pop()
{
    if(current_state->instances() > 1)
        current_state->pause(this);
    else
        current_state->cleanup(this);
    current_state->instances_dec();

    state_stack.pop_back();

    if(state_stack.empty())
        done = true;
    else
        current_state = state_stack.back();
}

void GameEngine::change(GameState *state)
{
	GameState::Status ret = GameState::Status::OK;

    if(current_state->instances() > 1)
	{
        ret = current_state->pause(this);
    } else {
        current_state->cleanup(this);
	}

	if(ret != GameState::Status::OK)
		return;

    current_state->instances_dec();

    state_stack.pop_back();
    state_stack.push_back(state);
    current_state = state;

    if(state->instances() > 0)
	{
        ret = state->resume(this);
    } else {
        ret = state->init(this);
	}

	while(ret != GameState::Status::OK)
	{
		if(state_stack.empty())
		{
			done = true;
			return;
		}

		//TODO: log this!
		state_stack.pop_back();
		current_state = state_stack.back();
		current_state->resume(this);
	}

	state->instances_inc();
}

bool GameEngine::run_once()
{
	switch(queue_state_action)
	{
	case PUSH:
		push(queue_state);
		queue_state_action = NONE;
		break;
	case POP:
		pop();
		queue_state_action = NONE;
		break;
	case CHANGE:
		change(queue_state);
		queue_state_action = NONE;
		break;
	case NONE:
		flush_events(current_state);
		current_state->run(this);
		break;
	}

	return done;
}

void GameEngine::loop()
{
	while(!done)
		run_once();
}

int GameEngine::flush_events(GameState *state)
{
	int i = 0;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if(event_hook)
			event_hook(&e);

		if(e.type == SDL_QUIT)
			done = true;

		state->event(this, &e);

		i++;
	}

	return i;
}
