#include "worldgenerator.hpp"

#include "logger.hpp"

WorldGenerator::WorldGenerator(int num_threads)
    :num_threads(num_threads)
{
    threads = new std::thread *[num_threads];
    for(int i=0; i<num_threads; i++)
        threads[i] = new std::thread(&WorldGenerator::worker, this);
}

WorldGenerator::~WorldGenerator()
{
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

void WorldGenerator::generate(Chunk *chunk, ChunkGenerator *generator)
{
    Message m;
    m.m_type = Message::GENERATIONA;
    m.data.generationa.chunk = chunk;
    m.data.generationa.generator = generator;
    chunk->lock_delete();
    queue.push(m);
}

void WorldGenerator::generate(Chunk *chunk, Chunk *chunkabove, Chunk *chunkbelow, WorldGen::GenFunc genfunc)
{
    Message m;
    m.m_type = Message::GENERATIONB;
    m.data.generationb.chunk = chunk;
    m.data.generationb.chunkabove = chunkabove;
    m.data.generationb.chunkbelow = chunkbelow;
    m.data.generationb.genfunc = genfunc;
    chunk->lock_delete();
    chunkabove->lock_delete();
    chunkbelow->lock_delete();

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
    Logger::standard.set_print_level(Logger::LOG_DEBUG);

    Uint32 time_gena = 0;
    Uint32 time_genb = 0;
    Uint32 time_remesh = 0;
    bool stop=false;
    while(!stop)
    {
        Message m;
        queue.pop(m);
        Uint32 start_time = SDL_GetTicks();
        switch(m.m_type)
        {
        case Message::GENERATIONA:
            generationa_f(m);
            m.data.generationa.chunk->unlock_delete();
            time_gena += SDL_GetTicks() - start_time;
            break;
        case Message::GENERATIONB:
            generationb_f(m);
            m.data.generationb.chunk->unlock_delete();
            m.data.generationb.chunkabove->unlock_delete();
            m.data.generationb.chunkbelow->unlock_delete();
            time_genb += SDL_GetTicks() - start_time;
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
            time_remesh += SDL_GetTicks() - start_time;
            break;
        case Message::CLOSE_SIG:
            Logger::standard.log(Logger::LOG_INFO) << "Worker thread close signal recieved" << Logger::MessageStream::endl;
            stop = true;
            break;
        }
    }

    Logger::standard.log(Logger::LOG_INFO) << "Worker thread times:\n"
                                     << "\tTime GenerateA: " << time_gena << "\n"
                                     << "\tTime GenerateB: " << time_genb << "\n"
                                     << "\tTime Remesh:    " << time_remesh << Logger::MessageStream::endl;
}

void WorldGenerator::generationa_f(Message &m)
{

        if(m.data.generationa.chunk)
        {
            m.data.generationa.generator->generate(m.data.generationa.chunk);
            m.data.generationa.chunk->gen_inc();
        }
}


void WorldGenerator::generationb_f(Message &m)
{

        if(m.data.generationb.chunk)
        {
            if(m.data.generationb.genfunc)
                m.data.generationb.genfunc(
                    m.data.generationb.chunk,
                    m.data.generationb.chunkabove,
                    m.data.generationb.chunkbelow);

            m.data.generationb.chunk->gen_inc();
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
