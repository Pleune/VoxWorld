#include "world.hpp"

#include <cstdlib>
#include "chunk.hpp"
#include "custommath.h"
#include "gl.h"
#include "state_window.hpp"
#include "logger.hpp"
#include "chunkgen.hpp"
#include "worldgen.hpp"

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
     client_tick_lim(10)
{
    center = {0,0,0};
    int num_threads = std::thread::hardware_concurrency();
    generator = new WorldGenerator(num_threads);
    Logger::stdout.log(Logger::INFO) << "WorldGenerator disbatcher created with "
                                     << num_threads << " threads." << Logger::MessageStream::endl;
    SDL_GLContext glcon = StateWindow::instance()->create_shared_gl_context();
    client_tick_t = new std::thread(&World::client_tick_func, this, glcon);
    chunk_generator = new ChunkGeneratorPerlin2D();
}

World::~World()
{
    stopthreads = true;
    client_tick_t->join();
    chunks_for_render_m.lock();
    chunks_rwl.lock(RWLock::WRITE);
    delete generator;
    if(chunks_for_render)
        delete chunks_for_render;
    for(ChunkMap::iterator it = chunks.begin(); it!=chunks.end(); it++)
    {
        while(!it->second->can_delete()) SDL_Delay(2);
        delete it->second;
    }
    chunks_rwl.unlock();
    delete client_tick_t;
    delete chunk_generator;
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

void World::render(Camera camera)
{
    int chunk_size = Chunk::size();
    center = {
        (int)std::roundf((camera.pos.x - chunk_size/2)/chunk_size),
        (int)std::roundf((camera.pos.y - chunk_size/2)/chunk_size),
        (int)std::roundf((camera.pos.z - chunk_size/2)/chunk_size),
    };

    int windoww, windowh;
    StateWindow::instance()->get_dimensions(&windoww, &windowh);

    //render to off-screen buffer
    glUseProgram(pre_program);
	glBindFramebuffer(GL_FRAMEBUFFER, renderbuffer.framebuffer);
    glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4_t projection = getprojectionmatrix(90, (float)windoww / (float)windowh, 3000, .1);

    static const vec3_t zero = {0, 0, 0};
    mat4_t view = getviewmatrix(zero, camera.forward, camera.up);

    mat4_t vp;
    dotmat4mat4(&vp, &projection, &view);
    glUniformMatrix4fv(pre_uniform_viewprojectionmatrix, 1, GL_FALSE, vp.mat);

    chunks_for_render_m.lock();
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    if(chunks_for_render)
    {
        for(std::vector<Chunk *>::iterator it = chunks_for_render->begin(); it != chunks_for_render->end(); it++)
            (*it)->render(camera.pos);
    }
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    chunks_for_render_m.unlock();

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

    glEnableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
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

std::list<Chunk *> World::client_tick_maploop(const long3_t &center)
{
    //The next set of chunks to be rendered from
    std::vector<Chunk *> *render_vec = new std::vector<Chunk *>;
    std::list<Chunk *> chunks_for_delete;

    //Search for chunks to be removed from map
    for(ChunkMap::iterator it = chunks.begin(); it!=chunks.end();)
    {
        long3_t cpos = it->first;
        long double dist;
        distlong3(&dist, &cpos, &center);
        Chunk *chnk = it->second;

        if(chnk != NULL)
        {
            if(dist > radius)
            {
                chunks_for_delete.push_back(chnk);
                chunks_rwl.lock(RWLock::WRITE);
                it = chunks.erase(it);
                chunks_rwl.unlock();
            } else {
                render_vec->push_back(it->second);

                switch(chnk->gen_level())
                {
                case 2:
                    {
                        long3_t cpos = chnk->cpos();

                        ChunkMap::iterator end = chunks.end();
                        ChunkMap::iterator it_a = chunks.find({cpos.x, cpos.y+1, cpos.z});
                        ChunkMap::iterator it_b = chunks.find({cpos.x, cpos.y-1, cpos.z});

                        Chunk *chunkabove = it_a == end ? 0 : it_a->second;
                        Chunk *chunkbelow = it_b == end ? 0 : it_b->second;

                        if(chunkabove && chunkbelow)
                        {
                            generator->generate(chnk,
                                               chunkabove,
                                               chunkbelow,
                                               &WorldGen::normal);

                            chnk->gen_inc();
                        }

                        break;
                    }
                case 4:
                    {
                        long3_t cpos = chnk->cpos();

                        ChunkMap::iterator end = chunks.end();
                        ChunkMap::iterator it_a = chunks.find({cpos.x, cpos.y+1, cpos.z});
                        ChunkMap::iterator it_b = chunks.find({cpos.x, cpos.y-1, cpos.z});
                        ChunkMap::iterator it_n = chunks.find({cpos.x, cpos.y, cpos.z-1});
                        ChunkMap::iterator it_s = chunks.find({cpos.x, cpos.y, cpos.z+1});
                        ChunkMap::iterator it_e = chunks.find({cpos.x+1, cpos.y, cpos.z});
                        ChunkMap::iterator it_w = chunks.find({cpos.x-1, cpos.y, cpos.z});

                        Chunk *chunkabove = it_a == end ? 0 : it_a->second;
                        Chunk *chunkbelow = it_b == end ? 0 : it_b->second;
                        Chunk *chunknorth = it_n == end ? 0 : it_n->second;
                        Chunk *chunksouth = it_s == end ? 0 : it_s->second;
                        Chunk *chunkeast = it_e == end ? 0 : it_e->second;
                        Chunk *chunkwest = it_w == end ? 0 : it_w->second;

                        if(chunkabove && chunkbelow && chunknorth && chunksouth && chunkeast && chunkwest)
                        {
                            chnk->lock(RWLock::READ);
                            generator->remesh(chnk,
                                             chunkabove,
                                             chunkbelow,
                                             chunknorth,
                                             chunksouth,
                                             chunkeast,
                                             chunkwest,
                                             false);
                            chnk->unlock();

                            chnk->gen_inc();
                        }


                        break;
                    }
                }

                it++;
            }
        }
    }

    //Swap render list out from under the render thread
    //only swap list when not in render loop
    chunks_for_render_m.lock();
    std::vector<Chunk *> *tmp_render_vec = chunks_for_render;
    chunks_for_render = render_vec;
    chunks_for_render_m.unlock();
    if(tmp_render_vec)
        delete tmp_render_vec;

    return chunks_for_delete;//should use std::move instead of copy contructor?
}

void World::client_tick_regenerate(const long3_t &center)
{
    //Search for chunks that need to be generated
    for(int x = -radius + center.x; x<= radius + center.x; x++)
    for(int z = -radius + center.z; z<= radius + center.z; z++)
    for(int y = -radius + center.y; y<= radius + center.y; y++)
    {
        long3_t cpos = {x,y,z};
        long double dist;
        distlong3(&dist, &cpos, &center);
        if(dist < radius)
        {
            ChunkMap::iterator it = chunks.find(cpos);
            if(it == chunks.end())
            {
                Chunk *chnk = new Chunk(cpos.x, cpos.y, cpos.z);
                auto pair = chunks.insert({cpos, chnk});
                if(pair.second)//was inserted
                {
                    generator->generate(chnk, chunk_generator);
                    chnk->gen_inc();
                }
            }
        }
    }
}

void World::client_tick_func(SDL_GLContext glcon)
{
    //Create opengl context to allow uploading of mesh in this thread
    StateWindow::instance()->make_gl_context_current(glcon);

    std::list<Chunk *> chunks_for_delete;

    while(!stopthreads)
    {
        long3_t center_ = this->center;

        chunks_for_delete.splice(chunks_for_delete.begin(), client_tick_maploop(center_));
        chunks_for_delete.sort();
        chunks_for_delete.unique();

        client_tick_regenerate(center_);

        //Make sure that the force_mesh_upload gets placed into the
        //GPU queue
        //glFlush();

        //Actually delete chunks after new list is in place
        for(std::list<Chunk *>::iterator it = chunks_for_delete.begin(); it != chunks_for_delete.end();)
        {
            chunks_for_render_m.lock();//lock inside the loop to give
                                       //render thread many
                                       //opertunities to relock for itself

            if((*it)->can_delete())
            {
                delete (*it);
                it = chunks_for_delete.erase(it);
            } else {
                it++;
            }

            chunks_for_render_m.unlock();
        }

        GLenum err = GL_NO_ERROR;
        while((err = glGetError()) != GL_NO_ERROR)
        {
            Logger::stdout.log(Logger::ERROR) << "OpenGL Error: " << err << Logger::MessageStream::endl;
        }

        client_tick_lim.delay();
    }

    for(std::list<Chunk *>::iterator it = chunks_for_delete.begin(); it != chunks_for_delete.end();)
    {
        if((*it)->can_delete())
        {
            delete (*it);
            it = chunks_for_delete.erase(it);
        } else {
            SDL_Delay(2);
        }
    }

    SDL_GL_DeleteContext(glcon);
}

Block::ID World::get(long x, long y, long z)
{
    int sizei = Chunk::size();
    float size = sizei;
    chunks_rwl.lock(RWLock::READ);
    auto it = chunks.find({(long)std::floor(x/size),
                (long)std::floor(y/size),
                (long)std::floor(z/size)});
    chunks_rwl.unlock();

    if(it == chunks.end())
        return Block::AIR;

    it->second->lock(RWLock::READ);
    auto ret = it->second->get(MODULO(x, sizei), MODULO(y, sizei), MODULO(z, sizei));
    it->second->unlock();

    return ret;
}

void World::set(Block::ID id, long x, long y, long z, bool update)
{
    int sizei = Chunk::size();
    float size = sizei;
    chunks_rwl.lock(RWLock::READ);
    auto it = chunks.find({(long)std::floor(x/size),
                (long)std::floor(y/size),
                (long)std::floor(z/size)});
    chunks_rwl.unlock();

    if(it == chunks.end())
        return;

    Chunk* chnk = it->second;

    chnk->lock(RWLock::WRITE);
    chnk->set(MODULO(x, sizei), MODULO(y, sizei), MODULO(z, sizei), id);
    if(update)
    {
        long3_t cpos = chnk->cpos();

        chunks_rwl.lock(RWLock::READ);
        ChunkMap::iterator end = chunks.end();
        ChunkMap::iterator it_a = chunks.find({cpos.x, cpos.y+1, cpos.z});
        ChunkMap::iterator it_b = chunks.find({cpos.x, cpos.y-1, cpos.z});
        ChunkMap::iterator it_n = chunks.find({cpos.x, cpos.y, cpos.z-1});
        ChunkMap::iterator it_s = chunks.find({cpos.x, cpos.y, cpos.z+1});
        ChunkMap::iterator it_e = chunks.find({cpos.x+1, cpos.y, cpos.z});
        ChunkMap::iterator it_w = chunks.find({cpos.x-1, cpos.y, cpos.z});
        chunks_rwl.unlock();

        Chunk *chunkabove = it_a == end ? 0 : it_a->second;
        Chunk *chunkbelow = it_b == end ? 0 : it_b->second;
        Chunk *chunknorth = it_n == end ? 0 : it_n->second;
        Chunk *chunksouth = it_s == end ? 0 : it_s->second;
        Chunk *chunkeast = it_e == end ? 0 : it_e->second;
        Chunk *chunkwest = it_w == end ? 0 : it_w->second;

        generator->remesh(chnk,
                     chunkabove,
                     chunkbelow,
                     chunknorth,
                     chunksouth,
                     chunkeast,
                     chunkwest,
                     true);
        chnk->gen_inc();
    }

    it->second->unlock();
}

World::BlockIterator::BlockIterator(World& ref, long3_t pos)
    :ref(ref), chnk(0)
{
    sizei = Chunk::size();
    size = sizei;

    cpos ={(int)std::floor(pos.x/size),
                (int)std::floor(pos.y/size),
                (int)std::floor(pos.z/size)};
    ipos = {(int)MODULO(pos.x, sizei), (int)MODULO(pos.y, sizei), (int)MODULO(pos.z, sizei)};
}

World::BlockIterator::~BlockIterator()
{
    if(chnk)
        chnk->unlock_delete();
}

bool World::BlockIterator::update_chnk()
{
    if(chnk)
        chnk->unlock_delete();

    ref.chunks_rwl.lock(RWLock::READ);
    auto it = ref.chunks.find(cpos);
    ref.chunks_rwl.unlock();
    if(it == ref.chunks.end())
    {
        chnk = 0;
        return false;
    }
    chnk = it->second;
    chnk->lock_delete();
    return true;
}

void World::BlockIterator::move(int3_t delta)
{
    ipos.x += delta.x;
    ipos.y += delta.y;
    ipos.z += delta.z;

    bool cpos_changed;

    while(ipos.x >= sizei)
    {
        ipos.x -= sizei;
        cpos.x++;
        cpos_changed = true;
    }
    while(ipos.y >= sizei)
    {
        ipos.y -= sizei;
        cpos.y++;
        cpos_changed = true;
    }
    while(ipos.z >= sizei)
    {
        ipos.z -= sizei;
        cpos.z++;
        cpos_changed = true;
    }

    while(ipos.x < 0)
    {
        ipos.x += sizei;
        cpos.x--;
        cpos_changed = true;
    }
    while(ipos.y < 0)
    {
        ipos.y += sizei;
        cpos.y--;
        cpos_changed = true;
    }
    while(ipos.z < 0)
    {
        ipos.z += sizei;
        cpos.z--;
        cpos_changed = true;
    }

    if(cpos_changed)
    {
        if(chnk) chnk->unlock_delete();
        chnk = 0;
    }
}

Block::ID World::BlockIterator::get()
{
    if(chnk == 0)
        if(!update_chnk())
            return Block::AIR;

    chnk->lock(RWLock::READ);
    auto ret = chnk->get(ipos.x, ipos.y, ipos.z);
    chnk->unlock();

    return ret;
}

void World::BlockIterator::set(Block::ID id, bool update)
{
    if(chnk == 0)
        if(!update_chnk())
            return;

    chnk->lock(RWLock::WRITE);
    chnk->set(ipos.x, ipos.y, ipos.z, id);
    if(update)
    {
        long3_t cpos = chnk->cpos();

        ref.chunks_rwl.lock(RWLock::READ);
        ChunkMap::iterator end = ref.chunks.end();
        ChunkMap::iterator it_a = ref.chunks.find({cpos.x, cpos.y+1, cpos.z});
        ChunkMap::iterator it_b = ref.chunks.find({cpos.x, cpos.y-1, cpos.z});
        ChunkMap::iterator it_n = ref.chunks.find({cpos.x, cpos.y, cpos.z-1});
        ChunkMap::iterator it_s = ref.chunks.find({cpos.x, cpos.y, cpos.z+1});
        ChunkMap::iterator it_e = ref.chunks.find({cpos.x+1, cpos.y, cpos.z});
        ChunkMap::iterator it_w = ref.chunks.find({cpos.x-1, cpos.y, cpos.z});
        ref.chunks_rwl.unlock();

        Chunk *chunkabove = it_a == end ? 0 : it_a->second;
        Chunk *chunkbelow = it_b == end ? 0 : it_b->second;
        Chunk *chunknorth = it_n == end ? 0 : it_n->second;
        Chunk *chunksouth = it_s == end ? 0 : it_s->second;
        Chunk *chunkeast = it_e == end ? 0 : it_e->second;
        Chunk *chunkwest = it_w == end ? 0 : it_w->second;

        ref.generator->remesh(chnk,
                     chunkabove,
                     chunkbelow,
                     chunknorth,
                     chunksouth,
                     chunkeast,
                     chunkwest,
                     true);
        chnk->gen_inc();
    }
    chnk->unlock();
}

long3_t World::blockpick_ref(vec3_t start, vec3_t dir, bool before, float dist)
{
    long3_t p;
    p.x = floorf(start.x);
    p.y = floorf(start.y);
    p.z = floorf(start.z);

    BlockIterator it = iterator(p);

    vec3_t b;
    b.x = start.x - p.x;
    b.y = start.y - p.y;
    b.z = start.z - p.z;

    int dirx = dir.x > 0 ? 1 : -1;
    int diry = dir.y > 0 ? 1 : -1;
    int dirz = dir.z > 0 ? 1 : -1;

    int3_t iszero = {
        dir.x == 0,
        dir.y == 0,
        dir.z == 0
    };

    vec3_t rt;
    vec3_t delta = {0, 0, 0};
    if(!iszero.x)
    {
        rt.y = dir.y / dir.x;
        rt.z = dir.z / dir.x;
        delta.x = sqrtf(1 + rt.y*rt.y + rt.z*rt.z);
    }
    if(!iszero.y)
    {
        rt.x = dir.x / dir.y;
        rt.z = dir.z / dir.y;
        delta.y = sqrtf(rt.x*rt.x + 1 + rt.z*rt.z);
    }
    if(!iszero.z)
    {
        rt.x = dir.x / dir.z;
        rt.y = dir.y / dir.z;
        delta.z = sqrtf(rt.x*rt.x + rt.y*rt.y + 1);
    }

    vec3_t max = {
        delta.x * (dir.x > 0 ? (1 - b.x) : b.x),
        delta.y * (dir.y > 0 ? (1 - b.y) : b.y),
        delta.z * (dir.z > 0 ? (1 - b.z) : b.z)
    };

    int i;
    for(i=0; i<dist; i++)
    {
        if((max.x <= max.y || iszero.y) && (max.x <= max.z || iszero.z) && !iszero.x)
        {
            max.x += delta.x;
            it.move({dirx, 0, 0});//p.x += dirx;
            if(Block::is_solid(it.get()))
            {
                if(before)
                    it.move({-dirx, 0, 0});//p.x -= dirx;
                break;
            }
        }
        else if((max.y <= max.x || iszero.x) && (max.y <= max.z || iszero.z) && !iszero.y)
        {
            max.y += delta.y;
            it.move({0, diry, 0});//p.y += diry;
            if(Block::is_solid(it.get()))
            {
                if(before)
                    it.move({0, -diry, 0});//p.y -= diry;
                break;
            }
        }
        else if((max.z <= max.x || iszero.x) && (max.z <= max.y || iszero.y) && !iszero.z)
        {
            max.z += delta.z;
            it.move({0, 0, dirz});//p.z += dirz;
            if(Block::is_solid(it.get()))
            {
                if(before)
                    it.move({0, 0, -dirz});//p.z -= dirz;
                break;
            }
        }
    }

    return it.pos();
}

Block::ID World::blockpick_get(vec3_t start, vec3_t dir, bool before, float dist)
{
    long3_t p = blockpick_ref(start, dir, before, dist);
    return get(p.x, p.y, p.z);
}

void World::blockpick_set(Block::ID id, vec3_t start, vec3_t dir, bool before, float dist, bool update)
{
    long3_t p = blockpick_ref(start, dir, before, dist);
    set(id, p.x, p.y, p.z, update);
}
