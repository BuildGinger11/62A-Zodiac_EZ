#include "main.h"

const int MOGO_OUT  = 820;
const float MOGO_KP = 0.5;
//const int DELAY_TIME = 10;
// Driver Control Parameters
bool mogo_up = true;
bool is_at_neut = false;
int mogo_lock   = 0;
int controller_mogo_timer = 0;
int mogo_out_timer = 0;

bool is_up = false;
bool is_neut = false;
bool is_out = false;


pros::Motor mogo(6, MOTOR_GEARSET_36, false, MOTOR_ENCODER_DEGREES);
pros::ADIDigitalOut FLock(8);

void set_mogo(int input)
{
  mogo = input;
}
void flock(bool position)
{
  FLock.set_value(position);
}
void zero_mogo()
 {
   mogo.tare_position();
 }
int  get_mogo()
{
  return mogo.get_position();
}
int  get_mogo_vel()
{
  return mogo.get_actual_velocity();
}

void
set_mogo_position(int target, int speed)
{
  mogo.move_absolute(target, speed);
}
bool timeout(int target, int vel, int current)
{
  static int vel_timeout;
  static int small_timeout;

  if (abs(target-current) < 20)
  {
    small_timeout+=DELAY_TIME;
    if (small_timeout>50)
     {
      small_timeout = 0;
      vel_timeout = 0;
      return true;
    }
  }
  else
  {
    small_timeout = 0;
  }

  if (vel == 0)
  {
    vel_timeout+=DELAY_TIME;
    if (vel_timeout>250)
    {
      vel_timeout = 0;
      small_timeout = 0;
      return true;
    }
  }
  else
  {
    vel_timeout = 0;
  }

  return false;
}

///
// Mogo Control
///

// Mogo In
void
mogo_in (bool hold)
 {
  if (get_mogo()<150)
  {
    if (get_mogo_vel()==0 || get_mogo()<0)
     {
      is_up = true;
      set_mogo(0);
    }
    else
    {
      set_mogo(is_up?0:-60);
    }
  }
  else
  {
    is_up = false;
    set_mogo(-127);
  }
  if (hold)
  {
    mogo_up = true;
    is_at_neut = false;
    pros::delay(DELAY_TIME);
    mogo_in(!is_up);
  }
}

// Mogo Neut

// Mogo Out
void
mogo_out(bool hold)
 {
  if (get_mogo() > (MOGO_OUT-100))
  {
    if (get_mogo_vel()==0)
    {
      set_mogo(0);
      is_out = true;
    }
    else
     {
      mogo_out_timer+=DELAY_TIME;
      if (mogo_out_timer<500)
      {
        set_mogo(20);
        is_out = false;
      }
      else
      {
        set_mogo(0);
        is_out = true;
      }
    }
  }
  else
  {
    set_mogo(127);
    mogo_out_timer = 0;
    is_out = false;
  }
  if (hold) {
    mogo_up = false;
    is_at_neut = false;
    pros::delay(DELAY_TIME);
    mogo_out(!is_out);
  }
}


///
// Driver Control
//  - when L1 is pressed, toggle between in and out.
//  - when L1 is held and the mogo lift is out, bring the mogo lift to neutral position
///
void
mogo_control(void*)
{
  while(true)
  {
  // Toggle for mogo
  if (master.get_digital(DIGITAL_R1) && mogo_lock==0)
  {
    if (is_at_neut)
      mogo_up = false;
    else
      mogo_up = !mogo_up;

    is_at_neut = false;
    mogo_lock = 1;
  }
  // If mogo is held while the mogo lift is out, bring the mogo lift to neut position
  else if (master.get_digital(DIGITAL_R1))
  {
    if (mogo_up) {
      controller_mogo_timer+=DELAY_TIME;
      if (controller_mogo_timer>=300)
        is_at_neut = true;
    }
  }
  // Reset when button is let go
  else if (!master.get_digital(DIGITAL_R1))
  {
    mogo_lock  = 0;
    controller_mogo_timer = 0;
  }

  // Bring mogo to position based on is_at_neut and mogo_up
  if (mogo_up)
  {
    flock(false);
    pros::delay(100);
    mogo_in();
  }
  else if (!mogo_up)
  {
    mogo_out();
    flock(true);
  }
    pros::delay(20);
  }
}
