#include <EasyNeoPixels.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>


#define IR_LEFT A0
#define IR_RIGHT A1
#define IR_MIDDLE A2

#define MOTOR_L1 6
#define MOTOR_L2 5

#define MOTOR_R3 4
#define MOTOR_R4 3

#define MOTOR_L_SPEED 10
#define MOTOR_R_SPEED 9

#define NRF_CE 7
#define NRF_CSN 8

#define NEO_PORT 2
#define NEO_AMOUNT 20

#define trackingSpeed 70

RF24 radio(NRF_CE, NRF_CSN); // CE, CSN

const byte address[6] = "00022"; //address, should match remote

bool lineTracking = false;

int LEFT_JOYSTICK;
int LEFT_CLICK;
int RIGHT_JOYSTICK;
int RIGHT_CLICK;

void setup() {

  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_MIDDLE, INPUT);

  pinMode(MOTOR_L1, OUTPUT);
  pinMode(MOTOR_L2, OUTPUT);
  pinMode(MOTOR_R3, OUTPUT);
  pinMode(MOTOR_R4, OUTPUT);
  pinMode(MOTOR_L_SPEED, OUTPUT);
  pinMode(MOTOR_R_SPEED, OUTPUT);

  setupEasyNeoPixels(NEO_PORT, NEO_AMOUNT);

  Serial.begin(9600);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  driveMotor('L', 0);
  driveMotor('R', 0);

}

void loop() {

  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);

    sscanf(text, "%d,%d,%d", &LEFT_JOYSTICK, &RIGHT_JOYSTICK, &LEFT_CLICK);

    if (LEFT_CLICK) {
      lineTracking = !lineTracking;
      //left stop
      driveMotor('L', 0);
      //right stop
      driveMotor('R', 0);

      setMotorSpeed(70);
      driveMotor('L', 1);
      driveMotor('R', 1);
    }

    if (lineTracking) {

      float reverseMult = 1.0;

      for (int i = 0; i < NEO_AMOUNT; i++) {
        writeEasyNeoPixel(i, LOW);
      }

      bool leftWhite = !digitalRead(IR_LEFT);
      bool middleWhite = !digitalRead(IR_MIDDLE);
      bool rightWhite = !digitalRead(IR_RIGHT);

      bool leftBlack = !leftWhite;
      bool middleBlack = !middleWhite;
      bool rightBlack = !rightWhite;

      if (leftWhite && middleBlack && rightWhite) {
        setMotorSpeed(70);
        driveMotor('L', 1);
        driveMotor('R', 1);
      } else {
        if (middleBlack) {
          if (leftBlack) {
            setMotorSpeed(70 * reverseMult);
            driveMotor('L', 0);
            driveMotor('R', 1);
          }
          if (rightBlack) {
            setMotorSpeed(70 * reverseMult);
            driveMotor('L', 1);
            driveMotor('R', 0);
          }
        } else {
          if (leftBlack) {
            setMotorSpeed(70 * reverseMult);
            driveMotor('L', -1);
            driveMotor('R', 1);
          }
          if (rightBlack) {
            setMotorSpeed(70 * reverseMult);
            driveMotor('L', 1);
            driveMotor('R', -1);
          }
        }
      }



      /*
        if (leftBlack && middleBlack && rightBlack) {

        //do nothing

        //setMotorSpeed(trackingSpeed * reverseMult);
        //driveMotor('L', -1);
        //driveMotor('R', -1);

        } else if (leftBlack && middleBlack && rightWhite) {

        setMotorSpeed(trackingSpeed * reverseMult);
        driveMotor('L', -1);
        driveMotor('R', 1);

        } else if (leftBlack && middleWhite && rightBlack) {

        setMotorSpeed(trackingSpeed);
        driveMotor('L', 1);
        driveMotor('R', 1);

        } else if (leftBlack && middleWhite && rightWhite) {

        setMotorSpeed(trackingSpeed * reverseMult);
        driveMotor('L', -1);
        driveMotor('R', 1);

        } else if (leftWhite && middleBlack && rightBlack) {

        setMotorSpeed(trackingSpeed * reverseMult);
        driveMotor('R', -1);
        driveMotor('L', 1);

        } else if (leftWhite && middleBlack && rightWhite) {

        //do nothing
        //setMotorSpeed(trackingSpeed);
        //driveMotor('L', 1);
        //driveMotor('R', 1);

        } else if (leftWhite && middleWhite && rightBlack) {

        setMotorSpeed(trackingSpeed * reverseMult);
        driveMotor('L', 1);
        driveMotor('R', -1);

        } else if (leftWhite && middleWhite && rightWhite) {

        setMotorSpeed(trackingSpeed);
        driveMotor('L', 1);
        driveMotor('R', 1);

        }
      */

    } else {

      for (int i = 0; i < NEO_AMOUNT; i++) {
        writeEasyNeoPixel(i, random(255), 0, 0);
      }

      setMotorSpeed(255);

      if (LEFT_JOYSTICK > 500 + 100 ) {
        //setMotorSpeed('L', map(LEFT_JOYSTICK, 600, 1023, 25, 255));
        driveMotor('L', -1);
      } else if (LEFT_JOYSTICK < 500 - 100) {
        //setMotorSpeed('L', map(LEFT_JOYSTICK, 400, 0, 25, 255));
        driveMotor('L', 1);
      } else {
        driveMotor('L', 0);
      }

      if (RIGHT_JOYSTICK > 500 + 100 ) {
        //setMotorSpeed('R', map(RIGHT_JOYSTICK, 600, 1023, 25, 255));
        driveMotor('R', -1);
      } else if (RIGHT_JOYSTICK < 500 - 100) {
        //setMotorSpeed('R', map(RIGHT_JOYSTICK, 400, 0, 25, 255));
        driveMotor('R', 1);
      } else {
        driveMotor('R', 0);
      }
    }
  }
}

void setMotorSpeed(char motorID, int velocity) {

  if (motorID == 'L') {
    analogWrite(MOTOR_L_SPEED, velocity);
  } else if (motorID == 'R') {
    analogWrite(MOTOR_R_SPEED, velocity);
  }

}

void setMotorSpeed(int velocity) {

  analogWrite(MOTOR_L_SPEED, velocity);
  analogWrite(MOTOR_R_SPEED, velocity);

}

void driveMotor(char motorID, int dir) {

  if (motorID == 'R') {

    if (dir == -1) {
      digitalWrite(MOTOR_L1, HIGH);
      digitalWrite(MOTOR_L2, LOW);
    } else if (dir == 1) {
      digitalWrite(MOTOR_L1, LOW);
      digitalWrite(MOTOR_L2, HIGH);
    } else if (dir == 0) {
      digitalWrite(MOTOR_L1, HIGH);
      digitalWrite(MOTOR_L2, HIGH);
    }

  } else if (motorID == 'L') {

    if (dir == 1) {
      digitalWrite(MOTOR_R3, HIGH);
      digitalWrite(MOTOR_R4, LOW);
    } else if (dir == -1) {
      digitalWrite(MOTOR_R3, LOW);
      digitalWrite(MOTOR_R4, HIGH);
    } else if (dir == 0) {
      digitalWrite(MOTOR_R3, HIGH);
      digitalWrite(MOTOR_R4, HIGH);
    }
  }
}
