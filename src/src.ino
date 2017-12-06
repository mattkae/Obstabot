#include <SoftwareSerial.h>

// Constants
const double MIN_DIST = 50;
const double MAX_VEL = 200;

// Ping sensors pins
const int triggerPinLeft = 8;
const int echoPinLeft = 9;
const int triggerPinRight = 6;
const int echoPinRight = 7;

// Wheel pins
const int dir1PinA = 4;
const int dir2PinA = 5;
const int speedPinA = 10;

const int dir1PinB = 2;
const int dir2PinB = 3;
const int speedPinB = 11;

// Bluetooth pins
const int rxPin = 13;
const int txPin = 12;

// Bluetooth
SoftwareSerial bluetooth(rxPin, txPin);

// Robot state
enum RobotState {
  Idling,
  Automatic,
  Manual
};
RobotState state = Idling;

// Robot direction
enum Direction {
  None,
  Forward,
  Right,
  Backward,
  Left
};
Direction dir = Direction::None;

// Wheel speed/direction
int leftWheelDir1 = LOW;
int leftWheelDir2 = LOW;
double leftWheelSpeed = MAX_VEL;

int rightWheelDir1 = LOW;
int rightWheelDir2 = LOW;
double rightWheelSpeed = MAX_VEL;

void setup() {
  // Define output for direction pins
  pinMode(dir1PinA, OUTPUT);
  pinMode(dir2PinA, OUTPUT);
  pinMode(speedPinA, OUTPUT);
  analogWrite(speedPinA, leftWheelSpeed);

  pinMode(dir1PinB, OUTPUT);
  pinMode(dir2PinB, OUTPUT);
  pinMode(speedPinB, OUTPUT);
  analogWrite(speedPinB, rightWheelSpeed);
  
  // Define I/O for left and right ping sensors
  pinMode(triggerPinLeft, OUTPUT);
  pinMode(echoPinLeft, INPUT);
  pinMode(triggerPinRight, OUTPUT);
  pinMode(echoPinRight, INPUT);

  // Open bluetooth pins
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  
  // Set the data rate for SoftwareSerial port
  bluetooth.begin(9600);

  // Print introduction to bluetooth device
  bluetooth.println("Robot Car Started.");
}

int var = 1;
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
  if (bluetooth.available() > 0) {
    char cmd = bluetooth.read();

    switch (cmd) {
      case '0':
        state = RobotState::Idling;
        Stop();
        MoveRobot();
        bluetooth.println("Robot car is idling.");
        break;
      case '1':
        state = RobotState::Automatic;
        bluetooth.println("Robot car is automatically moving");
        break;
      case '2':
        state = RobotState::Manual;
        bluetooth.println("Robot car is manually moving");
        break;
      case 'w':
        if (state == RobotState::Manual) {
          MoveForward();
        }
        else {
          bluetooth.println("Error: Not in Manual mode!");
        }
        break;
      case 's':
        if (state == RobotState::Manual) {
          MoveBackward();
        }
        else {
          bluetooth.println("Error: Not in Manual mode!");
        }
        break;
      case 'a':
        if (state == RobotState::Manual) {
          MoveLeft();
        }
        else {
          bluetooth.println("Error: Not in Manual mode!");
        }
        break;
      case 'd':
        if (state == RobotState::Manual) {
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
  
  if (distance_left <= MIN_DIST && distance_left >= 10) {
    if (distance_right <= MIN_DIST && distance_right >= 10) {
      MoveLeft();
    } else {
      MoveRight();
    }
  } else if (distance_right <= MIN_DIST && distance_right >= 10) {
    MoveLeft();
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
  return (duration / 2.0) / 29.1;
}

void MoveForward() {
  if (dir == Direction::Forward) {
    return;
  }
  
  bluetooth.println("Forward");
  leftWheelDir1 = LOW;
  leftWheelDir2 = HIGH;

  rightWheelDir1 = LOW;
  rightWheelDir2 = HIGH;

  MoveRobot();

  dir = Direction::Forward;
}

void MoveRight() {
  if (dir == Direction::Right) {
    return;
  }
  
  bluetooth.println("Right");
  leftWheelDir1 = LOW;
  leftWheelDir2 = HIGH;

  rightWheelDir1 = LOW;
  rightWheelDir2 = LOW;

  MoveRobot();

  dir = Direction::Right;

  delay(1000);
  MoveForward();
}

void MoveLeft() {
  if (dir == Direction::Left) {
    return;
  }
  
  bluetooth.println("Left");
  leftWheelDir1 = LOW;
  leftWheelDir2 = LOW;

  rightWheelDir1 = LOW;
  rightWheelDir2 = HIGH;

  MoveRobot();

  dir = Direction::Left;

  delay(1000);
  MoveForward();
}

void MoveBackward() {
  if (dir == Direction::Backward) {
    return;
  }
  
  leftWheelDir1 = HIGH;
  leftWheelDir2 = LOW;

  rightWheelDir1 = HIGH;
  rightWheelDir2 = LOW;

  MoveRobot();

  dir = Direction::Backward;
}

void Stop() {
  leftWheelDir1 = LOW;
  leftWheelDir2 = LOW;

  rightWheelDir1 = LOW;
  rightWheelDir2 = LOW;

  dir = Direction::None;
}

void MoveRobot() {
  delay(100);
  
  // Send movement to robot's wheels
  digitalWrite(dir1PinA, leftWheelDir1);
  digitalWrite(dir2PinA, leftWheelDir2);

  delay(100);

  digitalWrite(dir1PinB, rightWheelDir1);
  digitalWrite(dir2PinB, rightWheelDir2);
}







