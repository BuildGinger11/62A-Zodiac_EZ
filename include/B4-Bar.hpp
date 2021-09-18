#pragma once

extern bool mogo_up;

void set_mogoB(int input);
void zero_mogoB();
int get_mogoB();
int get_mogo_velB();
void set_mogo_positionB(int target, int speed);

void mogo_inB  (bool hold = false);
void mogo_neutB(bool hold = false);
void mogo_outB (bool hold = false);
void I_Like_Stinky_Butts(void*);
