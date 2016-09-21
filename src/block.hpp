#ifndef BLOCKS_BLOCK_H
#define BLOCKS_BLOCK_H

#include <SDL.h>

#define NUM_BLOCK_TYPES 6

namespace Block {
    enum BlockType {
        AIR,
        STONE,
        GRASS,
        DIRT,
        WATER,
        SAND
    };

    struct Properties {
        SDL_Color color;
        bool is_solid;
    };

    typedef short ID;

    const extern Properties properties_table[NUM_BLOCK_TYPES];

    inline SDL_Color color(ID id) {return properties_table[id].color;}
    inline bool is_solid(ID id) {return properties_table[id].is_solid;}
};

#endif
