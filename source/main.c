/*---------------------------------------------------------------------------------

	Basic template code for starting a DS app

---------------------------------------------------------------------------------*/
#include <nds.h>
#include <stdio.h>

#include "gfx_ball.h"
#include "gfx_brick.h"
#include "gfx_gradient.h"

//Tiles
#define tile_empty 0 //tile 0 = empty
#define tile_brick 1 //tile 1 = brick
#define tile_gradient 2 // tile 2 to 9 = gradient

//BG PALETTES
#define pal_brick 0 //brick palette (0~15)
#define pal_gradient 1 //gradient palette (16~31)

#define backdrop_colour RGB8(190, 225, 255) //bluish color for the backdrop
#define pal2bgram(p) (BG_PALETTE + (p) * 16) 

//BG_VRAM base address = BG_GFX
#define tile2bgram(t)(BG_GFX + (t) * 16)

#define bg0map ((u16*)BG_MAP_RAM(1))
#define bg1map ((u16*)BG_MAP_RAM(2))

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

  //Palette[0] is used as a "default" color
  BG_PALETTE[0] = backdrop_colour;

  //painting bricks
  //in real HW, the memory could be dirty
  for(n = 0; n < 1024; n++){
    bg1map[n] = 0;
  }

  int x,y;
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
}
//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------
  irqInit(); /*interrupts init */
  irqEnable(IRQ_VBLANK); /*enable VBlank interrupt*/
  setupGraphics();
  REG_BG0CNT = BG_MAP_BASE(1);
  REG_BG1CNT = BG_MAP_BASE(2);

  while(1){
    swiWaitForVBlank(); /*active only in VBlank*/
    //Active BG0 and on mode 0
    videoSetMode(MODE_0_2D | DISPLAY_BG0_ACTIVE | DISPLAY_BG1_ACTIVE);
  }

  return 0;
}
