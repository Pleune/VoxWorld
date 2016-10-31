#ifndef BLOCKS_CHUNKGEN_DSQUARE_H
#define BLOCKS_CHUNKGEN_DSQUARE_H

#include "chunkgen.hpp"
#include <mutex>

class ChunkGeneratorDSquare : public ChunkGenerator {
public:
    ChunkGeneratorDSquare();
    ~ChunkGeneratorDSquare();

    void generate(Chunk *);

private:
    template<int levels>
    class Heightmap {
    public:
        Heightmap(int x, int y);
        float get(int x, int y);

    private:
        float &at(int x, int y);
        float seed(int x, int y);

        int pound_level = 0;
        int level = 0;
        float *data;
    };

    int chunk_width;
    int heightmap_width;
};

#endif
