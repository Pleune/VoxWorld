#ifndef BLOCKS_CHUNK_H
#define BLOCKS_CHUNK_H

#include "glbufferraw.hpp"
#include "voxeltree.hpp"
#include "block.hpp"
#include <GL/glew.h>
#include <mutex>

class Chunk {
public:
    enum Status {OK, FAIL};
    enum LockType {READ, WRITE};

    Chunk(long x, long y, long z);

    void render();
    long num_vertices();

    void remesh();

    Block::ID get(int x, int y, int z);
    void set(int x, int y, int z, Block::ID id);

    void lock(LockType);
    void unlock();

    static Status init();
    static void cleanup();

private:
    struct {
        long x;
        long y;
        long z;
    } pos;

    LockType lock_type;
    std::mutex lock_m;
    int lock_num;

    GLBufferRaw mesh;
    VoxelTree<Block::ID, 2> data;

    long num_vertices_;

    static GLuint static_index_elements[2];
    static int side_len;
    static int side_len_p1;
};

#endif
