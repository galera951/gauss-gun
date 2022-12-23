#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <OneButton.h>

#include "GaussGun.hpp"


#define COIL_1_PIN 12
#define SENS_1_PIN A0
#define BTN_PIN 11



// Constants 

double const projectile_length = 49.0f;  // Length of projectile [millimeters]
unsigned long const timeout = 3e6; // Timeout between shoots [microseconds]

unsigned long const fire1_time = 3000;  // first coil fire delay time [microseconds]



// Variables

LiquidCrystal_I2C lcd(0x3F,16,2);
OneButton button(BTN_PIN, false);

Coil coil1(COIL_1_PIN);
Sensor sens1(SENS_1_PIN);

unsigned long start_time = 0;

bool exp_trig = false;
bool coil1_trig = false;



// Setup program

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("GaussGun v0.1");

  button.attachClick(startExperiment);

  delay(1000);
  lcd.clear();
  lcd.print("Ready to fire");
}



// Main loop

void loop() {
  button.tick();
  sens1.upd();
  
  coilTick();

  delayMicroseconds(10);
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
  }
}

void endExperiment()
{
  printInfo();
//  lcd.clear();
//  lcd.print("Ready to fire");
  exp_trig = false;
}

void coilTick()
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

void printInfo()
{
  double t1 = (double)(sens1.getEndTime() - sens1.getStartTime()) / 1000.0f;
  double v1 = projectile_length / t1;
  
  lcd.setCursor(0, 0);
  lcd.print("V1= ");
  lcd.setCursor(4, 0);
  lcd.print(v1);
  lcd.setCursor(12, 0);
  lcd.print(" m/s");

  lcd.setCursor(0, 1);
  lcd.print("t1= ");
  lcd.setCursor(4, 1);
  lcd.print(t1);
  lcd.setCursor(12, 1);
  lcd.print(" ms");
}
