#ifndef BALL_H
#define BALL_H
typedef struct t_ball{
  int x; //24.8 fixed point
  int y; //24.8 fixed point
  int xvel; //20.12 fixed point
  int yvel; //24.8 fixed point
  
  u8 sprite_index; //OAM entry (0~127)
  u8 sprite_affine_index; //OAM affine (0~31)

  int heigh;
} ball;

void ballUpdate(ball* b);
void ballRender(ball* b, int camera_x, int camera_y);
#endif