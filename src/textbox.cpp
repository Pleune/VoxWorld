#include "textbox.hpp"

#include <SDL.h>

#include "state_window.hpp"
#include "logger.hpp"
#include "gl.h"

Textbox::TTFInfo Textbox::ttf_info[] = {
    [Textbox::ROBOTO_REGULAR] = {
        "Roboto-Regular",
        "resources/fonts/Roboto/Roboto-Regular.ttf"},
    [Textbox::ROBOTO_MEDIUM] = {
        "Roboto-Medium",
        "resources/fonts/Roboto/Roboto-Medium.ttf"},
    [Textbox::ROBOTO_ITALIC] = {
        "Roboto-Italic",
        "resources/fonts/Roboto/Roboto-Italic.ttf"},
    [Textbox::ROBOTO_BOLD] = {
        "Roboto-Bold",
        "resources/fonts/Roboto/Roboto-Bold.ttf"},
    [Textbox::ROBOTO_THIN] = {
        "Roboto-Thin",
        "resources/fonts/Roboto/Roboto-Thin.ttf"},
    [Textbox::ROBOTO_LIGHT] = {
        "Roboto-Light",
        "resources/fonts/Roboto/Roboto-Light.ttf"},
    [Textbox::ROBOTO_BLACK] = {
        "Roboto-Black",
        "resources/fonts/Roboto/Roboto-Black.ttf"},
    [Textbox::ROBOTO_BLACKITALIC] = {
        "Roboto-BlackItalic",
        "resources/fonts/Roboto/Roboto-BlackItalic.ttf"},
    [Textbox::ROBOTO_LIGHTITALIC] = {
        "Roboto-LightItalic",
        "resources/fonts/Roboto/Roboto-LightItalic.ttf"},
    [Textbox::ROBOTO_THINITALIC] = {
        "Roboto-ThinItalic",
        "resources/fonts/Roboto/Roboto-ThinItalic.ttf"},
    [Textbox::ROBOTO_BOLDITALIC] = {
        "Roboto-BoldItalic",
        "resources/fonts/Roboto/Roboto-BoldItalic.ttf"},
    [Textbox::ROBOTO_MEDIUMITALIC] = {
        "Roboto-MediumItalic",
        "resources/fonts/Roboto/Roboto-MediumItalic.ttf"},


    [Textbox::ROBOTOCONDENSED_REGULAR] = {
        "RobotoCondensed-Regular",
        "resources/fonts/Roboto/RobotoCondensed-Regular.ttf"},
    [Textbox::ROBOTOCONDENSED_ITALIC] = {
        "RobotoCondensed-Italic",
        "resources/fonts/Roboto/RobotoCondensed-Italic.ttf"},
    [Textbox::ROBOTOCONDENSED_BOLD] = {
        "RobotoCondensed-Bold",
        "resources/fonts/Roboto/RobotoCondensed-Bold.ttf"},
    [Textbox::ROBOTOCONDENSED_LIGHT] = {
        "RobotoCondensed-Light",
        "resources/fonts/Roboto/RobotoCondensed-Light.ttf"},
    [Textbox::ROBOTOCONDENSED_BOLDITALIC] = {
        "RobotoCondensed-BoldItalic",
        "resources/fonts/Roboto/RobotoCondensed-BoldItalic.ttf"},
    [Textbox::ROBOTOCONDENSED_LIGHTITALIC] = {
        "RobotoCondensed-LightItalic",
        "resources/fonts/Roboto/RobotoCondensed-LightItalic.ttf"}
};

const int Textbox::size_lookup[] = {
    [Textbox::SMALL] = 10,
    [Textbox::MEDIUM] = 18,
    [Textbox::LARGE] = 28
};

GLuint Textbox::glprogram = 0;
GLuint Textbox::uniform_texture = 0;
GLuint Textbox::uniform_window_size = 0;
GLuint Textbox::uniform_offset = 0;
GLuint Textbox::uniform_size = 0;
GLuint Textbox::attribute_pos = 0;
GLuint Textbox::attribute_texcoord_vert = 0;
GLuint Textbox::vertices_buf = 0;

char *Textbox::base_path = 0;

const char *Textbox::shader_vertex = "\
#version 100\n\
precision mediump float;\n\
attribute vec2 texcoord_vert;\n\
varying vec2 texcoord_frag;\n\
uniform vec2 window_size;\n\
uniform vec2 offset;\n\
uniform vec2 size;\n\
void main() {\n\
	texcoord_frag = texcoord_vert;\n\
    vec2 p = (size*texcoord_vert + offset)/window_size;\n\
    p.y = 1.0 - p.y;\n\
    p = p * vec2(2.0,2.0) - vec2(1.0,1.0);\n\
	gl_Position = vec4(p, 0.0, 1.0);\n\
}\
";

const char *Textbox::shader_fragment = "\
#version 100\n\
precision mediump float;\n\
varying vec2 texcoord_frag;\n\
uniform sampler2D texture;\n\
void main()\n\
{\n\
    gl_FragColor = texture2D(texture, texcoord_frag);\n\
}\
";

Textbox::Status Textbox::init()
{
    gl_program_load_str(&glprogram, shader_vertex, shader_fragment);
    uniform_texture = glGetUniformLocation(glprogram, "texture");
    uniform_window_size = glGetUniformLocation(glprogram, "window_size");
    uniform_offset = glGetUniformLocation(glprogram, "offset");
    uniform_size = glGetUniformLocation(glprogram, "size");
    attribute_pos = glGetAttribLocation(glprogram, "pos");
    attribute_texcoord_vert = glGetAttribLocation(glprogram, "texcoord_vert");
    glGenBuffers(1, &vertices_buf);

    base_path = SDL_GetBasePath();

    GLfloat vertices_data[] = {
        0, 0,
        0, 1,
        1, 0,

        1, 1,
        1, 0,
        0, 1
    };

    glBindBuffer(GL_ARRAY_BUFFER, vertices_buf);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertices_data, GL_STATIC_DRAW);

    return OK;
}

void Textbox::cleanup()
{
    glDeleteProgram(glprogram);
    glDeleteBuffers(1, &vertices_buf);
    free(base_path);
}

Textbox::Textbox(int x, int y, int w, int h, Font font, Size size, SDL_Color &color, const std::string &text, Flags flags)
    :x(x), y(y), w(w), h(h),
     font(font),
     size(size),
     color(color),
     text(text),
     flags(flags)
{
	glGenTextures(1, &texture);
    redraw();
}

Textbox::~Textbox()
{
    glDeleteTextures(1, &texture);
}

void Textbox::redraw()
{
    char *file = (char *)calloc(1024, sizeof(char));
    strncat(file, base_path, 1024);
    strncat(file, ttf_info[font].path, 1024 - strlen(file));

    TTF_Font *font_sdl = TTF_OpenFont(file, size_lookup[size]);

    if(!font_sdl)
    {
        Logger::stdout.log(Logger::ERROR) << "Unable to load textbox_font: " << file << Logger::MessageStream::endl;
        //TODO: prevent render() when Textbox is invalid
        return;
    }

    free(file);

    SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font_sdl, text.c_str(), color, w);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_Surface *new_surface = SDL_CreateRGBSurface(0, surface->w, surface->h,
                                                    32,
                                                    0xFF000000,
                                                    0x00FF0000,
                                                    0x0000FF00,
                                                    0x000000FF
                                                    );

    SDL_BlitSurface(surface, NULL, new_surface, NULL);

    SDL_FreeSurface(surface);
    surface = new_surface;

    if(w < surface->w)
        Logger::stdout.log(Logger::WARN) << "Textbox: string \"" << text << "\" wider (by " << surface->w - w << "px) than textbox" << Logger::MessageStream::endl;

    if(h < surface->h)
        Logger::stdout.log(Logger::WARN) << "Textbox: string \"" << text << "\" taller (by " << surface->h - h << "px) than textbox" << Logger::MessageStream::endl;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, surface->w, surface->h,
                 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, surface->pixels);

    if(flags & CENTER_TEXT_H)
        offset.x = (w - surface->w) / 2.0f;

    if(flags & CENTER_TEXT_V)
        offset.y = (h - surface->h) / 2.0f;

    surf_w = surface->w;
    surf_h = surface->h;

    SDL_FreeSurface(surface);

    TTF_CloseFont(font_sdl);
}

void Textbox::render()
{
    glDisable(GL_DEPTH_TEST);
    glUseProgram(glprogram);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(uniform_texture, 0);

    int window_w, window_h;
    StateWindow::instance()->get_dimensions(&window_w, &window_h);

    GLfloat vec[2] = {(float)window_w, (float)window_h};
    glUniform2fv(uniform_window_size, 1, vec);
    vec[0] = x + offset.x;
    vec[1] = y + offset.y;
    glUniform2fv(uniform_offset, 1, vec);
    vec[0] = surf_w;
    vec[1] = surf_h;
    glUniform2fv(uniform_size, 1, vec);

    glBindBuffer(GL_ARRAY_BUFFER, vertices_buf);
    glVertexAttribPointer(attribute_texcoord_vert, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnableVertexAttribArray(attribute_texcoord_vert);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(attribute_texcoord_vert);
}
