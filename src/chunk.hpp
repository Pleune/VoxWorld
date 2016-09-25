#ifndef BLOCKS_CHUNK_H
#define BLOCKS_CHUNK_H

#include "glbufferraw.hpp"
#include "voxeltree.hpp"
#include "block.hpp"
#include <GL/glew.h>

class Chunk {
public:
    enum Status {OK, FAIL};

    Chunk(long x, long y, long z);

    void render();
    long num_vertices();

    void remesh();

    Block::ID get(int x, int y, int z);
    void set(int x, int y, int z, Block::ID id);

    static Status init();
    static void cleanup();

private:
    struct {
        long x;
        long y;
        long z;
    } pos;

    GLBufferRaw mesh;
    VoxelTree<Block::ID, 2> data;

    long num_vertices_;

    static GLuint static_index_elements[2];
    static int side_len;
    static int side_len_p1;
};

#endif