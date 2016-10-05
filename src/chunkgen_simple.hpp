#ifndef BLOCKS_CHUNKGEN_FLAT_H
#define BLOCKS_CHUNKGEN_FLAT_H

#include "chunkgen.hpp"

class ChunkGeneratorFlat : public ChunkGenerator {
public:
    void generate(Chunk *);
};

class ChunkGeneratorRandom : public ChunkGenerator {
public:
    void generate(Chunk *);
};

class ChunkGeneratorCrapHills : public ChunkGenerator {
public:
    void generate(Chunk *);
};

#endif
