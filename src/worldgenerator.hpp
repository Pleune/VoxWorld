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

    void generate(Chunk **ret, Chunk *chunk, ChunkGen::GenFunc);

private:
    struct Message {
        Chunk **ret;
        Chunk *chunk;
        ChunkGen::GenFunc f;
    };

    void worker();

    TQueue<Message> queue;
    bool stop_threads = false;
    int num_threads;
    std::thread **threads;
};

#endif
