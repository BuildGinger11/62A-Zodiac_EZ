#include "main.h"

pros::Motor Intake(4);
pros::Motor Conveyor(15);

void intake(int power)
{
  Intake.move(power);
  Conveyor.move(power);
}
void Intake_Control(void*)
{
  while(true)
  {
    printf("master: %d\n", master.get_digital(DIGITAL_L1));
  if(master.get_digital(DIGITAL_L1))
  {
     intake(127);
  }
  else if(master.get_digital(DIGITAL_DOWN))
  {
    intake(-127);
  }
  else
  {
    intake(0);
  }
  pros::delay(20);
  }
}
