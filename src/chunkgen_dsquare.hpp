#ifndef BLOCKS_CHUNKGEN_DSQUARE_H
#define BLOCKS_CHUNKGEN_DSQUARE_H

#include "chunkgen.hpp"
#include <mutex>

class ChunkGeneratorDSquare : public ChunkGenerator {
public:
    ChunkGeneratorDSquare();
    ~ChunkGeneratorDSquare();

    void generate(Chunk *);

    bool lod(int level, LODMesh *ret, long3_t cpos);
    int lod_max() {return 1;};

private:
    class Heightmap {
    public:
        Heightmap(int levels, int x, int y);
        float get(float x, float y);
        static float seed(int x, int y);

    private:
        float &at(int x, int y);

        int levels;
        int side_len;
        float *data;
    };

    void bias(double &h);

    int hmap_levels;
    int chunk_width;
    int heightmap_width;
};

#endif
