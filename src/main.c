#include <SDL2/SDL.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>

#include "constants.h"
#include "player.h"
#include "ray.h"
#include "mathaux.h"
#include "upng/upng.h"

const int map[MAP_NUM_ROWS][MAP_NUM_COLS] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 ,1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 2, 0, 3, 0, 4, 0, 5, 0, 6, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 5, 5, 5, 5, 5}
};

Player player;
Ray rays[NUM_RAYS];

// ////////////////////////////////////
// Variables
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int IsGameRunning = FALSE;
int TicksLastFrame = 0;
Uint32* colorBuffer = NULL;
SDL_Texture* colorBufferTexture = NULL;
float TimeSinceBeginning = 0;

// Wall textures
// TODO: Multiple wall textures loading from png files
Uint32* wallTexture[NUM_TEXTURES];
upng_t* pngTexture;

// weapon?
upng_t* pistolSprite;
Uint32* pistolTexture;

// ////////////////////////////////////
// Functions
int InitializeWindow();
void DestroyWindow();
void Begin();
void ProcessInput();

void Update();
void MovePlayer(float DeltaTime);
void CastAllRays();
void CastRay(float angle, int rayID);
int MapHasWallAt(float x, float y);

void Render();
void ClearColorBuffer(Uint32 color);
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
    free(colorBuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Begin() {
    // Setting up Player
    SetPlayerPosition(&player, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
    SetPlayerSize(&player, 1, 1);
    player.turnDirection = 0;
    player.walkDirection = 0;
    player.rotationAngle = PI / 2;
    player.walkSpeed = 200;
    player.turnSpeed = 90 * (PI / 180);

    // allocating the total amount of memory to hold the color buffer
    colorBuffer = (Uint32*) malloc( sizeof(Uint32) * (Uint32)WINDOW_WIDTH * (Uint32)WINDOW_HEIGHT );

    // create an sdl texture to display the color buffer
    colorBufferTexture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,       // (R, G, B, A)
        SDL_TEXTUREACCESS_STREAMING,  //  changes frequently, lockable
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    for(int i = 0; i < NUM_TEXTURES; i++) {    
        // allocating memory for the wall texture (I don't think I need to do that as the png already allocate memory for the decoded thing)
        // wallTexture[i] = (Uint32*) malloc( sizeof(Uint32) * (Uint32)TEX_WIDTH * (Uint32)TEX_HEIGHT );

        // load an external texture using the upng library to decode the file.
        pngTexture = upng_new_from_file(allTextures[i]);
        if(pngTexture != NULL) {
            upng_decode(pngTexture);
            if(upng_get_error(pngTexture) == UPNG_EOK) {
                wallTexture[i] = (Uint32*) upng_get_buffer(pngTexture);
            }
        }
    }

    pistolSprite = upng_new_from_file("./images/doom-pistol.png");
    if(pistolSprite != NULL) {
        upng_decode(pistolSprite);
        if(upng_get_error(pistolSprite) == UPNG_EOK) {
            pistolTexture = (Uint32*) upng_get_buffer(pistolSprite);
        }
    }
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
    TimeSinceBeginning += DeltaTime;

    // Always remember to update game objects as a function of delta time
    MovePlayer(DeltaTime);
    CastAllRays();
}

void CastAllRays() {
    float rayAngle = player.rotationAngle - (FOV_ANGLE / 2);

    for(int rayID = 0; rayID < NUM_RAYS; rayID++) {
        CastRay(rayAngle, rayID);
        rayAngle += (FOV_ANGLE / NUM_RAYS);
    }
}

void CastRay(float rayAngle, int rayID) {
    rayAngle = NormalizeAngle(rayAngle);


    int isRayFacingDown = (rayAngle > 0 && rayAngle < PI);
    int isRayFacingUp = !isRayFacingDown;

    int isRayFacingRight = (rayAngle < 0.5 * PI || rayAngle > 1.5 * PI);
    int isRayFacingLeft = !isRayFacingRight;

    float xintercept, yintercept;
    float xstep, ystep;

    // /////////////////////////////////
    // Horizontal Intersection Part
    // /////////////////////////////////
    int foundHorizontalWallHit = FALSE;
    float horizontalWallHitX = 0;
    float horizontalWallHitY = 0;
    int horizontalWallContent = 0;

    // Find the y-coordinate of the closest horizontal grid intersection
    yintercept = floor(player.position.y / TILE_SIZE) * TILE_SIZE;
    yintercept += isRayFacingDown ? TILE_SIZE : 0;

    // Find the x-coordinate of the closest horizontal grid intersection
    xintercept = player.position.x + (yintercept - player.position.y) / tan(rayAngle);

    // Calculate increments xstep and ystep
    ystep = TILE_SIZE;
    ystep *= isRayFacingUp ? -1 : 1;

    xstep = TILE_SIZE / tan(rayAngle);
    xstep *= (isRayFacingLeft && xstep > 0) ? -1 : 1;
    xstep *= (isRayFacingRight && xstep < 0) ? -1 : 1;

    float nextHorizontalTouchX = xintercept;
    float nextHorizontalTouchY = yintercept;

    // Increment xstep and ystep until we find a wall
    while(nextHorizontalTouchX > 0 && nextHorizontalTouchX < WINDOW_WIDTH && nextHorizontalTouchY > 0 && nextHorizontalTouchY < WINDOW_HEIGHT) {
        float xToCheck = nextHorizontalTouchX;
        float yToCheck = nextHorizontalTouchY + (isRayFacingUp ? -1 : 0);

        if(MapHasWallAt(xToCheck, yToCheck)) {
            // found a wall hit
            horizontalWallHitX = nextHorizontalTouchX;
            horizontalWallHitY = nextHorizontalTouchY;
            horizontalWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
            foundHorizontalWallHit = TRUE;
            break;
        } else {
            nextHorizontalTouchX += xstep;
            nextHorizontalTouchY += ystep;
        }
    }

    // /////////////////////////////////
    // Vertical Intersection Part
    // /////////////////////////////////
    int foundVerticalWallHit = FALSE;
    float verticalWallHitX = 0;
    float verticalWallHitY = 0;
    int verticalWallContent = 0;

    // Find the x-coordinate of the closest horizontal grid intersection
    xintercept = floor(player.position.x / TILE_SIZE) * TILE_SIZE;
    xintercept += isRayFacingRight ? TILE_SIZE : 0;

    // Find the y-coordinate of the closest horizontal grid intersection
    yintercept = player.position.y + (xintercept - player.position.x) * tan(rayAngle);

    // Calculate increments xstep and ystep
    xstep = TILE_SIZE;
    xstep *= isRayFacingLeft ? -1 : 1;

    ystep = TILE_SIZE * tan(rayAngle);
    ystep *= (isRayFacingUp && ystep > 0) ? -1 : 1;
    ystep *= (isRayFacingDown && ystep < 0) ? -1 : 1;

    float nextVerticalTouchX = xintercept;
    float nextVerticalTouchY = yintercept;

    // Increment xstep and ystep until we find a wall
    while(nextVerticalTouchX > 0 && nextVerticalTouchX < WINDOW_WIDTH && nextVerticalTouchY > 0 && nextVerticalTouchY < WINDOW_HEIGHT) {
        float xToCheck = nextVerticalTouchX + (isRayFacingLeft ? -1 : 0);
        float yToCheck = nextVerticalTouchY;

        if(MapHasWallAt(xToCheck, yToCheck)) {
            // found a wall hit
            verticalWallHitX = nextVerticalTouchX;
            verticalWallHitY = nextVerticalTouchY;
            verticalWallContent = map[(int)floor(yToCheck / TILE_SIZE)][(int)floor(xToCheck / TILE_SIZE)];
            foundVerticalWallHit = TRUE;
            break;
        } else {
            nextVerticalTouchX += xstep;
            nextVerticalTouchY += ystep;
        }
    }

    // Calculate both horizontal and vertical hit distance and choose the smallest one
    float horizontalHitDistance = foundHorizontalWallHit ? DistanceBetweenPoints(player.position.x, player.position.y, horizontalWallHitX, horizontalWallHitY) : INT_MAX; 
    float verticalHitDistance = foundVerticalWallHit ? DistanceBetweenPoints(player.position.x, player.position.y, verticalWallHitX, verticalWallHitY) : INT_MAX;

    if(verticalHitDistance < horizontalHitDistance) {
        rays[rayID].distance = verticalHitDistance;
        rays[rayID].wallHitX = verticalWallHitX;
        rays[rayID].wallHitY = verticalWallHitY;
        rays[rayID].wallHitContent = verticalWallContent;
        rays[rayID].wasHitVertical = TRUE;
    } else {
        rays[rayID].distance = horizontalHitDistance;
        rays[rayID].wallHitX = horizontalWallHitX;
        rays[rayID].wallHitY = horizontalWallHitY;
        rays[rayID].wallHitContent = horizontalWallContent;
        rays[rayID].wasHitVertical = FALSE;
    }

    rays[rayID].rayAngle = rayAngle;
    rays[rayID].isRayFacingDown = isRayFacingDown;
    rays[rayID].isRayFacingLeft = isRayFacingLeft;
    rays[rayID].isRayFacingUp = isRayFacingUp;
    rays[rayID].isRayFacingRight = isRayFacingRight;
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

    float newPlayerX = player.position.x + cos(player.rotationAngle) * moveStep;
    float newPlayerY = player.position.y + sin(player.rotationAngle) * moveStep;

    // TODO: Check Collision should be done in a different function?!
    if(!MapHasWallAt(newPlayerX, newPlayerY)) {
        SetPlayerPosition(&player, newPlayerX, newPlayerY);
    }
}

void RenderColorBuffer() {
    SDL_UpdateTexture(colorBufferTexture, NULL, colorBuffer, (int)(Uint32)WINDOW_WIDTH * sizeof(Uint32));
    SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
}

void Generate3DProjection() {
    for(int i = 0; i < NUM_RAYS; i++) {
        // getting the correct distance (removing fish-eye effect)
        float perpendicularDistance = rays[i].distance * cos(rays[i].rayAngle - player.rotationAngle);

        float distanceToProjectionPlane = (WINDOW_WIDTH / 2) / tan(FOV_ANGLE / 2);
        float projectedWallHeight = (TILE_SIZE / perpendicularDistance) * distanceToProjectionPlane;
        int wallStripHeight = (int)projectedWallHeight;

        int wallTopPixel = (WINDOW_HEIGHT / 2) - (wallStripHeight / 2);
        wallTopPixel = (wallTopPixel < 0) ? 0 : wallTopPixel;

        int wallBottomPixel = (WINDOW_HEIGHT / 2) + (wallStripHeight / 2);
        wallBottomPixel = (wallBottomPixel > WINDOW_HEIGHT) ? WINDOW_HEIGHT : wallBottomPixel;

        // set the color for the ceiling
        for(int y = 0; y < wallTopPixel; y++) {
            colorBuffer [(WINDOW_WIDTH * y) + i] = 0xFF333333;
        }

        // Render the wall from wallTopPixel to WallBottomPixel
        float textureOffsetX;
        if(rays[i].wasHitVertical) {
            // perform offset for the vertical hit
            textureOffsetX = (int)rays[i].wallHitY % TILE_SIZE;
        } else {
            // perform offset for the horizontal hit
            textureOffsetX = (int)rays[i].wallHitX % TILE_SIZE;
        }

        // get the correct texture ID number from the map content
        int textureNumber = rays[i].wallHitContent - 1;
        for(int y = wallTopPixel; y < wallBottomPixel; y++) {
            // set the color of the wall based on the color from the texture
            int distanceFromTop = (y + (wallStripHeight / 2) - (WINDOW_HEIGHT / 2));
            float textureOffsetY = distanceFromTop * ((float)TEX_HEIGHT / (float)wallStripHeight);
            Uint32 texelColor = wallTexture[textureNumber][(TEX_WIDTH * (int)textureOffsetY) + (int)textureOffsetX];
            // Uint32 texelColor = wallTexture[(TEX_WIDTH * (int)textureOffsetY) + (int)textureOffsetX];
            colorBuffer [(WINDOW_WIDTH * y) + i] = texelColor;
        }

        // set the color for the ground
        for(int y = wallBottomPixel; y < WINDOW_HEIGHT; y++) {
            colorBuffer [(WINDOW_WIDTH * y) + i] = 0xFF777777;
        }
    }
}

void Render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    Generate3DProjection();
    RenderColorBuffer();
    ClearColorBuffer(0xFF000000);

    // display the minimap
    RenderMap();
    RenderPlayer();
    RenderRays();

    // Rendering the pistol here... just a nice thing...
    SDL_Surface* pistolSurface = SDL_CreateRGBSurfaceWithFormatFrom(pistolTexture, 64, 64, 4 * 64, sizeof(Uint32) * 64, SDL_PIXELFORMAT_RGBA32);
    SDL_Texture* myPistolTexture = SDL_CreateTextureFromSurface(renderer, pistolSurface);
    SDL_FreeSurface(pistolSurface);
    // TODO: Remove those magic numbers?!
    SDL_Rect Myrect = {0, 0, 64, 64};

    float xOffset = 0;
    float yOffset = 0;
    if(player.walkDirection != 0) {
        xOffset = cos(TimeSinceBeginning * 5.0f);
        yOffset = sin(TimeSinceBeginning * 10.0f);
    }

    SDL_Rect MyRect2 = {
        (WINDOW_WIDTH / 2) - (64 * 4 / 2) + (xOffset * 32), 
        (WINDOW_HEIGHT - (64 * 4)) + 32 - (yOffset * 32), 
        64 * 4, 
        64 * 4
    };

    SDL_RenderCopy(renderer, myPistolTexture, &Myrect, &MyRect2);

    SDL_RenderPresent(renderer);   
}

void ClearColorBuffer(Uint32 color) {
    for(int x = 0; x < WINDOW_WIDTH; x++) {
        for(int y = 0; y < WINDOW_HEIGHT; y++) {
            colorBuffer[(WINDOW_WIDTH *  y) + x] = color;
        }
    }
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
        player.position.x * MINIMAP_SCALE_FACTOR,
        player.position.y * MINIMAP_SCALE_FACTOR,
        player.size.x * MINIMAP_SCALE_FACTOR,
        player.size.y * MINIMAP_SCALE_FACTOR
    };

    SDL_RenderFillRect(renderer, &playerRect);
}

void RenderRays() {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    for(int i = 0; i < NUM_RAYS; i++) {
        SDL_RenderDrawLine(
            renderer, 
            player.position.x * MINIMAP_SCALE_FACTOR, 
            player.position.y * MINIMAP_SCALE_FACTOR, 
            rays[i].wallHitX * MINIMAP_SCALE_FACTOR, 
            rays[i].wallHitY * MINIMAP_SCALE_FACTOR
        );
    }
}