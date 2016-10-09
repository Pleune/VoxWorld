#ifndef BLOCKS_CHUNKGEN_PERLIN_H
#define BLOCKS_CHUNKGEN_PERLIN_H

#include "chunkgen.hpp"

class ChunkGeneratorPerlin : public ChunkGenerator {
public:
    void generate(Chunk *);
};

#endif
