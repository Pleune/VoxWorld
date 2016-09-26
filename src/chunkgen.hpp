#ifndef BLOCKS_CHUNKGEN_H
#define BLOCKS_CHUNKGEN_H

#include "chunk.hpp"

namespace ChunkGen {
    typedef void (*GenFunc)(Chunk *);
    void random(Chunk *target);
};

#endif
