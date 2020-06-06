#ifndef CONSTANTS_H
#define CONSTANTS_H

#define FALSE 0
#define TRUE 1

// Math constants
#define PI 3.14159265
#define TWO_PI 6.28318530

#define TILE_SIZE 64
#define MAP_NUM_ROWS 13
#define MAP_NUM_COLS 20

#define MINIMAP_SCALE_FACTOR .25

#define WINDOW_WIDTH (MAP_NUM_COLS * TILE_SIZE)
#define WINDOW_HEIGHT (MAP_NUM_ROWS * TILE_SIZE)

#define FOV_ANGLE (60 * (PI / 180))

#define NUM_RAYS WINDOW_WIDTH

#define FPS 30
#define FRAME_TIME (1000 / FPS)

#define TEX_WIDTH 64
#define TEX_HEIGHT 64
#define REDBRICK_TEXTURE_FILEPATH "./images/redbrick.png"
#define PURPLESTONE_TEXTURE_FILEPATH "./images/purplestone.png"
#define MOSSYSTONE_TEXTURE_FILEPATH "./images/mossystone.png"
#define GRAYSTONE_TEXTURE_FILEPATH "./images/graystone.png"
#define COLORSTONE_TEXTURE_FILEPATH "./images/colorstone.png"
#define BLUESTONE_TEXTURE_FILEPATH "./images/bluestone.png"
#define WOOD_TEXTURE_FILEPATH "./images/wood.png"
#define EAGLE_TEXTURE_FILEPATH "./images/eagle.png"
#define NUM_TEXTURES 8

// not really a constant but... who cares
const char* allTextures[NUM_TEXTURES] = {
    REDBRICK_TEXTURE_FILEPATH,
    PURPLESTONE_TEXTURE_FILEPATH,
    MOSSYSTONE_TEXTURE_FILEPATH,
    GRAYSTONE_TEXTURE_FILEPATH,
    COLORSTONE_TEXTURE_FILEPATH,
    BLUESTONE_TEXTURE_FILEPATH,
    WOOD_TEXTURE_FILEPATH,
    EAGLE_TEXTURE_FILEPATH
};

#endif