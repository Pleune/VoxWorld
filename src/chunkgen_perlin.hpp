#ifndef BLOCKS_CHUNKGEN_PERLIN_H
#define BLOCKS_CHUNKGEN_PERLIN_H

#include "chunkgen.hpp"
#include <mutex>

class ChunkGeneratorPerlin : public ChunkGenerator {
public:
    ChunkGeneratorPerlin();
    ~ChunkGeneratorPerlin();

    void generate(Chunk *);

private:
    int c_width;
};

#endif
