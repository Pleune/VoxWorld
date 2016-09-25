#ifndef BLOCKS_WORLD_H
#define BLOCKS_WORLD_H

#include <unordered_map>
#include "chunk.hpp"

class World {
public:
    World();
    ~World();

    void render();

    void update_window_size();

private:
    static unsigned long hash_cpos(long3_t l)
    {
        return std::hash<long>{}(l.x) ^ std::hash<long>{}(l.y) ^ std::hash<long>{}(l.z);
    }

    static bool compare_cpos(long3_t a, long3_t b)
    {
        return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
    }

    typedef std::unordered_map<
        long3_t,
        Chunk *,
        std::function<unsigned long(long3_t)>,
        std::function<bool(long3_t, long3_t)>>
        ChunkMap;

    ChunkMap chunks;

    GLuint pre_program;
    GLuint pre_uniform_modelmatrix;
	GLuint pre_uniform_viewprojectionmatrix;

    GLuint post_program;
	GLuint post_uniform_tex;
	GLuint post_uniform_depth;
    GLuint post_uniform_window_size;
    GLuint post_buffer_target;

    struct {
        GLuint framebuffer;

        GLuint colorbuffer;
        GLuint depthbuffer;

        GLuint colorbufferid;
        GLuint depthbufferid;
    } renderbuffer;
};

#endif
