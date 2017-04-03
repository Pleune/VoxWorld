#ifndef BLOCKS_LIMITER_H
#define BLOCKS_LIMITER_H

#include <SDL_timer.h>

/*
 * The Goal of this limiter class to to stay accurate even
 * when the cycle delta is small (less than say 3-5 ms).
 * This will be true even though the SDL_GetTicks() function
 * only has a resolution of 1ms.
 */

class Limiter {
public:
    Limiter(int cps);  //cycles per second

    void set_cps(int cps);

    void delay();      //delay until the next limit
    void nodelay();
    void mark();       //reset the seccond
    int speed() {return count_last;}

private:
    int last_delay;    //cycle number (0 to cps-1)
    Uint32 last_sec;   //tick of last full seccond
    int cps;           //target cycles per second
    int count;
    int count_last;
    uint32_t count_time;
};

#endif
