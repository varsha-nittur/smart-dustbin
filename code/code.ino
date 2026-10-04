// Smart Dustbin: opens the lid when something is in front of it.
// Board: NodeMCU ESP8266 | Sensor: HC-SR04 ultrasonic | Actuator: SG90 servo
//
// Wiring
//   HC-SR04  Vcc -> 3V3   Gnd -> GND   Trig -> D3   Echo -> D2
//   SG90     Red -> Vin   Brown -> GND   Orange (signal) -> D5

#include <Servo.h>

#define TRIG_PIN  D3
#define ECHO_PIN  D2
#define SERVO_PIN D5

const int OPEN_DISTANCE_CM = 25;      // open the lid when an object is closer than this
const int CLOSED_ANGLE     = 0;       // servo angle when the lid is closed
const int OPEN_ANGLE       = 90;      // servo angle when the lid is open
const unsigned long HOLD_MS = 3000;   // keep the lid open this long after the object leaves

Servo lidServo;
bool lidOpen = false;
unsigned long lastSeenMs = 0;

// Measure distance in centimetres
long readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);                    // send a 10 microsecond pulse
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);  // wait up to 30 ms for the echo
  if (duration == 0) return 999;                   // no echo = nothing in range
  return duration / 58;                            // microseconds to centimetres
}

void setup() {
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  lidServo.attach(SERVO_PIN);
  lidServo.write(CLOSED_ANGLE);                    // start with the lid closed
}

void loop() {
  if (readDistanceCm() < OPEN_DISTANCE_CM) {       // something is in front of the bin
    lastSeenMs = millis();
    if (!lidOpen) {
      lidServo.write(OPEN_ANGLE);
      lidOpen = true;
    }
  } else if (lidOpen && millis() - lastSeenMs > HOLD_MS) {   // nothing there for 3 seconds
    lidServo.write(CLOSED_ANGLE);
    lidOpen = false;
  }
  delay(100);
}
