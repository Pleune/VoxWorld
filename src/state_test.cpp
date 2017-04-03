#include "state_test.hpp"

#include <SDL.h>
#include "state_window.hpp"
#include "logger.hpp"
#include "textbox.hpp"

StateTest *StateTest::instance_ = NULL;
StateTest::StateTest()
    :fps_limit(120)
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::StateTest()" << Logger::MessageStream::endl;
}

GameState::Status StateTest::init()
{
    World::init();
    Logger::stdout.log(Logger::DEBUG) << "World initaized" << Logger::MessageStream::endl;

    SDL_Color color = {0,255,0,0};
    text = new Textbox(10, 10, 400, 50, Textbox::ROBOTO_REGULAR, Textbox::MEDIUM, color, "Press ESC to quit\nPress 'i' to toggle mouse capture", Textbox::NONE);
    world = new World();
    rot = {0,0};
    camera = {
        {0,0,0},
        {0,0,0},
        {0,0,0}
    };

    fps_limit.mark();

    keyboard = SDL_GetKeyboardState(0);

    SDL_SetRelativeMouseMode(SDL_TRUE);

	return OK;
}

void StateTest::cleanup()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::cleanup()" << Logger::MessageStream::endl;

    SDL_SetRelativeMouseMode(SDL_FALSE);

    delete text;
    delete world;

    World::cleanup();
    Logger::stdout.log(Logger::DEBUG) << "Chunk de-initaized" << Logger::MessageStream::endl;
}

GameState::Status StateTest::resume()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::resume()" << Logger::MessageStream::endl;
    SDL_SetRelativeMouseMode(SDL_TRUE);
	return OK;
}

GameState::Status StateTest::pause()
{
	Logger::stdout.log(Logger::DEBUG) << "StateTest::pause()" << Logger::MessageStream::endl;
    SDL_SetRelativeMouseMode(SDL_FALSE);
	return OK;
}

void StateTest::input()
{
    vec3_t inputvec = {0,0,0};

    if(keyboard[SDL_SCANCODE_W])
        inputvec.z -= 1;
    if(keyboard[SDL_SCANCODE_A])
        inputvec.x -= 1;
    if(keyboard[SDL_SCANCODE_S])
        inputvec.z += 1;
    if(keyboard[SDL_SCANCODE_D])
        inputvec.x += 1;
    if(keyboard[SDL_SCANCODE_LSHIFT])
        inputvec.y -= 1;
    if(keyboard[SDL_SCANCODE_SPACE])
        inputvec.y += 1;

    camera.forward.x = std::sin(rot.x) * std::cos(rot.y);
    camera.forward.y = std::sin(rot.y);
    camera.forward.z = -std::cos(rot.x) * std::cos(rot.y);

    vec3_t rotatevec;
    rotatevec.x = std::cos(rot.x)*inputvec.x - std::sin(rot.x)*inputvec.z;
    rotatevec.z = std::sin(rot.x)*inputvec.x + std::cos(rot.x)*inputvec.z;
    rotatevec.y = inputvec.y;

    #define PLAYER_SPEED 1
    rotatevec.x *= PLAYER_SPEED;
    rotatevec.y *= PLAYER_SPEED;
    rotatevec.z *= PLAYER_SPEED;

    camera.pos.x += rotatevec.x;
    camera.pos.y += rotatevec.y;
    camera.pos.z += rotatevec.z;

    if(keyboard[SDL_SCANCODE_X])
        for(float x = -1; x < 1; x+=.1)
            for(float y = -1; y < 1; y+=.1)
                for(float z = -1; z < 1; z+=.1)
                    world->blockpick_set(Block::AIR, camera.pos, {x, y, z}, false, 40, false);

    if(keyboard[SDL_SCANCODE_E])
        for(int i=0; i<10; i++)
            world->blockpick_set(Block::SAND, camera.pos, camera.forward, true, 200, false);
}

void StateTest::run(GameEngine *engine)
{
    if(queue.exit)
        engine->finish();

    input();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    camera.up = {0, 1, 0};

    world->render(camera);
    text->render();

    StateWindow::instance()->swap();

    GLenum err = GL_NO_ERROR;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        Logger::stdout.log(Logger::ERROR) << "OpenGL Error: " << err << Logger::MessageStream::endl;
    }

    fps_limit.delay();
}

void StateTest::event(SDL_Event *e)
{
    if(e->type == SDL_KEYDOWN)
    {
        switch(e->key.keysym.sym)
        {
        case SDLK_ESCAPE:
            queue.exit = true;
            break;
        case SDLK_i:
            if(takeinput)
            {
                SDL_SetRelativeMouseMode(SDL_FALSE);
                takeinput = false;
            } else {
                SDL_SetRelativeMouseMode(SDL_TRUE);
                takeinput = true;
            }
            break;
        }
    }

    else if(e->type == SDL_MOUSEBUTTONDOWN)
    {
        if(e->button.button == SDL_BUTTON_LEFT)
            world->blockpick_set(Block::SAND, camera.pos, camera.forward, true, 1000, true);
        else if(e->button.button == SDL_BUTTON_RIGHT)
            world->blockpick_set(Block::AIR, camera.pos, camera.forward, false, 1000, true);
    }

    else if(e->type == SDL_WINDOWEVENT)
    {
        if(e->window.event == SDL_WINDOWEVENT_RESIZED)
        {
            StateWindow::instance()->update_size();
            world->update_window_size();
        }
    }

    else if(e->type == SDL_MOUSEMOTION)
    {
        if(takeinput)
        {
            float deltamousex = e->motion.xrel;
            float deltamousey = e->motion.yrel;

            #define MOUSE_SENSITIVITY (1.0/600.0)

            rot.x += MOUSE_SENSITIVITY*deltamousex;
            rot.y -= MOUSE_SENSITIVITY*deltamousey;

            rot.y = rot.y > M_PI/2-.005 ? M_PI/2-.005 : rot.y;
            rot.y = rot.y < -M_PI/2+.005 ? -M_PI/2+.005 : rot.y;

            rot.x = rot.x > M_PI*2 ? rot.x - M_PI*2: rot.x;
            rot.x = rot.x < -M_PI*2 ? rot.x + M_PI*2: rot.x;
        }
    }
}

std::string StateTest::getname()
{
    return "test";
}

StateTest *StateTest::instance()
{
	if(instance_ == NULL)
		instance_ = new StateTest();
	return instance_;
}
