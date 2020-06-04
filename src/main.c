#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>
#include "constants.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

struct Player {
    float x;
    float y;
    float width;
    float height;

    int turnDirection; // -1 for left, 1 for right
    int walkDirection; // -1 for back, 1 for front

    float rotationAngle;
    float walkSpeed;
    float turnSpeed;
} player;

// Variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int IsGameRunning = FALSE;
int TicksLastFrame = 0;

// Functions
int InitializeWindow();
void DestroyWindow();
void Begin();
void ProcessInput();

void Update();
void MovePlayer(float DeltaTime);

void Render();
void RenderMap();
void RenderPlayer();
void RenderRays();

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
    
    // Setting up Player
    player.x = WINDOW_WIDTH / 2;
    player.y = WINDOW_HEIGHT / 2;
    player.width = 5;
    player.height = 5;
    player.turnDirection = 0;
    player.walkDirection = 0;
    player.rotationAngle = PI / 2;

    player.walkSpeed = 200;
    player.turnSpeed = 90 * (PI / 180);
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

            if(event.key.keysym.sym == SDLK_UP) {
                player.walkDirection = 1;
                break;
            }

            if(event.key.keysym.sym == SDLK_DOWN) {
                player.walkDirection = -1;
                break;
            }

            if(event.key.keysym.sym == SDLK_RIGHT) {
                player.turnDirection = 1;
                break;
            }

            if(event.key.keysym.sym == SDLK_LEFT) {
                player.turnDirection = -1;
                break;
            }
        }

        case SDL_KEYUP: {
            if(event.key.keysym.sym == SDLK_UP) {
                player.walkDirection = 0;
                break;
            }

            if(event.key.keysym.sym == SDLK_DOWN) {
                player.walkDirection = 0;
                break;
            }

            if(event.key.keysym.sym == SDLK_RIGHT) {
                player.turnDirection = 0;
                break;
            }

            if(event.key.keysym.sym == SDLK_LEFT) {
                player.turnDirection = 0;
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

    // Always remember to update game objects as a function of delta time
    MovePlayer(DeltaTime);
}

int MapHasWallAt(float x, float y) {
    if(x < 0 || x > WINDOW_WIDTH || y < 0 || y > WINDOW_HEIGHT) {
        return TRUE;
    }

    int mapGridIndexX = floor(x / TILE_SIZE);
    int mapGridIndexY = floor(y / TILE_SIZE);

    return map[mapGridIndexY][mapGridIndexX] != 0;
}

void MovePlayer(float DeltaTime) {
    player.rotationAngle += player.turnDirection * player.turnSpeed * DeltaTime;
    float moveStep = player.walkDirection * player.walkSpeed * DeltaTime;

    float newPlayerX = player.x + cos(player.rotationAngle) * moveStep;
    float newPlayerY = player.y + sin(player.rotationAngle) * moveStep;

    if(!MapHasWallAt(newPlayerX, newPlayerY)) {
        player.x = newPlayerX;
        player.y = newPlayerY;
    }
}

void Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    // TODO: render game objects here.
    RenderMap();
    RenderPlayer();
    // TODO: RenderRays();

    SDL_RenderPresent(renderer);   
}

void RenderMap() {
    for(int i = 0; i < MAP_NUM_ROWS; i++) {
        for(int j = 0; j < MAP_NUM_COLS; j++) {
            int tileX = j * TILE_SIZE;
            int tileY = i * TILE_SIZE;
            int tileColor = map[i][j] != 0 ? 255 : 0;

            SDL_SetRenderDrawColor(renderer, tileColor, tileColor, tileColor, 255);
            SDL_Rect mapTileRect = {
                tileX * MINIMAP_SCALE_FACTOR,
                tileY * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR,
                TILE_SIZE * MINIMAP_SCALE_FACTOR
            }; 
            SDL_RenderFillRect(renderer, &mapTileRect);
        }
    }
}

void RenderPlayer() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_Rect playerRect = {
        player.x * MINIMAP_SCALE_FACTOR,
        player.y * MINIMAP_SCALE_FACTOR,
        player.width * MINIMAP_SCALE_FACTOR,
        player.height * MINIMAP_SCALE_FACTOR
    };

    SDL_RenderFillRect(renderer, &playerRect);
    SDL_RenderDrawLine(renderer, player.x * MINIMAP_SCALE_FACTOR, player.y * MINIMAP_SCALE_FACTOR, player.x + cos(player.rotationAngle) * 45 * MINIMAP_SCALE_FACTOR, player.y + sin(player.rotationAngle) * 45 * MINIMAP_SCALE_FACTOR);
}