#include "chunk.hpp"

#include <vector>
#include <GL/glew.h>
#include "block.hpp"
#include "logger.hpp"

GLuint Chunk::static_index_elements[2] = {0,0};
int Chunk::side_len = 16;
int Chunk::side_len_p1 = 17;

GLuint Chunk::draw_program;
GLuint Chunk::draw_uniform_modelmatrix;

Chunk::Chunk(long x, long y, long z)
    :pos({x,y,z}),
     data(4, 0)
{
}

void Chunk::render(vec3_t camera_pos)
{
    if(num_vertices_ > 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, static_index_elements[0]);
        glVertexAttribPointer(
                0,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                0);

        glBindBuffer(GL_ARRAY_BUFFER, static_index_elements[1]);
        glVertexAttribPointer(
                1,
                3,
                GL_FLOAT,
                GL_FALSE,
                0,
                0);

        mesh.bind(GL_ELEMENT_ARRAY_BUFFER);

        vec3_t wpos = {(float)(pos.x*side_len),
                       (float)(pos.y*side_len),
                       (float)(pos.z*side_len)};
        mat4_t matrix = gettranslatematrix(wpos.x - camera_pos.x, wpos.y - camera_pos.y, wpos.z - camera_pos.z);
        glUniformMatrix4fv(draw_uniform_modelmatrix, 1, GL_FALSE, matrix.mat);

        glDrawElements(GL_TRIANGLES, num_vertices_, GL_UNSIGNED_INT, 0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void Chunk::remesh()
{
    const static int faces[] = {
        //top
        0,1,0,
        0,1,1,
        1,1,0,

        1,1,1,
        1,1,0,
        0,1,1,

        //bottom
        0,0,0,
        1,0,0,
        0,0,1,

        1,0,1,
        0,0,1,
        1,0,0,

        //south
        0,1,1,
        0,0,1,
        1,1,1,

        1,0,1,
        1,1,1,
        0,0,1,

        //north
        1,1,0,
        1,0,0,
        0,1,0,

        0,0,0,
        0,1,0,
        1,0,0,

        //east
        1,1,0,
        1,1,1,
        1,0,0,

        1,0,0,
        1,1,1,
        1,0,1,

        //west
        0,1,0,
        0,0,0,
        0,1,1,

        0,0,1,
        0,1,1,
        0,0,0
    };

    Chunk *chunkabove = 0;
    Chunk *chunkbelow = 0;
    Chunk *chunknorth = 0;
    Chunk *chunksouth = 0;
    Chunk *chunkeast = 0;
    Chunk *chunkwest = 0;

    std::vector<GLuint> elements;
    int x, y, z;
    for(x=0; x<side_len; ++x)
    {
        for(y=0; y<side_len; ++y)
        {
            for(z=0; z<side_len; ++z)
            {
                Block::ID block = data.get(x, y, z);
                if(block != Block::AIR)
                {
                    //TODO: remove =1 cases
                    int top=1, bottom=1, south=1, north=1, east=1, west=1;

                    if(y==side_len-1)
                    {
                        if(chunkabove)
                        {
                            //block_t b = get_block(chunkabove, x,0,z);
                            //if(b.id != AIR && (block.id != WATER || block.metadata.number == SIM_WATER_LEVELS))
                            //    top=0;
                            //else
                            //    top=1;
                        } else
                            top=1;
                    } else {
                        if(Block::is_solid(data.get(x,y+1,z)))
                            top = 0;
                        else
                            top = 1;
                    }
                    if(y==0)
                    {
                        if(chunkbelow)
                        {
                            //if(get_block(chunkbelow, x,side_len-1,z).id)
                            //    bottom=0;
                            //else
                            //    bottom=1;
                        } else
                            bottom=1;
                    } else {
                        if(Block::is_solid(data.get(x,y-1,z)))
                            bottom = 0;
                        else
                            bottom = 1;
                    }
                    if(z==side_len-1)
                    {
                        if(chunksouth)
                        {
                            //block_t b = get_block(chunksouth, x,y,0);
                            //if(b.id != AIR && (b.id != WATER || (block.id == WATER && b.metadata.number == block.metadata.number)))
                            //    south=0;
                            //else
                            //    south=1;
                        } else
                            south=1;
                    } else {
                        if(Block::is_solid(data.get(x,y,z+1)))
                            south = 0;
                        else
                            south = 1;
                    }
                    if(z==0)
                    {
                        if(chunknorth)
                        {
                            //block_t b = get_block(chunknorth, x,y,CHUNKSIZE-1);
                            //if(b.id != AIR && (b.id != WATER || (block.id == WATER && b.metadata.number == block.metadata.number)))
                            //    north=0;
                            //else
                            //    north=1;
                        } else
                            north=1;
                    } else {
                        if(Block::is_solid(data.get(x,y,z-1)))
                            north = 0;
                        else
                            north = 1;
                    }

                    if(x==side_len-1)
                    {
                        if(chunkeast)
                        {
                            //block_t b = get_block(chunkeast, 0,y,z);
                            //if(b.id != AIR && (b.id != WATER || (block.id == WATER && b.metadata.number == block.metadata.number)))
                            //    east=0;
                            //else
                            //    east=1;
                        } else
                            east=1;
                    } else {
                        if(Block::is_solid(data.get(x+1,y,z)))
                            east = 0;
                        else
                            east = 1;
                    }
                    if(x==0)
                    {
                        if(chunkwest)
                        {
                            //block_t b = get_block(chunkwest, CHUNKSIZE-1,y,z);
                            //if(b.id != AIR && (b.id != WATER || (block.id == WATER && b.metadata.number == block.metadata.number)))
                            //    west=0;
                            //else
                            //    west=1;
                        } else
                            west=1;
                    } else {
                        if(Block::is_solid(data.get(x-1,y,z)))
                            west = 0;
                        else
                            west = 1;
                    }

                    int U[6] = {
                        top,
                        bottom,
                        south,
                        north,
                        east,
                        west
                    };
                    int q=0;
                    int t;

                    for(t=0; t<6; ++t)
                    {
                        if(U[t])
                        {
                            int Q=q+18;
                            while(q<Q)
                            {
                                int x_ = faces[q++] + x;
                                int y_ = faces[q++] + y;
                                int z_ = faces[q++] + z;

                                //TODO: create indexing function
                                GLuint index = x_ + y_*side_len_p1 + z_*side_len_p1*side_len_p1 + block * side_len_p1*side_len_p1*side_len_p1;
                                elements.push_back(index);
                            }
                        } else {
                            q+=18;
                        }
                    }
                }
            }
        }
    }

    mesh.set(elements.data(), elements.size() * sizeof(GLuint), GL_STATIC_DRAW);
    num_vertices_ = elements.size();
    has_mesh = true;
}

void Chunk::force_mesh_upload()
{
    if(!has_mesh)
        Logger::stdout.log(Logger::ERROR) << "OOPS";
    mesh.bind(GL_ELEMENT_ARRAY_BUFFER);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Block::ID Chunk::get(int x, int y, int z)
{
    return data.get(x, y, z);
}

void Chunk::set(int x, int y, int z, Block::ID id)
{
    data.set(x, y, z, id);
}

void Chunk::lock(LockType type)
{
    if(type == WRITE)
    {
        lock_m.lock();
        lock_type = WRITE;
    } else {
        if(lock_type == WRITE)
        {
            lock_m.lock();
            lock_type = READ;
            lock_num = 1;
        } else {
            if(lock_num == 0)
                lock_m.lock();
            lock_num++;
        }
    }
}

void Chunk::unlock()
{
    if(lock_type == WRITE)
    {
        lock_m.unlock();
    } else {
        lock_num--;
        if(lock_num == 0)
            lock_m.unlock();
    }
}

Chunk::Status Chunk::init(GLuint draw_program)
{
    Chunk::draw_program = draw_program;
    draw_uniform_modelmatrix = glGetUniformLocation(draw_program, "MODEL");

    glGenBuffers(2, static_index_elements);

    size_t buffer_len = side_len_p1*side_len_p1*side_len_p1 * 3 * NUM_BLOCK_TYPES;

    GLfloat *vertices_pos = new GLfloat[buffer_len];
    GLfloat *vertices_color = new GLfloat[buffer_len];

    int i = 0;
    for(int id = 0; id<NUM_BLOCK_TYPES; id++)
    for(int z = 0; z<side_len_p1; z++)
    for(int y = 0; y<side_len_p1; y++)
    for(int x = 0; x<side_len_p1; x++)
    {
        //TODO: render wobble
        SDL_Color color = Block::color(id);

        vertices_pos[i] = x;// + ((int)(noise3D(x%(side_len), y%(side_len), z%(side_len), 1) % 100) - 50) * (RENDER_WOBBLE / 100.0f);
        vertices_color[i] = color.r / 255.0;
        ++i;

        vertices_pos[i] = y;// + ((int)(noise3D(y%(side_len), z%(side_len), x%(side_len), 1) % 100) - 50) * (RENDER_WOBBLE / 100.0f);
        vertices_color[i] = color.g / 255.0;
        ++i;

        vertices_pos[i] = z;// + ((int)(noise3D(z%(side_len), x%(side_len), y%(side_len), 1) % 100) - 50) * (RENDER_WOBBLE / 100.0f);
        vertices_color[i] = color.b / 255.0;
        ++i;
    }

    glBindBuffer(GL_ARRAY_BUFFER, static_index_elements[0]);
    glBufferData(GL_ARRAY_BUFFER, buffer_len * sizeof(GLfloat), vertices_pos, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, static_index_elements[1]);
    glBufferData(GL_ARRAY_BUFFER, buffer_len * sizeof(GLfloat), vertices_color, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] vertices_pos;
    delete[] vertices_color;

    return OK;
}

void Chunk::cleanup()
{
    glDeleteBuffers(2, static_index_elements);
}
