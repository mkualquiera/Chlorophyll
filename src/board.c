#if INTERFACE
#include "SDL2/SDL.h"
#include "colors.h"
#define CELL_PADDING 4
#define CELL_SIDE 32
typedef struct Board {
  unsigned char** data;
  unsigned char width;
  unsigned char height;
}
#endif

#include <stdlib.h>
#include "board.h"
#include <stdio.h>
#include <time.h>

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
  srand(time(0));
  Board result;
  result.width = width;
  result.height = height;
  result.data = Board_Allocate(width, height);
  for (int x = 0; x < width; ++x){
    for (int y = 0; y < height; ++y) {
      unsigned char random = rand() % 0xFF;
      if (random <= density) {
        Board_Set(&result, x, y, 0x0F);
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

// Calculate the risk for a given cell.
unsigned char Board_Risk (Board* target, unsigned char x, unsigned char y) {
  unsigned char result = 0;
  if (x < 0 || x >= target->width || y < 0 || y >= target->height) {
    return result;
  }
  if (Board_Get(target, x-1, y-1) == 0x0F | Board_Get(target, x-1, y-1) == 0xFF) {
    result++;
  }
  if (Board_Get(target, x, y-1) == 0x0F | Board_Get(target, x, y-1) == 0xFF) {
    result++;
  }
  if (Board_Get(target, x+1, y-1) == 0x0F | Board_Get(target, x+1, y-1) == 0xFF) {
    result++;
  }
  if (Board_Get(target, x-1, y) == 0x0F | Board_Get(target, x-1, y) == 0xFF) {
    result++;
  }
  if (Board_Get(target, x+1, y) == 0x0F | Board_Get(target, x+1, y) == 0xFF) {
    result++;
  }
  if (Board_Get(target, x-1, y+1) == 0x0F | Board_Get(target, x-1, y+1) == 0xFF) {
    result++;
  }
  if (Board_Get(target, x, y+1) == 0x0F | Board_Get(target, x, y+1) == 0xFF) {
    result++;
  }
  if (Board_Get(target, x+1, y+1) == 0x0F | Board_Get(target, x+1, y+1) == 0xFF ) {
    result++;
  }
  if (result == 0x00) { result = 0x09; }
  return result;
}

// Flag a cell on the board
void Board_FlagCell (Board* target, unsigned char x, unsigned char y) {
  if (Board_Get(target,x,y) == 0x0F) {
    Board_Set(target,x,y,0xFF);
    return;
  }
  if (Board_Get(target,x,y) == 0x00) {
    Board_Set(target,x,y,0xF0);
    return;
  }
  if (Board_Get(target,x,y) == 0xFF) {
    Board_Set(target,x,y,0x0F);
    return;
  }
  if (Board_Get(target,x,y) == 0xF0) {
    Board_Set(target,x,y,0x00);
    return;
  }
}

// Tick the board on a specific cell
unsigned char Board_TickCell (Board* target, unsigned char x, unsigned char y) {
  if (x < 0 || x >= target->width || y < 0 || y >= target->height) {
    return 0;
  }
  if (Board_Get(target,x,y) == 0x0F) {
    return 1;
  }
  if (Board_Get(target,x,y)==0x00) {
    unsigned char value = Board_Risk(target,x,y);
    Board_Set(target,x,y,value);
    if (value == 0x09) {
      Board_TickCell(target, x-1, y-1);
      Board_TickCell(target, x, y-1);
      Board_TickCell(target, x+1, y-1);
      Board_TickCell(target, x-1, y);
      Board_TickCell(target, x+1, y);
      Board_TickCell(target, x-1, y+1);
      Board_TickCell(target, x, y+1);
      Board_TickCell(target, x+1, y+1);
    }
  }
  return 0;
}

// Board drawing
void Board_Draw (SDL_Renderer* renderer, Board* target, Color unknown_tiles, Color mined_tiles, Color selected_tile, Color known_tiles, Color flagged_tiles, Color selected_flagged_tile, unsigned char selected_x, unsigned char selected_y, unsigned char dead) {
  for (int x = 0; x < target->width; x++){
    for (int y = 0; y < target->height; y++){
      SDL_Rect r;
      r.x = (x * CELL_SIDE) + (CELL_PADDING * 2);
      r.y = (y * CELL_SIDE) + (CELL_PADDING * 2);
      r.w = CELL_SIDE - (CELL_PADDING * 2);
      r.h = CELL_SIDE - (CELL_PADDING * 2);
      unsigned char value = Board_Get(target, x,y);
      Color_Set(renderer, unknown_tiles);
      if (x == selected_x & y == selected_y) {
        Color_Set(renderer, selected_tile);
      }
      if (value >= 0x01 & value <= 0x08) {
        Color_Set(renderer, known_tiles);
        unsigned char bit_one   = value % 0x2;
        unsigned char bit_two   = (value / 0x2) % 0x2;
        unsigned char bit_three = (value / 0x4) % 0x2;
        if (bit_three) {
          SDL_Rect rbone;
          rbone.x = r.x;
          rbone.y = r.y;
          rbone.w = r.w/0x3;
          rbone.h = r.h;
          SDL_RenderFillRect(renderer, &rbone);
        }
        if (bit_two) {
          SDL_Rect rbtwo;
          rbtwo.x = r.x + r.w/0x3;
          rbtwo.y = r.y;
          rbtwo.w = r.w/0x3;
          rbtwo.h = r.h;
          SDL_RenderFillRect(renderer, &rbtwo );
        }
        if (bit_one) {
          SDL_Rect rbthree;
          rbthree.x = r.x + (r.w*0x2/0x3);
          rbthree.y = r.y;
          rbthree.w = r.w/0x3;
          rbthree.h = r.h;
          SDL_RenderFillRect(renderer, &rbthree);
        }
      }
      if (value == 0xFF | value == 0xF0) {
        Color_Set(renderer, flagged_tiles);
        if (x == selected_x & y == selected_y) {
          Color_Set(renderer, selected_flagged_tile);
        }
      }
      if (dead) {
          if (value == 0x0F) {
            Color_Set(renderer, mined_tiles);
          }
      }
      if (value == 0x00 | value == 0x0F | value == 0xFF | value == 0xF0) {
        SDL_RenderFillRect(renderer, &r);
      }
    }
  }
}
