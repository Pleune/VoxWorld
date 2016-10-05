#ifndef BLOCKS_WORLDGENERATOR_H
#define BLOCKS_WORLDGENERATOR_H

#include <thread>
#include "queue.hpp"
#include "chunk.hpp"
#include "chunkgen.hpp"

class WorldGenerator {
public:
    WorldGenerator(int num_threads);
    ~WorldGenerator();

    void generate(Chunk **ret, Chunk *chunk, ChunkGenerator *);
    void remesh(Chunk *chunk, Chunk *chunkabove, Chunk *chunkbelow, Chunk *chunknorth, Chunk *chunksouth, Chunk *chunkeast, Chunk *chunkwest);

private:
    struct Message {
        enum Type {GENERATION, REMESH, CLOSE_SIG} m_type;
        union {
            struct {
                Chunk **ret;
                Chunk *chunk;
                ChunkGenerator *generator;
            } generation;
            struct {
                Chunk *chunk;
                Chunk *chunkabove;
                Chunk *chunkbelow;
                Chunk *chunknorth;
                Chunk *chunksouth;
                Chunk *chunkeast;
                Chunk *chunkwest;
            } remesh;
        } data;
    };

    void worker();

    void generation_f(Message &m);
    void remesh_f(Message &m);

    TQueue<Message> queue;
    bool stop_threads = false;
    int num_threads;
    std::thread **threads;
};

#endif
