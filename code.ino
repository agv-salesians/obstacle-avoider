#define DISTANCE_SENSOR_PIN A9
#define DISTANCE_BUFFER_SIZE 10

#include <AFMotor.h>

int distanceBuffer[DISTANCE_BUFFER_SIZE] = {0};
int distanceIndex = 0;
float distance = 0;

bool onProcedure = false;
int procedureStep = 0;
long lastStep = 0;

AF_DCMotor motor1(1, MOTOR12_64KHZ);
AF_DCMotor motor2(2, MOTOR12_64KHZ);
AF_DCMotor motor3(3, MOTOR12_64KHZ);
AF_DCMotor motor4(4, MOTOR12_64KHZ);

bool addToDistanceBuffer(int *distanceBuffer, int &distanceIndex, int value) {
  distanceBuffer[distanceIndex] = value;
  distanceIndex++;
  if (distanceIndex > DISTANCE_BUFFER_SIZE - 1) {
    distanceIndex = 0;
    return true;
  } else {
    return false;
  }
}

void calculateDistance() {
  long total = 0;
  for (int i = 0; i < DISTANCE_BUFFER_SIZE; i++) {
    total += distanceBuffer[i];
  }
  float distanceAnalog = 1023 - (float) total / (float) DISTANCE_BUFFER_SIZE;

  if (distanceAnalog < 409) {
    distanceAnalog = 409;
  } else if (distanceAnalog > 941) {
    distanceAnalog = 941;
  }
  distance = (float) (distanceAnalog - 409) / 7.266 + 7;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(DISTANCE_SENSOR_PIN, INPUT);
  Serial.begin(9600);
  motor1.run(BACKWARD);
  motor2.run(BACKWARD);
  motor3.run(BACKWARD);
  motor4.run(BACKWARD);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (addToDistanceBuffer(distanceBuffer, distanceIndex, analogRead(A9))) {
    calculateDistance();
    if (distance < 25 && !onProcedure) {
      onProcedure = true;
      procedureStep = 0;
      lastStep = millis();
    }
  }

  motor1.setSpeed(105);
  motor2.setSpeed(105);
  motor3.setSpeed(105);
  motor4.setSpeed(105);


  if (onProcedure) {
    if (millis() - lastStep > 800) {
      procedureStep++;
      lastStep = millis();
    }
    if (procedureStep == 1) {
      motor1.run(FORWARD);
      motor2.run(FORWARD);
      motor3.run(FORWARD);
      motor4.run(FORWARD);
    } else if (procedureStep == 2) {
      motor1.run(FORWARD);
      motor2.run(BACKWARD);
      motor3.run(FORWARD);
      motor4.run(BACKWARD);
    } else if (procedureStep == 3) {
      onProcedure = false;
      procedureStep = 0;
      motor1.run(BACKWARD);
      motor2.run(BACKWARD);
      motor3.run(BACKWARD);
      motor4.run(BACKWARD);
    }
  }
}
