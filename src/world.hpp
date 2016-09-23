#ifndef BLOCKS_WORLD_H
#define BLOCKS_WORLD_H

#include "chunk.hpp"

class World {
public:
    World();
    ~World();

    void render();

private:
    Chunk *chunk;

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
