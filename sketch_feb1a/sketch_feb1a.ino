//void setup() {
  // put your setup code here, to run once:

//}

//void loop() {
  // put your main code here, to run repeatedly:

//}

#include <Wire.h>

// Sensor and motor definitions
#define COLOR_SENSOR_S2 4
#define COLOR_SENSOR_S3 5
#define COLOR_SENSOR_OUT 6
#define MOTOR_LEFT_FORWARD 9
#define MOTOR_LEFT_BACKWARD 10
#define MOTOR_RIGHT_FORWARD 11
#define MOTOR_RIGHT_BACKWARD 12
#define FLAG_SERVO 3

int red, green, blue;

// Function to read color
void readColor(int &r, int &g, int &b) {
    digitalWrite(COLOR_SENSOR_S2, LOW);
    digitalWrite(COLOR_SENSOR_S3, LOW);
    r = pulseIn(COLOR_SENSOR_OUT, LOW);
  
    digitalWrite(COLOR_SENSOR_S3, HIGH);
    g = pulseIn(COLOR_SENSOR_OUT, LOW);
  
    digitalWrite(COLOR_SENSOR_S2, HIGH);
    b = pulseIn(COLOR_SENSOR_OUT, LOW);
}

// Motor control function
void moveForward() {
    digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
    digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
}

void turnLeft() {
    digitalWrite(MOTOR_LEFT_FORWARD, LOW);
    digitalWrite(MOTOR_RIGHT_FORWARD, HIGH);
    digitalWrite(MOTOR_LEFT_BACKWARD, HIGH);
    digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
}

void turnRight() {
    digitalWrite(MOTOR_LEFT_FORWARD, HIGH);
    digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD, HIGH);
}

void stopRobot() {
    digitalWrite(MOTOR_LEFT_FORWARD, LOW);
    digitalWrite(MOTOR_RIGHT_FORWARD, LOW);
    digitalWrite(MOTOR_LEFT_BACKWARD, LOW);
    digitalWrite(MOTOR_RIGHT_BACKWARD, LOW);
}

void dropFlag() {
    digitalWrite(FLAG_SERVO, HIGH);
    delay(500);
    digitalWrite(FLAG_SERVO, LOW);
}

void setup() {
    Serial.begin(9600);
    pinMode(COLOR_SENSOR_S2, OUTPUT);
    pinMode(COLOR_SENSOR_S3, OUTPUT);
    pinMode(COLOR_SENSOR_OUT, INPUT);
    pinMode(MOTOR_LEFT_FORWARD, OUTPUT);
    pinMode(MOTOR_LEFT_BACKWARD, OUTPUT);
    pinMode(MOTOR_RIGHT_FORWARD, OUTPUT);
    pinMode(MOTOR_RIGHT_BACKWARD, OUTPUT);
    pinMode(FLAG_SERVO, OUTPUT);
}

void loop() {
    readColor(red, green, blue);
    Serial.print("R: "); Serial.print(red);
    Serial.print(" G: "); Serial.print(green);
    Serial.print(" B: "); Serial.println(blue);

    // Example logic: Adjust this based on actual RGB values
    if (red > green && red > blue) {
        turnLeft();  // Red detected, adjust left
    } else if (blue > red && blue > green) {
        turnRight();  // Blue detected, adjust right
    } else {
        moveForward();  // Otherwise, continue moving forward
    }

    // Simulated condition for reaching the center
    if (red < 30 && green < 30 && blue < 30) {  // Assume darker center
        stopRobot();
        dropFlag();
        while (true);  // Stop forever
    }
}


