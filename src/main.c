#include <SDL2/SDL.h>
#include <stdio.h>
#include "constants.h"

// Variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int IsGameRunning = FALSE;
int TicksLastFrame = 0;

int PlayerX, PlayerY;

// Functions
int InitializeWindow();
void DestroyWindow();
void Begin();
void ProcessInput();
void Update();
void Render();

int main(int argc, char* argv[]) {
    IsGameRunning = InitializeWindow();

    Begin();
    while(IsGameRunning) {
        ProcessInput();
        Update();
        Render();
    }

    DestroyWindow();
    return 0;
}

int InitializeWindow() {
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL2.\n");
        return FALSE;
    }

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0
    );

    if(window == NULL) {
        fprintf(stderr, "Error creating SDL window.\n");
        return FALSE;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if(renderer == NULL) {
        fprintf(stderr, "Error creating SDL renderer.\n");
        return FALSE;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    return TRUE;
}

void DestroyWindow() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Begin() {
    PlayerX = 0;
    PlayerY = 0;
}

void ProcessInput() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch(event.type) {
        case SDL_QUIT: {
            IsGameRunning = FALSE;
            break;
        }
        case SDL_KEYDOWN: {
            if(event.key.keysym.sym == SDLK_ESCAPE) {
                IsGameRunning = FALSE;
                break;
            }
        }
    }
}

void Update() {
    // compute how long we have to wait until we reach the target frame time in milliseconds
    int TimeToWait = FRAME_TIME - (SDL_GetTicks() - TicksLastFrame);

    // Only delay execution if we are running too fast
    if(TimeToWait > 0 && TimeToWait <= FRAME_TIME) {
        SDL_Delay(TimeToWait);
    }

    // Computing current DeltaTime
    float DeltaTime = (SDL_GetTicks() - TicksLastFrame) / 1000.0f;
    TicksLastFrame = SDL_GetTicks();

    // Update stuff
    PlayerX += 50 * DeltaTime;
    PlayerY += 50 * DeltaTime;
}

void Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // TODO: Render all gameobjects for this frame
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect Rect = { PlayerX, PlayerY, 32, 32 };
    SDL_RenderFillRect(renderer, &Rect);

    SDL_RenderPresent(renderer);   
}