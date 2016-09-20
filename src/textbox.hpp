#ifndef BLOCKS_TEXTBOX_H
#define BLOCKS_TEXTBOX_H

#include <string>
#include <SDL.h>
#include <GL/glew.h>

class Textbox {
public:
    enum Status { OK, ERROR };

    enum Font {
        ROBOTO_REGULAR,
        ROBOTO_MEDIUM,
        ROBOTO_ITALIC,
        ROBOTO_BOLD,
        ROBOTO_THIN,
        ROBOTO_LIGHT,
        ROBOTO_BLACK,
        ROBOTO_BLACKITALIC,
        ROBOTO_LIGHTITALIC,
        ROBOTO_THINITALIC,
        ROBOTO_BOLDITALIC,
        ROBOTO_MEDIUMITALIC,

        ROBOTOCONDENSED_REGULAR,
        ROBOTOCONDENSED_ITALIC,
        ROBOTOCONDENSED_BOLD,
        ROBOTOCONDENSED_LIGHT,
        ROBOTOCONDENSED_BOLDITALIC,
        ROBOTOCONDENSED_LIGHTITALIC
    };

    enum Size {
        SMALL,
        MEDIUM,
        LARGE
    };

    enum Flags {
        NONE=0,
        CENTER_TEXT_H=1,
        CENTER_TEXT_V=2
    };

    struct TTFInfo {
        const char *name;
        const char *path;
    };

    static Status init();
    static void cleanup();

    Textbox(int x, int y, int w, int h, Font, Size, SDL_Color &, const std::string &text, Flags);
    ~Textbox();

    void set_pos(int x, int y)             {this->x=x; this->y=y; redraw();}
    void set_dimensions(int w, int h)      {this->w=w; this->h=h; redraw();}
    void set_color(SDL_Color &color)       {this->color = color; redraw();}
    void set_text(const std::string &text) {this->text = text; redraw();}

    void render();

private:
    const static TTFInfo ttf_info[];
    const static int size_lookup[];

    void redraw();

    int x, y;
    int w, h;
    Font font;
    Size size;
    SDL_Color color;
    std::string text;
    Flags flags;

    GLuint texture;

    static GLuint glprogram;
    static GLuint uniform_texture;
    static GLuint uniform_window_size;
    static GLuint uniform_offset;
    static GLuint attribute_pos;
    static GLuint attribute_texcoord_vert;
    static GLuint vertices_buf;

    static char *base_path;

    const static char *shader_vertex;
    const static char *shader_fragment;
};

#endif
