#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneButton.h>

#include "GaussGun.hpp"


#define COIL_1_PIN 12
#define SENS_1_PIN A2
#define VOLT_1_PIN A1
#define BTN_PIN 11


enum class Menu
{
  ready_to_fire,
  after_fire
};

enum class InfoScreen
{
  first,
  second,
  third
};



// Constants

unsigned long const screen_update_timeout = 0.5e6;

double const projectile_length = 49.0f;  // Length of projectile [millimeters]
unsigned long const timeout = 3e6; // Timeout between shoots [microseconds]

unsigned long const fire1_time = 200;  // first coil fire delay time [microseconds]



// Variables

Menu menu_var;
InfoScreen info_screen_var;

unsigned long screen_time = 0;

LiquidCrystal_I2C lcd(0x3F, 16, 2);
OneButton button(BTN_PIN, false);

Coil coil1(COIL_1_PIN);
Sensor sens1(SENS_1_PIN);
Voltmeter voltm1(VOLT_1_PIN);

unsigned long start_time = 0;

bool exp_trig = false;
bool coil1_trig = false;
bool reset_trig = false;

double t1;
double t2;
double t3;

double u1;
double u2;
double u3;



// Setup program

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("GaussGun v0.2");

  button.attachClick(handleClick);
  button.attachDoubleClick(handleDoubleClick);
  button.attachLongPressStart(handleLongPressStart);

  delay(1000);

  preparingToExperiment();
}



// Main loop

void loop() {
  button.tick();
  sens1.tick();
  voltm1.tick();

  screenTick();
  coilTick();

  delayMicroseconds(10);
}

void handleClick()
{
  switch (menu_var)
  {
    case Menu::ready_to_fire:
      startExperiment();
      break;

    case Menu::after_fire:
      changeInfo();
      break;
  }
}

void handleDoubleClick()
{

}

void handleLongPressStart()
{
  switch (menu_var)
  {
    case Menu::after_fire:
      preparingToExperiment();
      break;
  }
}

void screenTick()
{
  if (micros() - screen_time > screen_update_timeout)
  {
    switch (menu_var)
    {
      case Menu::ready_to_fire:
        lcd.setCursor(4, 0);
        lcd.print("        ");
        lcd.setCursor(4, 0);
        lcd.print(voltm1.getVoltage());
        break;
    }

    screen_time = micros();
  }
}

void preparingToExperiment()
{
  menu_var = Menu::ready_to_fire;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("V1= ");
  lcd.setCursor(12, 0);
  lcd.print(" V");

  lcd.setCursor(0, 1);
  lcd.print("Click to fire");
}

void startExperiment()
{
  if (micros() - start_time > timeout)
  {
    lcd.clear();
    lcd.setCursor(6, 0);
    lcd.print("Fire!");

    coil1_trig = true;
    exp_trig = true;
    start_time = micros();

    sens1.startMeasure();
  }
}

void coilTick()
{
  if (exp_trig)
  {
    if (coil1_trig)
    {
      coil1.setOn();
      coil1_trig = false;
    }
    else if (micros() - coil1.getStartTime() > fire1_time)
    {
      coil1.setOff();
    }

    if ((micros() - start_time > 1e6) && exp_trig)
    {
      endExperiment();
    }
  }


  if (reset_trig)
  {
    if (coil1_trig)
    {
      coil1.setOn(); 
      coil1_trig = false;
    } else if (micros() - coil1.getStartTime() > 1e6)
    {
      coil1.setOff();
      reset_trig = false;
    }
  }
}

void endExperiment()
{
  sens1.endMeasure();
  
  t1 = (double)sens1.getImpulseTime() / 1000.0f;
  u1 = voltm1.getVoltage();

  menu_var = Menu::after_fire;
  info_screen_var = InfoScreen::first;

  changeInfo();
  resetSetup();
}

void resetSetup()
{
  exp_trig = false;
  reset_trig = true;
  coil1_trig = true;

  coil1.clean();
  sens1.clean();
}

void changeInfo()
{
  lcd.clear();

  switch (info_screen_var)
  {
    case InfoScreen::first:
      lcd.setCursor(0, 0);
      lcd.print("t1= ");
      lcd.setCursor(4, 0);
      lcd.print(t1);
      lcd.setCursor(12, 0);
      lcd.print(" ms");

      lcd.setCursor(0, 1);
      lcd.print("u1= ");
      lcd.setCursor(4, 1);
      lcd.print(u1);
      lcd.setCursor(12, 1);
      lcd.print(" V");

      info_screen_var = InfoScreen::second;
      break;

    case InfoScreen::second:
      lcd.setCursor(0, 0);
      lcd.print("t2= ");
      lcd.setCursor(4, 0);
      lcd.print(t2);
      lcd.setCursor(12, 0);
      lcd.print(" ms");

      lcd.setCursor(0, 1);
      lcd.print("u2= ");
      lcd.setCursor(4, 1);
      lcd.print(u2);
      lcd.setCursor(12, 1);
      lcd.print(" V");

      info_screen_var = InfoScreen::third;
      break;

    case InfoScreen::third:
      lcd.setCursor(0, 0);
      lcd.print("t3= ");
      lcd.setCursor(4, 0);
      lcd.print(t3);
      lcd.setCursor(12, 0);
      lcd.print(" ms");

      lcd.setCursor(0, 1);
      lcd.print("u3= ");
      lcd.setCursor(4, 1);
      lcd.print(u3);
      lcd.setCursor(12, 1);
      lcd.print(" V");

      info_screen_var = InfoScreen::first;
      break;
  }
}
