#include <SoftwareSerial.h>

// Constants
const double MIN_DIST = 0.5;
const double MAX_VEL = 1.0;

// Ping sensors pins
const int triggerPinLeft = 11;
const int echoPinLeft = 12;
const int triggerPinRight = 13;
const int echoPinRight = 14;

// Robot state
enum RobotState {
  Idling,
  Automatic,
  Manual
};
RobotState state = Idling;

// Wheel speed/direction
int leftWheelDir1 = LOW;
int leftWheelDir2 = LOW;
double leftWheelSpeed = 0;

int rightWheelDir1 = LOW;
int rightWheelDir2 = LOW;
double rightWheelSpeed = 0.0;

// Bluetooth
SoftwareSerial bluetooth(10, 11);

// Wheel pins
const int dir1PinA = 2;
const int dir2PinA = 3;
const int speedPinA = 9;

const int dir1PinB = 4;
const int dir2PinB = 5;
const int speedPinB = 10;

void setup() {
  // Define output for direction pins
  pinMode(dir1PinA, OUTPUT);
  pinMode(dir2PinA, OUTPUT);
  pinMode(speedPinA, OUTPUT);
  
  pinMode(dir1PinB, OUTPUT);
  pinMode(dir2PinB, OUTPUT);
  pinMode(speedPinB, OUTPUT);
  
  // Define I/O for left and right ping sensors
  pinMode(triggerPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(triggerPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);
  
  // Set the data rate for SoftwareSerial port
  bluetooth.begin(9600);

  // Print introduction to bluetooth device
  bluetooth.println("Robot Car Started.");
}


void loop() {
  readInput();

  // Update robot car's position based on its current state
  switch (state) {
    case Idling:
      break;
    case Automatic:
      CheckObstacles();
      MoveRobot();
      break;
    case Manual:
      MoveRobot();
      break;
    default:
      break;
  }
}


void readInput() {
  if (bluetooth.available()) {
    char cmd = bluetooth.read();
    
    switch (cmd) {
      case '0':
        state = RobotState::Idling;
        bluetooth.println("Robot car is idling.");
        break;
      case '1':
        state = RobotState::Automatic;
        bluetooth.println("Robot car is automatically moving");
        break;
      case '2':
        state = RobotState::Manual;
        break;
      case 'w':
        if (state = RobotState::Manual) {
          MoveForward();
        }
        else {
          bluetooth.println("Error: Not in Manual mode!");
        }
        break;
      case 's':
        if (state = RobotState::Manual) {
          MoveBackward();
        }
        else {
          bluetooth.println("Error: Not in Manual mode!");
        }
        break;
      case 'a':
        if (state = RobotState::Manual) {
          MoveLeft();
        }
        else {
          bluetooth.println("Error: Not in Manual mode!");
        }
        break;
      case 'd':
        if (state = RobotState::Manual) {
          MoveRight();
        }
        else {
          bluetooth.println("Error: Not in Manual mode!");
        }
        break;
      default:
        bluetooth.println("Unknown command. Options are:\n'0' Idle\n'1' Move Automatically\n'2' Move Manually");
        break;
    }
  }
}

void CheckObstacles() {
  long distance_left = GetDistanceOfPingSensor(triggerPinLeft, echoPinLeft);
  long distance_right = GetDistanceOfPingSensor(triggerPinRight, echoPinRight);
  
  if (distance_left <= MIN_DIST || distance_right <= MIN_DIST) {
    if (distance_left < distance_right) {
      MoveRight();
    }
    else {
      MoveLeft();
    }
  } else {
    MoveForward();
  }
}

long GetDistanceOfPingSensor(int trigger, int echo)
{
  // Read leftt pin
  // Send a trigger
  digitalWrite(trigger, LOW);
  delayMicroseconds(5);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);

  // Read in the duration
  pinMode(echoPinLeft, INPUT);
  long duration = pulseIn(echo, HIGH);

  // Convert time into distance
  return ((duration / 2.0) / (29.1)) / (100);
}

void MoveForward() {
  leftWheelDir1 = LOW;
  leftWheelDir2 = HIGH;
  leftWheelSpeed = MAX_VEL;

  rightWheelDir1 = LOW;
  rightWheelDir2 = HIGH;
  rightWheelSpeed = MAX_VEL;
}

void MoveRight() {
  leftWheelDir1 = LOW;
  leftWheelDir2 = HIGH;
  leftWheelSpeed = MAX_VEL;

  rightWheelDir1 = LOW;
  rightWheelDir2 = LOW;
  rightWheelSpeed = 0;
}

void MoveLeft() {
  leftWheelDir1 = LOW;
  leftWheelDir2 = LOW;
  leftWheelSpeed = 0;

  rightWheelDir1 = LOW;
  rightWheelDir2 = HIGH;
  rightWheelSpeed = MAX_VEL;
}

void MoveBackward() {
  leftWheelDir1 = HIGH;
  leftWheelDir2 = LOW;
  leftWheelSpeed = MAX_VEL;

  rightWheelDir1 = HIGH;
  rightWheelDir2 = LOW;
  rightWheelSpeed = MAX_VEL;
}

void MoveRobot() {
  // Send movement to robot's wheels
  digitalWrite(dir1PinA, leftWheelDir1);
  digitalWrite(dir2PinA, leftWheelDir2);
  analogWrite(speedPinA, leftWheelSpeed);

  digitalWrite(dir1PinB, rightWheelDir1);
  digitalWrite(dir2PinB, rightWheelDir2);
  analogWrite(speedPinB, rightWheelSpeed);
}







