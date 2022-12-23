#define COIL_1 12
#define SENS_1 A0

double const projectile_length = 60;  // [millimeters]


unsigned long const fire1_time = 300;  // first coil fire delay time [microseconds]
unsigned long fire1_start;
bool fire1_status = false;

int const sensor_min = 999;
int const sensor_max = 1000;

int sensor1_start = 0;


void setup() {
  Serial.begin(115200);           // open the COM port
  Serial.println("starting up");  // start up message

  pinMode(COIL_1, OUTPUT);
  pinMode(SENS_1, INPUT_PULLUP);
}

void loop() {
  Coil1();
  Sensor1();

  delayMicroseconds(10);
}

void Coil1() {
  if (micros() - fire1_start > fire1_time) {
    digitalWrite(COIL_1, LOW);
  } else
    return;
  
  if (Serial.available() > 0) {
    Serial.readString();

    Serial.println("FIRE");
    digitalWrite(COIL_1, HIGH);
    fire1_start = micros();
  }

  // Serial.println(micros() - fire1_start);
}

void Sensor1() {
  unsigned long value = analogRead(SENS_1);
  if ((value < sensor_min) && sensor1_start)  // OUT
  {
    // Serial.println("OUT SENSOR");

    Serial.print("Sensor 1 speed: ");
    Serial.print(projectile_length / (double)(micros() - sensor1_start) * 1000.0f);
    Serial.println(" m/s");
    sensor1_start = 0;

    Serial.print("Flight 1 time: ");
    Serial.print(micros() - fire1_start);
    Serial.println(" us");

    Serial.println();
  } else if ((value > sensor_max) && (!sensor1_start))  // IN
  {
    sensor1_start = micros();
    // Serial.println("IN SENSOR");
  }

  // if(value > 920)
  //   Serial.println(value);
}