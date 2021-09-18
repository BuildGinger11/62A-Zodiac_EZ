#include "main.h"

const int MOGO_OUT  = 500;
const float MOGO_KP = 0.5;
//const int DELAY_TIME = 10;
// Driver Control Parameters
bool mogo_upB = true;
bool is_at_neutB = false;
int mogo_lockB   = 0;
int controller_mogo_timerB = 0;
int mogo_out_timerB = 0;

bool is_upB = false;
bool is_neutB = false;
bool is_outB = false;


pros::Motor mogoB(10, MOTOR_GEARSET_36, true, MOTOR_ENCODER_DEGREES);
pros::ADIDigitalOut BLock(3);

void set_mogoB(int input)
{
  mogoB = input;
}
void block(bool position)
{
  BLock.set_value(position);
}
void zero_mogoB()
 {
   mogoB.tare_position();
 }
int  get_mogoB()
{
  return mogoB.get_position();
}
int  get_mogo_velB()
{
  return mogoB.get_actual_velocity();
}

void
set_mogo_positionB(int target, int speed)
{
  mogoB.move_absolute(target, speed);
}
bool timeoutB(int target, int vel, int current)
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
mogo_inB (bool hold)
 {
  if (get_mogoB()<150)
  {
    if (get_mogo_velB()==0 || get_mogoB()<0)
     {
      is_upB = true;
      set_mogoB(0);
    }
    else
    {
      set_mogoB(is_upB?0:-60);
    }
  }
  else
  {
    is_upB = false;
    set_mogoB(-127);
  }
  if (hold)
  {
    mogo_upB = true;
    is_at_neutB = false;
    pros::delay(DELAY_TIME);
    mogo_inB(!is_upB);
  }
}

// Mogo Neut

// Mogo Out
void
mogo_outB(bool hold)
 {
  if (get_mogoB() > (MOGO_OUT-100))
  {
    if (get_mogo_velB()==0)
    {
      set_mogo(0);
      is_outB = true;
    }
    else
     {
      mogo_out_timerB+=DELAY_TIME;
      if (mogo_out_timerB<500)
      {
        set_mogoB(20);
        is_outB = false;
      }
      else
      {
        set_mogoB(0);
        is_outB = true;
      }
    }
  }
  else
  {
    set_mogoB(127);
    mogo_out_timerB = 0;
    is_outB = false;
  }
  if (hold) {
    mogo_upB = false;
    is_at_neutB = false;
    pros::delay(DELAY_TIME);
    mogo_outB(!is_outB);
  }
}


///
// Driver Control
//  - when L1 is pressed, toggle between in and out.
//  - when L1 is held and the mogo lift is out, bring the mogo lift to neutral position
///
void
I_Like_Stinky_Butts(void*)
{
  while(true)
  {
  // Toggle for mogo
  if (master.get_digital(DIGITAL_R2) && mogo_lockB==0)
  {
    if (is_at_neutB)
      mogo_upB = false;
    else
      mogo_upB = !mogo_upB;

    is_at_neutB = false;
    mogo_lockB = 1;
  }
  // If mogo is held while the mogo lift is out, bring the mogo lift to neut position
  else if (master.get_digital(DIGITAL_R2))
  {
    if (mogo_upB) {
      controller_mogo_timerB+=DELAY_TIME;
      if (controller_mogo_timerB>=300)
        is_at_neutB = true;
    }
  }
  // Reset when button is let go
  else if (!master.get_digital(DIGITAL_R1))
  {
    mogo_lockB  = 0;
    controller_mogo_timerB = 0;
  }

  // Bring mogo to position based on is_at_neutB and mogo_upB
  if (mogo_upB)
  {
    block(false);
    pros::delay(100);
    mogo_inB(false);
  }
  else if (!mogo_upB)
  {
    mogo_outB(false);
    block(true);
  }
    pros::delay(20);
  }
}
