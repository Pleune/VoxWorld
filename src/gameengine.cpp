#include "gameengine.hpp"

#include <SDL.h>
#include <iostream>
#include "logger.hpp"

GameEngine::GameEngine(GameState *inital_state)
{
    Logger::stdout.log(Logger::DEBUG) << "GameEngine::GameEngine(): pushing state \"" << inital_state->getname() << "\" onto stack" << Logger::MessageStream::endl;

    state_stack.push_back(inital_state);
    current_state = inital_state;
    inital_state->init();
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
    Logger::stdout.log(Logger::DEBUG) << "GameEngine::push(): pushing state " << state->getname() << " onto stack" << Logger::MessageStream::endl;

	GameState::Status ret;

    ret = current_state->pause();

	if(ret != GameState::Status::OK)
		return;

    state_stack.push_back(state);
    current_state = state;

    if(state->instances() > 0)
	{
        ret = state->resume();
    } else {
        ret = state->init();
	}

	while(ret != GameState::Status::OK)
	{
		if(state_stack.empty())
		{
			done = true;
			return;
		}

        Logger::stdout.log(Logger::DEBUG) << "GameEngine::push(): resume/init failed! poping failed state "<< current_state->getname() << Logger::MessageStream::endl;
		state_stack.pop_back();
		current_state = state_stack.back();
		current_state->resume();
	}

	state->instances_inc();
}

void GameEngine::pop()
{
    Logger::stdout.log(Logger::DEBUG) << "GameEngine::pop(): poping state " << current_state->getname() << " off stack" << Logger::MessageStream::endl;

    if(current_state->instances() > 1)
        current_state->pause();
    else
        current_state->cleanup();
    current_state->instances_dec();

    state_stack.pop_back();

    if(state_stack.empty())
        done = true;
    else
        current_state = state_stack.back();
}

void GameEngine::change(GameState *state)
{
    Logger::stdout.log(Logger::DEBUG) << "GameEngine::change(): changing from state " << current_state->getname() << " to state " << state->getname() <<  Logger::MessageStream::endl;
	GameState::Status ret = GameState::Status::OK;

    if(current_state->instances() > 1)
	{
        ret = current_state->pause();
    } else {
        current_state->cleanup();
	}

	if(ret != GameState::Status::OK)
		return;

    current_state->instances_dec();

    state_stack.pop_back();
    state_stack.push_back(state);
    current_state = state;

    if(state->instances() > 0)
	{
        ret = state->resume();
    } else {
        ret = state->init();
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
		current_state->resume();
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

    cleanup();
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

        if(e.type == SDL_WINDOWEVENT)
        {
            if(e.window.event == SDL_WINDOWEVENT_RESIZED)
                window.resize_context();
        }

		state->event(&e);

		i++;
	}

	return i;
}

void GameEngine::cleanup()
{
    while(!state_stack.empty())
        pop();
}

void GameEngine::reset(GameState *state)
{
    cleanup();

    Logger::stdout.log(Logger::DEBUG) << "GameEngine::reset() pushing state \"" << current_state->getname() << "\" onto stack" << Logger::MessageStream::endl;

    state_stack.push_back(state);
    current_state = state;
    current_state->init();
    current_state->instances_inc();
    done = false;
}
