#ifndef GL_H
#define GL_H

#include <GL/glew.h>

void gl_program_load_str(GLuint *program, const char *vertexshader_src, const char *fragmentshader_src);
void gl_program_load_file(GLuint *program, const char *vertexshadername, const char *fragmentshadername, const char *basepath);

#endif
