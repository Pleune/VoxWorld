#include "chunk.hpp"

#include <random>
#include <vector>
#include <GL/glew.h>
#include "block.hpp"
#include "logger.hpp"

GLuint Chunk::static_index_elements[2] = {0,0};
int Chunk::side_len = 16;
int Chunk::side_len_p1 = side_len +1;
int Chunk::side_len_m1 = side_len -1;

GLuint Chunk::draw_program;
GLuint Chunk::draw_uniform_modelmatrix;

Chunk::Chunk(long x, long y, long z)
    :pos({x,y,z}),
     data(2, 0),
     dont_delete(0)
{
}

void Chunk::render(vec3_t camera_pos)
{
    if(num_vertices_ > 0 && has_mesh)
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

void Chunk::remesh(Chunk *chunkabove, Chunk *chunkbelow, Chunk *chunknorth, Chunk *chunksouth, Chunk *chunkeast, Chunk *chunkwest)
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

    Block::ID *above_b = new Block::ID[side_len*side_len];
    Block::ID *below_b = new Block::ID[side_len*side_len];
    Block::ID *north_b = new Block::ID[side_len*side_len];
    Block::ID *south_b = new Block::ID[side_len*side_len];
    Block::ID *east_b = new Block::ID[side_len*side_len];
    Block::ID *west_b = new Block::ID[side_len*side_len];

    if(chunkabove)
    {
        chunkabove->lock(RWLock::READ);
        for(int x=0; x<side_len; x++)
        for(int z=0; z<side_len; z++)
            above_b[x + side_len*z] = chunkabove->get(x, 0, z);
        chunkabove->unlock();
    } else {
        std::fill_n(above_b, side_len*side_len, Block::AIR);
    }

    if(chunkbelow)
    {
        chunkbelow->lock(RWLock::READ);
        for(int x=0; x<side_len; x++)
        for(int z=0; z<side_len; z++)
            below_b[x + side_len*z] = chunkbelow->get(x, side_len_m1, z);
        chunkbelow->unlock();
    } else {
        std::fill_n(below_b, side_len*side_len, Block::AIR);
    }

    if(chunknorth)
    {
        chunknorth->lock(RWLock::READ);
        for(int x=0; x<side_len; x++)
        for(int y=0; y<side_len; y++)
            north_b[x + side_len*y] = chunknorth->get(x, y, side_len_m1);
        chunknorth->unlock();
    } else {
        std::fill_n(north_b, side_len*side_len, Block::AIR);
    }

    if(chunksouth)
    {
        chunksouth->lock(RWLock::READ);
        for(int x=0; x<side_len; x++)
        for(int y=0; y<side_len; y++)
            south_b[x + side_len*y] = chunksouth->get(x, y, 0);
        chunksouth->unlock();
    } else {
        std::fill_n(south_b, side_len*side_len, Block::AIR);
    }

    if(chunkeast)
    {
        chunkeast->lock(RWLock::READ);
        for(int y=0; y<side_len; y++)
        for(int z=0; z<side_len; z++)
            east_b[y + side_len*z] = chunkeast->get(0, y, z);
        chunkeast->unlock();
    } else {
        std::fill_n(east_b, side_len*side_len, Block::AIR);
    }

    if(chunkwest)
    {
        chunkwest->lock(RWLock::READ);
        for(int y=0; y<side_len; y++)
        for(int z=0; z<side_len; z++)
            west_b[y + side_len*z] = chunkwest->get(side_len_m1, y, z);
        chunkwest->unlock();
    } else {
        std::fill_n(west_b, side_len*side_len, Block::AIR);
    }

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
                {                    int top=1, bottom=1, south=1, north=1, east=1, west=1;

                    if(y==side_len-1)
                    {
                        if(Block::is_solid(above_b[x + side_len*z]))
                            top = 0;
                    } else {
                        if(Block::is_solid(data.get(x,y+1,z)))
                            top = 0;
                    }
                    if(y==0)
                    {
                        if(Block::is_solid(below_b[x + side_len*z]))
                            bottom = 0;
                    } else {
                        if(Block::is_solid(data.get(x,y-1,z)))
                            bottom = 0;
                    }
                    if(z==side_len-1)
                    {
                        if(Block::is_solid(south_b[x + side_len*y]))
                            south = 0;
                    } else {
                        if(Block::is_solid(data.get(x,y,z+1)))
                            south = 0;
                    }
                    if(z==0)
                    {
                        if(Block::is_solid(north_b[x + side_len*y]))
                            north = 0;
                    } else {
                        if(Block::is_solid(data.get(x,y,z-1)))
                            north = 0;
                    }

                    if(x==side_len-1)
                    {
                        if(Block::is_solid(east_b[y + side_len*z]))
                            east = 0;
                    } else {
                        if(Block::is_solid(data.get(x+1,y,z)))
                            east = 0;
                    }
                    if(x==0)
                    {
                        if(Block::is_solid(west_b[y + side_len*z]))
                            west = 0;
                    } else {
                        if(Block::is_solid(data.get(x-1,y,z)))
                            west = 0;
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

    delete[] above_b;
    delete[] below_b;
    delete[] north_b;
    delete[] south_b;
    delete[] east_b;
    delete[] west_b;

    mesh.set(elements.data(), elements.size() * sizeof(GLuint), GL_STATIC_DRAW);
    num_vertices_ = elements.size();
    has_mesh = true;
}

void Chunk::force_mesh_upload()
{
    if(!has_mesh)
        Logger::stdout.log(Logger::ERROR) << "Chunk::force_mesh_upload(): mesh not generated yet!" << Logger::MessageStream::endl;
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
    //TODO: only if changed
    //mesh_current = false;
    if(gen_level_ > 4)
        gen_level_ = 4;
}

Chunk::Status Chunk::init(GLuint draw_program)
{
    Chunk::draw_program = draw_program;
    draw_uniform_modelmatrix = glGetUniformLocation(draw_program, "MODEL");

    glGenBuffers(2, static_index_elements);

    size_t buffer_len = side_len_p1*side_len_p1*side_len_p1 * 3 * NUM_BLOCK_TYPES;

    GLfloat *vertices_pos = new GLfloat[buffer_len];
    GLfloat *vertices_color = new GLfloat[buffer_len];

    std::random_device rd;
    std::default_random_engine rand_gen(rd());
    std::uniform_int_distribution<int> dist (-10, 10);

    float wobble[side_len][side_len][side_len][3];
    for(size_t i=0; i<side_len*side_len*side_len*3; i++)
        reinterpret_cast<float *>(wobble)[i] = dist(rand_gen) / 230.0f;

    int i = 0;
    for(int id = 0; id<NUM_BLOCK_TYPES; id++)
    for(int z = 0; z<side_len_p1; z++)
    for(int y = 0; y<side_len_p1; y++)
    for(int x = 0; x<side_len_p1; x++)
    {
        SDL_Color color = Block::color(id);

        vertices_pos[i] = x + wobble[x == side_len ? 0 : x][y == side_len ? 0 : y][z == side_len ? 0 : z][0];
        vertices_color[i] = color.r / 255.0;
        ++i;

        vertices_pos[i] = y + wobble[x == side_len ? 0 : x][y == side_len ? 0 : y][z == side_len ? 0 : z][1];
        vertices_color[i] = color.g / 255.0;
        ++i;

        vertices_pos[i] = z + wobble[x == side_len ? 0 : x][y == side_len ? 0 : y][z == side_len ? 0 : z][2];
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
