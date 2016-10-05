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
        Message m;
        m.m_type = Message::CLOSE_SIG;
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
    Message m;
    m.m_type = Message::GENERATION;
    m.data.generation.ret = ret;
    m.data.generation.chunk = chunk;
    m.data.generation.f = f;
    chunk->lock_delete();
    queue.push(m);
}

void WorldGenerator::remesh(Chunk *chunk, Chunk *chunkabove, Chunk *chunkbelow, Chunk *chunknorth, Chunk *chunksouth, Chunk *chunkeast, Chunk *chunkwest)
{
    Message m;
    m.m_type = Message::REMESH;
    m.data.remesh.chunkabove = chunkabove;
    m.data.remesh.chunkbelow = chunkbelow;
    m.data.remesh.chunknorth = chunknorth;
    m.data.remesh.chunksouth = chunksouth;
    m.data.remesh.chunkeast = chunkeast;
    m.data.remesh.chunkwest = chunkwest;
    m.data.remesh.chunk = chunk;
    chunk->lock_delete();
    if(chunkabove)
        chunkabove->lock_delete();
    if(chunkbelow)
        chunkbelow->lock_delete();
    if(chunknorth)
        chunknorth->lock_delete();
    if(chunksouth)
        chunksouth->lock_delete();
    if(chunkeast)
        chunkeast->lock_delete();
    if(chunkwest)
        chunkwest->lock_delete();
    queue.push(m);
}

void WorldGenerator::worker()
{
    while(!stop_threads)
    {
        Message m;
        queue.pop(m);
        switch(m.m_type)
        {
        case Message::GENERATION:
            generation_f(m);
            m.data.generation.chunk->unlock_delete();
            break;
        case Message::REMESH:
            remesh_f(m);
            m.data.remesh.chunk->unlock_delete();
            if(m.data.remesh.chunkabove)
                m.data.remesh.chunkabove->unlock_delete();
            if(m.data.remesh.chunkbelow)
                m.data.remesh.chunkbelow->unlock_delete();
            if(m.data.remesh.chunknorth)
                m.data.remesh.chunknorth->unlock_delete();
            if(m.data.remesh.chunksouth)
                m.data.remesh.chunksouth->unlock_delete();
            if(m.data.remesh.chunkeast)
                m.data.remesh.chunkeast->unlock_delete();
            if(m.data.remesh.chunkwest)
                m.data.remesh.chunkwest->unlock_delete();
            break;
        case Message::CLOSE_SIG:
            break;
        }
    }
}

void WorldGenerator::generation_f(Message &m)
{

        if(m.data.generation.chunk)
        {
            m.data.generation.f(m.data.generation.chunk);
            *m.data.generation.ret = m.data.generation.chunk;
        }
}

void WorldGenerator::remesh_f(Message &m)
{
    Chunk *chunk = m.data.remesh.chunk;

    chunk->lock(Chunk::READ);
    chunk->remesh(m.data.remesh.chunkabove,
                  m.data.remesh.chunkbelow,
                  m.data.remesh.chunknorth,
                  m.data.remesh.chunksouth,
                  m.data.remesh.chunkeast,
                  m.data.remesh.chunkwest);
    chunk->unlock();
}
