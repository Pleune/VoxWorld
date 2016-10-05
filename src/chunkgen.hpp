#ifndef BLOCKS_CHUNKGEN_H
#define BLOCKS_CHUNKGEN_H

#include "chunk.hpp"

class ChunkGenerator {
public:
    virtual void generate(Chunk *) = 0;
    virtual ~ChunkGenerator() {}
};

#include "chunkgen_simple.hpp"

#endif
