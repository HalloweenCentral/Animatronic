#include <VarSpeedServo.h>

VarSpeedServo servo;
# define PinLED       8 //This is your defining code. Change the pin numbers based on what you connected things to
# define PinSpeaker   4
# define PinSensor    9
boolean deviceOn = false;
enum states
{
  WAIT_FOR_TRIGGER,
  MOVE_TO_160_DEGREES,
  WAIT1,
  MOVE_TO_60_DEGREES,
  WAIT2,
  WAIT3,
  MOVE_TO_90_DEGREES,
};

const unsigned long wait1Period = 5000;
const unsigned long wait2Period = 5000;
const unsigned long wait3Period = 5000;
const unsigned long movementPeriod = 30000; //Adjust these if needed, The arduino counts in milliseconds, So the waits are 5 seconds each, and the movement time is 30
unsigned long currentTime;
unsigned long shortPeriodStartTime;
unsigned long movementPeriodStartTime;

byte currentState = WAIT_FOR_TRIGGER;
int pos = 90;
void setup()
{
  Serial.begin(115200);
  pinMode(PinSensor, INPUT_PULLUP);
  Serial.println("waiting for trigger");
  servo.attach(6);
  digitalWrite (PinLED,    LOW);
  pinMode(PinSpeaker, OUTPUT);
  pinMode(PinLED, OUTPUT);

}
// -----------------------------------------------------------------------------
struct Timer {
  byte           pin;
  unsigned long  duration;
  bool           on;
};

Timer timers [] = {
  { PinLED, 30000 }, //Change these based on how long you want the LED and sensor to be on for. In most cases it should be the same as movement period. 
  { PinSpeaker,    30000 }
};

#define N_TIMERS    (sizeof(timers)/sizeof(Timer))

// -----------------------------------------------------------------------------
void loop()
{
  static unsigned long msecLst;
  unsigned long msec = millis ();

  // Enable timed outputs
  if ((digitalRead(PinSensor) == LOW) && (deviceOn == false))  {
    deviceOn = true;
    msecLst = msec;
    for (unsigned n = 0; n < N_TIMERS; n++)  {
      timers [n].on = true;
      digitalWrite (timers [n].pin, HIGH);
    }
  }

  // Turn off timed outputs when duration expired
  Timer *t = timers;
  for (unsigned n = 0; n < N_TIMERS; n++, t++)  {
    if (t->on && (msec - msecLst) > t->duration)  {
      digitalWrite (t->pin, LOW);
      t->on = false;

    }
  }


  currentTime = millis();
  switch (currentState)
  {
    case WAIT_FOR_TRIGGER:
      if ((digitalRead(PinSensor) == LOW) && (deviceOn == true   ))
      {
        currentState = MOVE_TO_160_DEGREES;
        movementPeriodStartTime = currentTime;
      }
      break;
    case MOVE_TO_160_DEGREES:
      Serial.println("move to 160 degrees");
      servo.write(160, 10, true); //160 is angle, and 10 is speed. Change those however you wish. 
      shortPeriodStartTime = currentTime;
      currentState = WAIT1;

      break;
    case WAIT1:
      deviceOn = true;
      if (currentTime - shortPeriodStartTime >= wait1Period)
      {
        currentState = MOVE_TO_60_DEGREES;
      }
      checkMovementPeriod();
      break;
    case MOVE_TO_60_DEGREES:
      Serial.println("move to 60 degrees");
      servo.write(60, 10, true);
      shortPeriodStartTime = currentTime;
      currentState = WAIT3;
      break;
    case WAIT3:
      if (currentTime - shortPeriodStartTime >= wait3Period)
      {
        currentState = MOVE_TO_90_DEGREES;
      }
      checkMovementPeriod();
      break;
    case MOVE_TO_90_DEGREES:
      Serial.println("move to 90 degrees");
      servo.write(90, 10, true);
      shortPeriodStartTime = currentTime;
      currentState = WAIT2;
      break;
    case WAIT2:
      deviceOn = true;
      if (currentTime - shortPeriodStartTime >= wait1Period)
      {
        currentState = MOVE_TO_160_DEGREES;
      }
      checkMovementPeriod();

      break;

  };

}


void checkMovementPeriod()
{
  if (currentTime - movementPeriodStartTime >= movementPeriod)
  { deviceOn = false;
    Serial.println("movement  period ended");
    Serial.println("waiting for trigger");
    currentState = WAIT_FOR_TRIGGER;
  }

}
