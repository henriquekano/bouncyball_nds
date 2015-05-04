#include <nds.h>
#include "ball.h"

#define ball_radius (8 << 8) //ball radius in 24.8 format
#define c_diam 16 //diameter of the ball (integer)

//Some serious science defines for the physics
#define c_gravity           80                 // gravity constant (add to vertical velocity) (*.8 fixed)
#define c_air_friction      1                  // friction in the air... multiply X velocity by (256-f)/256
#define c_ground_friction   30                 // friction when the ball hits the ground, multiply X by (256-f)/256
#define c_platform_level    ((192-48) << 8)    // the level of the brick platform in *.8 fixed point
#define c_bounce_damper     20                 // the amount of Y velocity that is absorbed when you hit the ground
#define acceleration  10
#define min_height          (1200)             // the minimum height of the ball (when it gets squished) (*.8)
#define min_yvel            (1200)             // the minimum Y velocity (*.8)
#define max_xvel            (1000<<4)          // the maximum X velocity (*.12)

//limits the maximum/minimum values
int clampint(int value, int low, int high){
  if(value < low) value = low;
  if(value > high) value = high;
  return value;
}

//change the attributtes of the ball
void ballUpdate(ball* b){
  b->x += (b->xvel >> 4); //x is 24.8 and xvel is 20.12
  // b->xvel = (b->xvel * (256 + acceleration)) >> 8;
  b->xvel = clampint(b->xvel, -max_xvel, max_xvel);

  b->yvel += c_gravity;
  b->y += (b->yvel);

  //lets stop the ball before it falls off the ground!
  if(b->y + ball_radius >= c_platform_level){
    b->xvel = (b->xvel * (256 - c_ground_friction)) >> 8;
    b->y = c_platform_level - ball_radius;
    b->yvel = -(b->yvel * (256 - c_bounce_damper)) >> 8;
    // if(b->yvel > -min_yvel)
    //   b->yvel = -min_yvel;
  }
}

//render the ball in the screen
void ballRender(ball* b, int camera_x, int camera_y){
  //pointer to sprite OAM entry
  u16* sprite = OAM + b->sprite_index * 4;

  //sprite coordinates
  //camera coordinates is offset from left wall and the floor
  //sprite coordinates is in reference to the sprite center
  int x, y;
  x = ((b->x - ball_radius) >> 8) - camera_x;
  y = ((b->y - ball_radius) >> 8) - camera_y;

  //check if ball is out of the camera baundaries
  if(x <= -16 || x >= 256 || y <= -16 || y >= 192){
    //disable it
    sprite[0] = ATTR0_DISABLED;
    return;
  }

  // ignoring sign
  sprite[0] = y & 255; //-> y position in the attrribute 0
  sprite[1] = (x & 511) | ATTR1_SIZE_16; // -> x position an size in attribute 1
  sprite[2] = 0; //-> other stuff

}