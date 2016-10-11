#ifndef BLOCKS_WORLDGEN_H
#define BLOCKS_WORLDGEN_H

#include "chunk.hpp"

namespace WorldGen {
    typedef void (*GenFunc)(Chunk *chunk, Chunk *chunkabove, Chunk *chunkbelow);

    void normal(Chunk *chunk, Chunk *chunkabove, Chunk *chunkbelow);
};

#endif
