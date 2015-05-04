/*---------------------------------------------------------------------------------

	Basic template code for starting a DS app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

#include "ball.h"

#include "gfx_ball.h"
#include "gfx_brick.h"
#include "gfx_gradient.h"

//Tiles
  //VRAM E (BG)
  #define tile_empty 0 //tile 0 = empty
  #define tile_brick 1 //tile 1 = brick
  #define tile_gradient 2 // tile 2 to 9 = gradient

  //VRAM F (sprite)
  #define tiles_ball 0 //ball tiles
  #define tile2objram(t) (SPRITE_GFX + (t) * 16)

//PALETTES
  // BG
  #define pal_brick 0 //brick palette (0~15)
  #define pal_gradient 1 //gradient palette (16~31)
  #define backdrop_colour RGB8(190, 225, 255) //bluish color for the backdrop
  #define pal2bgram(p) (BG_PALETTE + (p) * 16)

  //Sprite
  #define pal_ball 0 //sprite palette (0 ~15)
  #define pal2objram(p) (SPRITE_PALETTE + (p) * 16)

//BG_VRAM base address = BG_GFX
#define tile2bgram(t)(BG_GFX + (t) * 16)

#define bg0map ((u16*)BG_MAP_RAM(1))
#define bg1map ((u16*)BG_MAP_RAM(2))


typedef struct t_spriteEntry{
  u16 attr0;
  u16 attr1;
  u16 attr2;
  u16 affine_data;
} spriteEntry;
#define sprites ((spriteEntry*)OAM)

ball g_ball;

void setupInterruptions(void){
  irqInit(); /*interrupts init */
  irqEnable(IRQ_VBLANK); /*enable VBlank interrupt*/
}

void setupGraphics(void){
  vramSetBankE(VRAM_E_MAIN_BG);
  vramSetBankF(VRAM_F_MAIN_SPRITE);

  //clear first tile
  int n;
  for(n = 0; n < 16; n++)
    BG_GFX[n] = 0;

  //copy bg graphics
  //dmaCopyHalfWords:
  //inputs:
  //  1) Channel: 1&2 -> high priority; 
  //              3 -> general purposes; 
  //              0 -> highest priority for critical data
  //  2) source address
  //  3) destination address
  //  4) how much data (bytes)
  dmaCopyHalfWords(3, gfx_brickTiles, tile2bgram(tile_brick), gfx_brickTilesLen);
  dmaCopyHalfWords(3, gfx_gradientTiles, tile2bgram(tile_gradient), gfx_gradientTilesLen);
  dmaCopyHalfWords(3, gfx_brickPal, pal2bgram(pal_brick), gfx_brickTilesLen);
  dmaCopyHalfWords(3, gfx_gradientPal, pal2bgram(pal_gradient), gfx_gradientPalLen);
  
  dmaCopyHalfWords(3, gfx_ballTiles, tile2objram(tiles_ball), gfx_ballTilesLen);
  dmaCopyHalfWords(3, gfx_ballPal, pal2objram(pal_ball), gfx_ballPalLen);

  //Palette[0] is used as a "default" color
  BG_PALETTE[0] = backdrop_colour;

  //in real HW, the memory could be dirty
  for(n = 0; n < 1024; n++){
    bg1map[n] = 0;
  }

  int x,y;
  //bricks
  //iteration in the horizontal
  for(x = 0; x < 32; x++){
    //iteration in the vertical
    for(y = 0; y < 6; y++){
      //if the brick have to be displayed inverted for the bricky effect
      int hflip = (x & 1) ^ (y & 1);
      //see BG map entry for more info on the fields
      bg0map[x + y * 32] = tile_brick | (hflip << 10) | (pal_brick << 12);
    }
  }

  //gradient
  for(x = 0; x < 32; x++){
    for(y = 0; y < 8; y++){
      printf("kasibilds");
      int tile = tile_gradient + y;
      //now, in the background 1
      bg1map[x + y * 32] = tile | (pal_gradient << 12);
    }
  }
  //reg for "special effects" on video. Selecting the effect and the layers to apply it
  REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BACKDROP;
  //configuring the alpha blending
  REG_BLDALPHA = (16 << 8) + (4);

  //Offsets the entire background (in pixels)
  REG_BG0VOFS = 112;

  for(n = 0; n < 128; n++){
    sprites[n].attr0 = ATTR0_DISABLED;
  }

  videoSetMode(
                  MODE_0_2D | 
                  DISPLAY_BG0_ACTIVE | 
                  DISPLAY_BG1_ACTIVE |
                  DISPLAY_SPR_ACTIVE |
                  DISPLAY_SPR_1D_LAYOUT
                  );

  REG_BG0CNT = BG_MAP_BASE(1);
  REG_BG1CNT = BG_MAP_BASE(2);
}

void resetBall(void){
  g_ball.sprite_index = 0;
  g_ball.sprite_affine_index = 0;
  g_ball.x = 128 << 8;
  g_ball.y = 64 << 8;
  g_ball.xvel = 100 << 4;
  g_ball.yvel = 0;
}

void updateLogic(void){
  ballUpdate(&g_ball);
}

void updateGraphics(void){
  ballRender(&g_ball, 0, 0);
}

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
  setupInterruptions();
  setupGraphics();
  resetBall();

  while(1){

    updateLogic();

    swiWaitForVBlank(); /*active only in VBlank*/

    updateGraphics();
  }

  return 0;
}
