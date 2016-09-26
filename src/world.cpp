#include "world.hpp"

#include <cstdlib>
#include "chunk.hpp"
#include "custommath.h"
#include "gl.h"
#include "state_window.hpp"
#include "logger.hpp"
#include "chunkgen.hpp"

GLuint World::pre_program;
GLuint World::pre_uniform_viewprojectionmatrix;

GLuint World::post_program;
GLuint World::post_uniform_tex;
GLuint World::post_uniform_depth;
GLuint World::post_uniform_window_size;
GLuint World::post_buffer_target;

World::Renderbuff World::renderbuffer;

World::World()
    :chunks(1000, hash_cpos, compare_cpos),
     generator(8)
{
}

World::~World()
{
    for(ChunkMap::iterator it = chunks.begin(); it!=chunks.end(); it++)
        delete it->second;
}

void World::init()
{
    int windoww, windowh;
    StateWindow::instance()->get_dimensions(&windoww, &windowh);

    //Load Programs
    char *basepath = SDL_GetBasePath();
    gl_program_load_file(&pre_program, "shaders/vs", "shaders/fs", basepath);
    gl_program_load_file(&post_program, "shaders/pvs", "shaders/pfs", basepath);
    free(basepath);

    Chunk::init(pre_program);

	pre_uniform_viewprojectionmatrix = glGetUniformLocation(pre_program, "VP");
	post_uniform_tex = glGetUniformLocation(post_program, "tex");
	post_uniform_depth = glGetUniformLocation(post_program, "depth");
	post_uniform_window_size = glGetUniformLocation(post_program, "window_size");

    //setup for pre_program
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
}

void World::cleanup()
{
    glDeleteProgram(pre_program);
    glDeleteProgram(post_program);

    Chunk::cleanup();
}

void World::render()
{
    chunk_loader_func();

    static float theta = 0;
    theta += .01;

    int windoww, windowh;
    StateWindow::instance()->get_dimensions(&windoww, &windowh);

    //render to off-screen buffer
    glUseProgram(pre_program);
	glBindFramebuffer(GL_FRAMEBUFFER, renderbuffer.framebuffer);
    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4_t projection = getprojectionmatrix(90, (float)windoww / (float)windowh, 3000, .1);

    vec3_t forward = {cos(theta), 1, sin(theta)};
    static const vec3_t height = {0, 1.65, 0};
    vec3_t pos = {-cos(theta)*22*16, 22*16, -sin(theta)*22*16};
    static const vec3_t up = {0, 1, 0};

    mat4_t view = getviewmatrix(height, forward, up);

    mat4_t vp;
    dotmat4mat4(&vp, &projection, &view);
    glUniformMatrix4fv(pre_uniform_viewprojectionmatrix, 1, GL_FALSE, vp.mat);

    for(ChunkMap::iterator it = chunks.begin(); it!=chunks.end(); it++)
    {
        if(it->second)
            it->second->render(pos);
    }

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

void World::chunk_loader_func()
{
    long3_t center = {0,0,0};
    bool loaded[21][21][21];
    std::fill_n(&loaded[0][0][0], 21*21*21, false);

    for(ChunkMap::iterator it = chunks.begin(); it!=chunks.end(); it++)
    {
        long3_t localpos = {
            it->first.x - center.x,
            it->first.y - center.y,
            it->first.z - center.z
        };

        if(localpos.x >= -10 && localpos.x <= 10 &&
           localpos.y >= -10 && localpos.y <= 10 &&
           localpos.z >= -10 && localpos.z <= 10)
        {
            loaded[localpos.x+10][localpos.y+10][localpos.z+10] = true;
        } else {
            if(it->second == NULL)
                continue;

            Chunk *tmp = it->second;
            chunks.erase(it);
            delete tmp;
        }
    }

    for(int x=0; x<21; x++)
    for(int y=0; y<21; y++)
    for(int z=0; z<21; z++)
    {
        if(loaded[x][y][z] == false)
        {
            long3_t cpos = {x-10, y-10, z-10};
            Chunk *chnk = new Chunk(cpos.x, cpos.y, cpos.z);
            auto ret = chunks.insert({cpos, NULL});
            ChunkMap::iterator it = ret.first;
            if(ret.second)
            {
                generator.generate(&(it->second), chnk,
                                   &ChunkGen::random);
            } else {
                Logger::stdout.log(Logger::ERROR) << "World::chunk_loader_func(): failed to insert new chunk!";
                delete chnk;
            }
        }
    }
}
