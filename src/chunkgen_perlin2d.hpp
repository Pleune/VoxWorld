#ifndef BLOCKS_CHUNKGEN_PERLIN2D_H
#define BLOCKS_CHUNKGEN_PERLIN2D_H

#include "chunkgen.hpp"
#include <mutex>

class ChunkGeneratorPerlin2D : public ChunkGeneratorHeightmap {
public:
    void gen_map(Heightmap &map);
};

#endif
