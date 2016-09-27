#include "worldgenerator.hpp"

WorldGenerator::WorldGenerator(int num_threads)
    :num_threads(num_threads)
{
    threads = new std::thread *[num_threads];
    for(int i=0; i<num_threads; i++)
        threads[i] = new std::thread(&WorldGenerator::worker, this);
}

WorldGenerator::~WorldGenerator()
{
    stop_threads = true;

    for(int i=0; i<num_threads; i++)
    {
        Message m = {0,0,0};
        queue.push(m);
    }

    for(int i=0; i<num_threads; i++)
    {
        threads[i]->join();
        delete threads[i];
    }

    delete[] threads;
}

void WorldGenerator::generate(Chunk **ret, Chunk *chunk, ChunkGen::GenFunc f)
{
    Message m = {ret, chunk, f};
    queue.push(m);
}

void WorldGenerator::worker()
{
    while(!stop_threads)
    {
        Message m;
        queue.pop(m);

        if(m.chunk)
        {
            m.f(m.chunk);
            *m.ret = m.chunk;
        }
    }
}
