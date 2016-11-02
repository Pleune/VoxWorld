#ifndef BLOCKS_CHUNK_H
#define BLOCKS_CHUNK_H

#include "glbufferraw.hpp"
#include "voxeltree.hpp"
#include "block.hpp"
#include <GL/glew.h>
#include <mutex>
#include <atomic>
#include "custommath.h"

class Chunk {
public:
    enum Status {OK, FAIL};
    enum LockType {READ, WRITE};

    Chunk(long x, long y, long z);

    void render(vec3_t camera_pos);
    long num_vertices();

    void remesh(Chunk *chunkabove, Chunk *chunkbelow, Chunk *chunknorth, Chunk *chunksouth, Chunk *chunkeast, Chunk *chunkwest);
    bool meshed() {return has_mesh;}
    void force_mesh_upload();

    Block::ID get(int x, int y, int z);
    void set(int x, int y, int z, Block::ID id);
    void fill(Block::ID block)
    {
        data.fill(block);
    }

    long3_t cpos() {return pos;}

    void lock(LockType);
    void unlock();

    static Status init(GLuint draw_program);
    static void cleanup();

    static int size() {return side_len;}

    void lock_delete() {dont_delete++;}
    void unlock_delete() {dont_delete--;}
    bool can_delete() {return dont_delete == 0;}
    int delete_count() {return dont_delete;};

    /* Level 0: nothing done
     * Level 1: sent to WorldGenerator
     * Level 2: WorldGenerator done
     * Level 3: Sent to WorldGen
     * Level 4: WorldGen done
     */
    int gen_level() {return gen_level_;}
    void gen_inc() {gen_level_++;}

private:
    long3_t pos;

    LockType lock_type;
    std::mutex lock_m;
    std::atomic<int> lock_num;

    GLBufferRaw mesh;
    VoxelTree<Block::ID, 5> data;

    long num_vertices_ = 0;
    bool has_mesh = false;
    std::atomic<int> dont_delete;

    int gen_level_ = 0;

    static GLuint static_index_elements[2];
    static int side_len;
    static int side_len_p1;
    static int side_len_m1;

    static GLuint draw_program;
    static GLuint draw_uniform_modelmatrix;
};

#endif
