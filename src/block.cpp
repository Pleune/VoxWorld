#include "block.hpp"

const Block::Properties Block::properties_table[NUM_BLOCK_TYPES] = {
    [AIR]   = {{  0,  0,  0,  0}, false},
    [STONE] = {{ 76, 76, 76,255}, true},
    [GRASS] = {{ 13, 69, 25,255}, true},
    [DIRT]  = {{ 47, 23, 13,255}, true},
    [WATER] = {{ 20, 20, 76,100}, true},
    [SAND]  = {{ 71, 76, 38,255}, true},
};
