#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "HTMLRenderer.h"
int main(int argc, char *argv[])
{
    HTMLRenderer renderer;

    if (!renderer.init())
    {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return 1;
    }

    renderer.run();
    renderer.cleanup();

    return 0;
}