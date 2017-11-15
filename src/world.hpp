#ifndef BLOCKS_WORLD_H
#define BLOCKS_WORLD_H

#include <unordered_map>
#include <vector>
#include <list>
#include <functional>
#include "chunk.hpp"
#include "worldgenerator.hpp"
#include "limiter.hpp"
#include "camera.hpp"
#include "custommath.h"
#include "rwlock.hpp"

class World {
public:
    class BlockIterator {
    public:
        friend World;
        ~BlockIterator();

        void move(int3_t);

        long3_t pos() {return {cpos.x*sizei + ipos.x, cpos.y*sizei + ipos.y, cpos.z*sizei + ipos.z};};
        Block::ID get();
        void set(Block::ID, bool update);

    private:
        BlockIterator(World& ref, long3_t);
        bool update_chnk();

        World &ref;
        Chunk* chnk;
        int sizei;
        float size;
        int3_t ipos;
        long3_t cpos;
    };

    World();
    ~World();

    void render(Camera camera);
    void update_window_size();

    static void init();
    static void cleanup();

    Block::ID get(long x, long y, long z);
    void set(Block::ID, long x, long y, long z, bool update);

    BlockIterator iterator(long3_t pos) {return BlockIterator(*this, pos);};

    Block::ID blockpick_get(vec3_t start, vec3_t dir, bool before, float dist);
    void blockpick_set(Block::ID, vec3_t start, vec3_t dir, bool before, float dist, bool update);

private:
    static unsigned long hash_cpos(long3_t l)
    {
        return std::hash<long>{}(l.x) ^ std::hash<long>{}(l.y) ^ std::hash<long>{}(l.z);
    }

    static bool compare_cpos(long3_t a, long3_t b)
    {
        return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
    }

    long3_t blockpick_ref(vec3_t start, vec3_t dir, bool before, float dist);

    typedef std::unordered_map<
        long3_t,
        Chunk *,
        std::function<unsigned long(long3_t)>,
        std::function<bool(long3_t, long3_t)>>
        ChunkMap;

    long3_t center = {0,0,0};
    int radius = 12;
    ChunkMap chunks;
    RWLock chunks_rwl;
    std::vector<Chunk *> *chunks_for_render = 0;
    std::mutex chunks_for_render_m;
    WorldGenerator *generator;
    ChunkGenerator *chunk_generator;

    bool stopthreads = false;

    //Client tick stuff
    std::thread *client_tick_t;
    Limiter client_tick_lim;
    void client_tick_func(SDL_GLContext);
    std::list<Chunk *> client_tick_maploop(const long3_t &center);
    void client_tick_regenerate(const long3_t &center);

    static GLuint pre_program;
    static GLuint pre_uniform_viewprojectionmatrix;
    static GLuint post_program;
	static GLuint post_uniform_tex;
	static GLuint post_uniform_depth;
    static GLuint post_uniform_window_size;
    static GLuint post_buffer_target;

    struct Renderbuff {
        GLuint framebuffer;

        GLuint colorbuffer;
        GLuint depthbuffer;

        GLuint colorbufferid;
        GLuint depthbufferid;
    } static renderbuffer;
};

#endif
