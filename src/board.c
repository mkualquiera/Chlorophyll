#if INTERFACE
#include "SDL2/SDL.h"
#include "colors.h"
typedef struct Board {
  unsigned char** data;
  unsigned char width;
  unsigned char height;
}
#endif

#include <stdlib.h>
#include "board.h"
#include <stdio.h>

// Allocate memory for the game board.
unsigned char** Board_Allocate(unsigned char width, unsigned char height) {
  unsigned char** data = malloc(width * sizeof(unsigned char*));
  for (unsigned char column = 0; column < height; ++column) {
    data[column] = malloc(height * sizeof(unsigned char));
    for (unsigned char row = 0; row < height; ++row) {
      data[column][row] = 0x00;
    }
  }
  return data;
}

// Obtain value in the board
unsigned char Board_Get(Board* target, unsigned char x, unsigned char y) {
  if (x < 0 || x >= (*target).width || y < 0 || y >= (*target).height) {
    return 0x00;
  }
  unsigned char* column = (*target).data[x];
  unsigned char result = column[y];
  return result;
}

// Set value in the board
void Board_Set(Board* target, unsigned char x, unsigned char y, unsigned char value) {
  if (x < 0 || x >= (*target).width || y < 0 || y >= (*target).height) {
    return;
  }
  unsigned char* column = (*target).data[x];
  column[y] = value;
}

void Board_Debug(Board* target) {
  for (int x = 0; x < target->width; ++x){
    for (int y = 0; y < target->height; ++y) {
      printf("%02X", Board_Get(target, x, y));
    }
    printf("\n");
  }
}

// Create a board with the given size and density.
Board Board_Create (unsigned char width, unsigned char height, unsigned char density) {
  Board result;
  result.width = width;
  result.height = height;
  result.data = Board_Allocate(width, height);
  for (int x = 0; x < width; ++x){
    for (int y = 0; y < height; ++y) {
      unsigned char random = rand() % 0xFF;
      if (random <= density) {
        Board_Set(&result, x, y, 0xFF);
      }
    }
  }
  return result;
}

// Free the memory used by the board.
void Board_Dispose (Board* target) {
  for (unsigned char column = 0; column < (*target).height; ++column) {
    free((*target).data[column]);
  }
  free((*target).data);
}


#define CELL_PADDING 4
#define CELL_SIDE 32

// Board drawing
void Board_Draw (SDL_Renderer* renderer, Board* target, Color unknown_tiles, Color mined_tiles) {
  for (int x = 0; x < target->width; x++){
    for (int y = 0; y < target->height; y++){
      SDL_Rect r;
      r.x = (x * CELL_SIDE) + (CELL_PADDING * 2);
      r.y = (y * CELL_SIDE) + (CELL_PADDING * 2);
      r.w = CELL_SIDE - (CELL_PADDING * 2);
      r.h = CELL_SIDE - (CELL_PADDING * 2);
      unsigned char value = Board_Get(target, x,y);
      Color_Set(renderer, unknown_tiles);
      if(value == 0xFF)
      {
        Color_Set(renderer, mined_tiles);
      }
      SDL_RenderFillRect(renderer, &r);
    }
  }
}
