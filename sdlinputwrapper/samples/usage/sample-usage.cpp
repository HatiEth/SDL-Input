#include <iostream>
#include <SDL2/SDL.h>
#include <cassert>

#include <sdlinputwrapper/sdlinputwrapper.h>
#include <memory>

#include "device.h"
#include <sdlinputwrapper/callback.h>

enum SampleInputActions
{
    SampleDown,
    Shoot,
    CHANGE_CTX,

    GAME_QUIT
};

enum SampleContext
{
    GameMod,
    Game,
    Menu,

    MAX_COUNT
};

void func()
{
    std::cout << "raw c function" << std::endl;
}

void sdlKeyEvent(const SDL_Event& e, sdli::Interface& device)
{
    device.push(sdli::InputType::Keyboard, e.key.keysym.scancode, e.key.state);
}

#include "stub.h"

int main(int argc, char** argv)
{
//    std::setbuf(stdout, NULL); // used to always flush std::cout

    auto sdl_init_status = SDL_Init(SDL_INIT_EVENTS
                                     | SDL_INIT_JOYSTICK
                                     | SDL_INIT_GAMECONTROLLER);
    assert(sdl_init_status==0);

    bool sampleQuit = false;

    auto inputproc = std::unique_ptr<sdli::Processor>(new sdli::Processor(SampleContext::MAX_COUNT));

    auto& device = inputproc->getDevice(sdli::InputType::Keyboard, 0);
    auto& pad = inputproc->getDevice(sdli::InputType::Gamecontroller, 0);

    /** BEG Mappings **/

    auto ctx1 = inputproc->createContext(SampleContext::GameMod);
    auto ctx2 = inputproc->createContext(SampleContext::Game);

    ctx1->mapDigital(SDL_SCANCODE_S, SampleInputActions::SampleDown);
    ctx2->mapDigital(SDL_SCANCODE_S, SampleInputActions::SampleDown);

    ctx1->mapDigital(SDL_SCANCODE_F1, SampleInputActions::CHANGE_CTX);
    ctx2->mapDigital(SDL_SCANCODE_F1, SampleInputActions::CHANGE_CTX);

    ctx1->mapDigital(SDL_CONTROLLER_BUTTON_A, SampleInputActions::SampleDown);
    ctx1->mapDigital(SDL_CONTROLLER_BUTTON_X, SampleInputActions::CHANGE_CTX);

    ctx2->mapDigital(SDL_CONTROLLER_BUTTON_X, SampleInputActions::CHANGE_CTX);
    ctx2->mapDigital(SDL_SCANCODE_ESCAPE, SampleInputActions::GAME_QUIT);

    auto l = []()
    {
        std::cout << "Hello";
    };
    ctx1->addCallback(SampleInputActions::SampleDown, sdli::CallType::OnPress, l);

    auto l2 = [ctx1, ctx2, &device, &pad]()
    {
        device.popContext();
        device.pushContext(ctx2);

        pad.popContext();
        pad.pushContext(ctx2);
        std::cout << "swap to ctx2 while hold" << std::endl;
    };

    ctx2->addCallback(SampleInputActions::GAME_QUIT, sdli::CallType::OnPress, [&sampleQuit](){
        sampleQuit = true;
    });

    ctx1->addCallback(SampleInputActions::CHANGE_CTX, sdli::CallType::OnPress, l2);

    ctx2->addCallback(SampleInputActions::CHANGE_CTX, sdli::CallType::OnRelease, [=, &device, &pad]()
    {
        device.popContext();
        device.pushContext(ctx1);

        pad.popContext();
        pad.pushContext(ctx1);
        std::cout << "change back to ctx1 - released" << std::endl;
    });

    ctx2->addCallback(SampleInputActions::SampleDown, sdli::CallType::OnPress, &func);

    ctx1->addCallback(SampleInputActions::SampleDown, sdli::CallType::OnRelease, []()
    {
        std::cout << " World" << std::endl;
    });
    /** END Mappings **/

    SDL_Window* w = SDL_CreateWindow("SampleWindow", 0, 0, 256, 256, SDL_WindowFlags::SDL_WINDOW_SHOWN|SDL_WindowFlags::SDL_WINDOW_OPENGL);

    auto sdl_glContext = SDL_GL_CreateContext(w);

    device.pushContext(ctx1);
    pad.pushContext(ctx1);


    SDL_Event event;
    while(!sampleQuit)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
            case SDL_QUIT:
                sampleQuit = true;
                break;
            }
            inputproc->handleSdlEvents(event);
        }
//        device.poll();
//        pad.poll();

        inputproc->poll();

        device.dispatch();
//        pad.dispatch();



        SDL_GL_SwapWindow(w);
    }

    SDL_GL_DeleteContext(sdl_glContext);
    SDL_DestroyWindow(w);

    return 0;
}