// Define color sensor pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define OE_PIN 3

// Motor Pins
int EN_A = 11; // Enable pin for first motor
int IN1 = 9;   // Control pin for first motor
int IN2 = 8;   // Control pin for first motor
int IN3 = 7;   // Control pin for second motor
int IN4 = 6;   // Control pin for second motor
int EN_B = 10; // Enable pin for second motor

// Calibration values for the color sensor
int redMin = 0, redMax = 1118;
int greenMin = 0, greenMax = 2200;
int blueMin = 3, blueMax = 2200;

// Variables for color detection
int redPW = 0, greenPW = 0, bluePW = 0;
String currentColor = "";
String previousColor = "";
int ringCounter = 0;
const int totalRings = 6; // Total rings to traverse

void setup() {
    // Setup motor pins
    pinMode(EN_A, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(EN_B, OUTPUT);

    // Setup color sensor pins
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(OE_PIN, OUTPUT);
    digitalWrite(OE_PIN, LOW); // Enable sensor
    pinMode(sensorOut, INPUT);

    // Set frequency scaling to 20%
    digitalWrite(S0, HIGH);
    digitalWrite(S1, LOW);

    // Start serial communication
    Serial.begin(9600);
}

// Function to get red pulse width
int getRedPW() {
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);
    return pulseIn(sensorOut, LOW);
}

// Function to get green pulse width
int getGreenPW() {
    digitalWrite(S2, HIGH);
    digitalWrite(S3, HIGH);
    return pulseIn(sensorOut, LOW);
}

// Function to get blue pulse width
int getBluePW() {
    digitalWrite(S2, LOW);
    digitalWrite(S3, HIGH);
    return pulseIn(sensorOut, LOW);
}

// Detect the dominant color
String detectColor() {
    redPW = map(getRedPW(), redMin, redMax, 255, 0);
    greenPW = map(getGreenPW(), greenMin, greenMax, 255, 0);
    bluePW = map(getBluePW(), blueMin, blueMax, 255, 0);

    if (redPW > greenPW && redPW > bluePW) {
        return "RED";
    } else if (greenPW > redPW && greenPW > bluePW) {
        return "GREEN";
    } else if (bluePW > redPW && bluePW > greenPW) {
        return "BLUE";
    } else {
        return "NONE"; // No valid color detected
    }
}

// Motor control for the right motor
void MotorR(int speed) {
    if (speed == -2) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(EN_B, 255);
    } else if (speed == -1) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(EN_B, 127);
    } else if (speed == 0) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(EN_B, 0);
    } else if (speed == 1) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(EN_B, 127);
    } else if (speed == 2) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(EN_B, 255);
    }
}

// Motor control for the left motor
void MotorL(int speed) {
    if (speed == -2) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(EN_A, 255);
    } else if (speed == -1) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(EN_A, 127);
    } else if (speed == 0) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(EN_A, 0);
    } else if (speed == 1) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(EN_A, 127);
    } else if (speed == 2) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(EN_A, 255);
    }
}

// Move the robot forward
void moveTowardCenter() {
    MotorR(1);
    MotorL(1);
    delay(500); // Move forward slightly
}

// Follow the ring boundary
void followRingBoundary() {
    MotorR(1);
    MotorL(-1); // Slight turn
}

// Perform a 180-degree turn
void turn180() {
    MotorR(-2);
    MotorL(2);
    delay(1500); // Adjust duration for a complete 180° turn
    stopRobot();
}

// Stop the robot
void stopRobot() {
    MotorR(0);
    MotorL(0);
}

// Simulate placing the flag
void placeFlag() {
    Serial.println("Flag placed!");
}

void loop() {
    // Perform a 180-degree turn if starting
    if (ringCounter == 0) {
        turn180();
        ringCounter++; // Increment to mark the first ring
        previousColor = detectColor();
    }

    // Detect the current color
    currentColor = detectColor();

    // Check for a ring transition
    if (currentColor != previousColor && currentColor != "NONE") {
        ringCounter++;
        Serial.print("Ring Transition Detected: ");
        Serial.println(ringCounter);

        // Move inward toward the next ring
        moveTowardCenter();

        // Update previous color
        previousColor = currentColor;
    }

    // Stop and place the flag at the center
    if (ringCounter >= totalRings) {
        stopRobot();
        placeFlag();
        while (true); // Halt execution
    } else {
        // Follow the ring boundary
        followRingBoundary();
    }
}
