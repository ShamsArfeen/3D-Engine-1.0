#include <SDL.h>
#include <SDL_image.h>

#include <iostream>
#include "3d_matrix.hpp"
using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window *gWindow = NULL;

//The window renderer
SDL_Renderer *gRenderer = NULL;
bool init()
{
    //Initialization mouseClicked
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        //Create window
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

void close()
{
    //Destroy window
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char *args[])
{
    matrix MAT;
    MAT.load_pgn_model();

    //Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
        return 0;
    }

    bool quit = false; //Main loop controller
    SDL_Event e; //Event handler that takes care of all events
    bool mouseClicked = false, screen_updated = false;
    int mouse_pre_x, mouse_pre_y;
    float x_rotate = 0, y_rotate = 0;

    //While application is running
    while (!quit)
    {
        //Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            //Clear screen
            //User requests quit
            if (e.type == SDL_QUIT)
            {
                quit = true;
            }
            if (e.type == SDL_MOUSEMOTION || e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
            {
                //Get mouse position
                int x, y;
                SDL_GetMouseState(&x, &y);

                if (e.type == SDL_MOUSEMOTION)
                {
                    if (mouseClicked == true)
                    {
                        x_rotate = (float)(mouse_pre_x - x) / (float)150.0;
                        y_rotate = (float)(mouse_pre_y - y) / (float)150.0;
                        screen_updated = false;
                    }
                }
                if (e.type == SDL_MOUSEBUTTONDOWN)
                {
                    mouseClicked = true;
                    mouse_pre_x = x;
                    mouse_pre_y = y;
                }
                if (e.type == SDL_MOUSEBUTTONUP)
                {
                    mouseClicked = false;
                    MAT.freeze_mat(x_rotate, y_rotate);
                    x_rotate = 0;
                    y_rotate = 0;
                    //MAT.get_surface(x, y);
                }
            }
        }
        if (screen_updated == false)
        {
            MAT.rotate_mat(x_rotate, y_rotate);
            MAT.render_surfaces(gRenderer);
            //MAT.render_wireframe(gRenderer);

            screen_updated = true;
        }
        //Sleep(1);
    }
    //Free resources and close SDL
    close();
    return 0;
}
