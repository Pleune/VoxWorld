#include "world.hpp"

#include <cstdlib>
#include "chunk.hpp"
#include "custommath.h"
#include "gl.h"
#include "state_window.hpp"
#include "logger.hpp"

World::World()
{
    int windoww, windowh;
    StateWindow::instance()->get_dimensions(&windoww, &windowh);

    //Load Programs
    char *basepath = SDL_GetBasePath();
    gl_program_load_file(&pre_program, "shaders/vs", "shaders/fs", basepath);
    gl_program_load_file(&post_program, "shaders/pvs", "shaders/pfs", basepath);
    free(basepath);

    //setup for pre_program
    pre_uniform_modelmatrix = glGetUniformLocation(pre_program, "MODEL");
	pre_uniform_viewprojectionmatrix = glGetUniformLocation(pre_program, "VP");
	post_uniform_tex = glGetUniformLocation(post_program, "tex");
	post_uniform_depth = glGetUniformLocation(post_program, "depth");
	post_uniform_window_size = glGetUniformLocation(post_program, "window_size");

    glUseProgram(pre_program);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glUseProgram(0);

    glUseProgram(post_program);
    glEnableVertexAttribArray(0);
    glUseProgram(0);

    //setup for post_program
    glGenFramebuffers(1, &renderbuffer.framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, renderbuffer.framebuffer);

    glGenTextures(1, &renderbuffer.colorbuffer);
    glGenTextures(1, &renderbuffer.depthbuffer);

    glBindTexture(GL_TEXTURE_2D, renderbuffer.colorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windoww, windowh, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, renderbuffer.depthbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windoww, windowh, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderbuffer.colorbuffer, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renderbuffer.depthbuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //put the textures in the right spots
    glUseProgram(post_program);
    glUniform1i(post_uniform_tex, 0);
    glUniform1i(post_uniform_depth, 1);
    glUseProgram(0);

    //generate the post processing mesh
    GLfloat mesh[] = {
        -1.0f, 1.0f,
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f,
        1.0f, -1.0f
    };

    glGenBuffers(1, &post_buffer_target);
    glBindBuffer(GL_ARRAY_BUFFER, post_buffer_target);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mesh) * sizeof(GLfloat), mesh, GL_STATIC_DRAW);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Logger::stdout.log(Logger::FATAL) << "Bad world post-process framebuffer" << Logger::MessageStream::endl;

    chunk = new Chunk(0,0,0);
    for(int i=0; i<100; i++)
    {
        int x = rand()%16;
        int y = rand()%16;
        int z = rand()%16;
        chunk->set(x,y,z, Block::GRASS);
    }
    chunk->remesh();

    GLenum glerr = glGetError();
    if(glerr != GL_NO_ERROR)
        Logger::stdout.log(Logger::FATAL) << "OpenGL error code " << glerr << " after World::World()"
                                          << Logger::MessageStream::endl;
}

World::~World()
{
    glDeleteProgram(pre_program);
    glDeleteProgram(post_program);
    delete chunk;
}

void World::render()
{
    static float theta = 0;
    theta += .025;

    int windoww, windowh;
    StateWindow::instance()->get_dimensions(&windoww, &windowh);

    //render to off-screen buffer
    glUseProgram(pre_program);
	glBindFramebuffer(GL_FRAMEBUFFER, renderbuffer.framebuffer);
    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4_t projection = getprojectionmatrix(90, (float)windoww / (float)windowh, 3000, .1);

    vec3_t pos = {20*cos(theta) + 8, 20, 20*sin(theta) + 8};
    static const vec3_t up = {0, 1, 0};
    static const vec3_t forward = {8, 8, 8};

    mat4_t view = getviewmatrix(pos, forward, up);
    mat4_t vp;

    dotmat4mat4(&vp, &projection, &view);
    glUniformMatrix4fv(pre_uniform_viewprojectionmatrix, 1, GL_FALSE, vp.mat);

    //long3_t chunkpos = chunk_pos_get(data[x][y][z].chunk);
    //long3_t worldpos = get_worldpos_from_chunkpos(&chunkpos);
    //mat4_t matrix = gettranslatematrix(worldpos.x - pos.x, worldpos.y - pos.y, worldpos.z - pos.z);
    mat4_t matrix = gettranslatematrix(0, 0, 0);
    glUniformMatrix4fv(pre_uniform_modelmatrix, 1, GL_FALSE, matrix.mat);

    chunk->render();

    //render to screen
    glUseProgram(post_program);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    GLfloat window_vec[2] = {(float)windoww, (float)windowh};
    glUniform2fv(post_uniform_window_size, 1, window_vec);

    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindBuffer(GL_ARRAY_BUFFER, post_buffer_target);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, renderbuffer.colorbuffer);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, renderbuffer.depthbuffer);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void World::update_window_size()
{
    int windoww, windowh;
    StateWindow::instance()->get_dimensions(&windoww, &windowh);

    glBindFramebuffer(GL_FRAMEBUFFER, renderbuffer.framebuffer);

    glBindTexture(GL_TEXTURE_2D, renderbuffer.colorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windoww, windowh, 0, GL_RGB, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, renderbuffer.depthbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, windoww, windowh, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
}
