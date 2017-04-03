#include "limiter.hpp"

Limiter::Limiter(int cps)
    :cps(cps), count_last(0)
{
    mark();
}

void Limiter::set_cps(int cps)
{
    this->cps = cps;
    mark();
}

void Limiter::delay()
{
    last_delay++;
    Uint32 next_ms = last_sec + (1000.0*last_delay/(double)cps);
    Uint32 this_ms = SDL_GetTicks();

    count++;
    if(count_time +1000 < this_ms)
    {
        count_last = count;
        count = 0;
        count_time += 1000;
    }

    if(last_delay == cps)
    {
        last_sec = this_ms;
        last_delay = 1;//not zero because this delay is delay num 0.
    }

    if(next_ms > this_ms)
        SDL_Delay(next_ms - this_ms);
}

void Limiter::nodelay()
{
    Uint32 this_ms = SDL_GetTicks();

    count++;
    if(count_time +1000 < this_ms)
    {
        count_last = count;
        count = 0;
        count_time += 1000;
    }
}

void Limiter::mark()
{
    last_sec = SDL_GetTicks();
    last_delay = 0;
    count_time = last_sec;
    count = 0;
}
