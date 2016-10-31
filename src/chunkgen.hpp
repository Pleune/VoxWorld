#ifndef BLOCKS_CHUNKGEN_H
#define BLOCKS_CHUNKGEN_H

#include "chunk.hpp"

class ChunkGenerator {
public:
    virtual void generate(Chunk *) = 0;
    virtual ~ChunkGenerator() {}
};

class ChunkGeneratorHeightmap : public ChunkGenerator {
public:
    virtual void generate(Chunk *);
    ChunkGeneratorHeightmap();
    virtual ~ChunkGeneratorHeightmap();

protected:
    struct Heightmap {
        long3_t pos;
        float *data;
        int width;

        float get(int x, int z);
        void set(int x, int y, float val);
    };

    virtual void gen_map(Heightmap &map) = 0;

    std::mutex history_m;
    Heightmap history[32];
    int history_index = 0;
    int c_width;
};

#include "chunkgen_simple.hpp"
#include "chunkgen_perlin.hpp"
#include "chunkgen_perlin2d.hpp"
#include "chunkgen_dsquare.hpp"

#endif
