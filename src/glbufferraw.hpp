#ifndef BLOCKS_GLBUFFER_H
#define BLOCKS_GLBUFFER_H

#include <mutex>
#include <cstring>
#include <GL/glew.h>

class GLBufferRaw {
public:
    inline GLBufferRaw() {glGenBuffers(1, &buffer);}
    inline ~GLBufferRaw() {glDeleteBuffers(1, &buffer); if(data_) delete data_;}

    inline void bind(GLenum target)
    {
        upload_m.lock();
        glBindBuffer(target, buffer);
        make_current(target);
        upload_m.unlock();
    }

    inline void set(void *data, size_t len, GLenum usage)
    {
        upload_m.lock();
        if(upload_ready)
        {
            upload_ready = false;
            if(data_ != 0)
                delete[] data_;
        }

        data_ = new unsigned char[len];
        memcpy(data_, data, len);
        data_len = len;
        data_usage = usage;
        upload_ready = true;
        upload_m.unlock();
    }

private:
    inline void make_current(GLenum target)
    {
        if(upload_ready)
        {
            glBufferData(target, data_len, data_, data_usage);
            delete[] data_;
            data_ = 0;
            upload_ready = false;
        }
    }

    std::mutex upload_m;
    bool upload_ready = false;
    unsigned char *data_ = 0;
    size_t data_len = 0;
    GLenum data_usage;

    GLuint buffer;
};

#endif
