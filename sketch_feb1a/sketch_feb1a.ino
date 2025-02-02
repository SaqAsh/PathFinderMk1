// ===========================
// Hardware Pin Definitions
// ===========================

// --- Color Sensor Pins ---
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define OE_PIN 3

// --- Sonar Pins ---
const int trigPin = 9;
const int echoPin = 10;

// --- Motor Pins (Reassigned for Arduino R4 Uno) ---
// Left Motor
int EN_A = 11;  // Left Motor Enable
int IN1  = 12;  // Left Motor Control 1
int IN2  = 13;  // Left Motor Control 2
// Right Motor
int EN_B = A2;  // Right Motor Enable (using an analog pin as digital)
int IN3  = A0;  // Right Motor Control 1
int IN4  = A1;  // Right Motor Control 2

// --- LED Pin ---
// We'll use the built-in LED to signal a correct tile detection.
#define LED_PIN LED_BUILTIN

// ===========================
// Color Sensor Calibration Values
// ===========================
int redMin   = 0, redMax   = 1118;
int greenMin = 0, greenMax = 2200;
int blueMin  = 3, blueMax  = 2200;

// Variables for Color Pulse Width Measurements
int redPW   = 0, greenPW = 0, bluePW  = 0;
// Variables for Final Color Values
int redRaw, greenRaw, blueRaw;

// ===========================
// Sequence and Navigation Data
// ===========================
// The expected color sequence: Red, Green, Blue, Green, Blue
char sequence[] = {'R', 'G', 'B', 'G', 'B'};
int sequenceIndex = 0;

// Global robot position and orientation (for a grid)
// Start at (0,0), facing North.
int currentX = 0;
int currentY = 0;
// Orientation: 0 = North, 1 = East, 2 = South, 3 = West
int currentOrientation = 0;

// Data structure for tracking visited tiles (i.e. tiles that have already contributed to the sequence)
struct Tile { 
  int x; 
  int y; 
  char color; // store the detected color for reference
};
Tile visited[100];
int visitedCount = 0;

// Data structure for backtracking stack (stores tile coordinates)
struct StackTile { 
  int x; 
  int y; 
};
StackTile backtrackStack[100];
int stackTop = -1;

void pushToStack(int x, int y) {
  stackTop++;
  backtrackStack[stackTop].x = x;
  backtrackStack[stackTop].y = y;
}

StackTile popFromStack() {
  StackTile tile = backtrackStack[stackTop];
  stackTop--;
  return tile;
}

bool isStackEmpty() { 
  return stackTop == -1; 
}

// Check whether the current tile (position) has already been used for a correct color detection.
// (This prevents using the same physical tile twice for repeated colors.)
bool isTileVisited(int x, int y) {
  for (int i = 0; i < visitedCount; i++) {
    if (visited[i].x == x && visited[i].y == y)
      return true;
  }
  return false;
}

void markTileVisited(int x, int y, char color) {
  visited[visitedCount].x = x;
  visited[visitedCount].y = y;
  visited[visitedCount].color = color;
  visitedCount++;
}

// ===========================
// Sensor Functions
// ===========================

void sendUltrasonicPulse() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
}

int getDistance() {
  sendUltrasonicPulse();
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

// ===========================
// Motor Control Functions
// ===========================
void Motor_R(int speed) { // Right Motor (second motor)
  if (speed == -1) { // Move backward
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, 150);
  }
  if (speed == 0) { // Stop
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(EN_B, 0);
  }
  if (speed == 1) { // Move forward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(EN_B, 150);
  }
}

void Motor_L(int speed) { // Left Motor (first motor)
  if (speed == -1) { // Move backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, 150);
  }
  if (speed == 0) { // Stop
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(EN_A, 0);
  }
  if (speed == 1) { // Move forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(EN_A, 150);
  }
}

// Basic movement commands (assume one unit = one tile)
void forward(float unit) {
  Motor_R(1);
  Motor_L(1);
  delay(200 * unit);
  Motor_R(0);
  Motor_L(0);
  delay(200 * unit);
}

void backward(float unit) {
  Motor_R(-1);
  Motor_L(-1);
  delay(200 * unit);
  Motor_R(0);
  Motor_L(0);
  delay(200 * unit);
}

void rightTurn(float unit) {
  Motor_R(-1);
  Motor_L(1);
  delay(200 * unit);
  Motor_R(0);
  Motor_L(0);
  delay(200 * unit);
}

void leftTurn(float unit) {
  Motor_R(1);
  Motor_L(-1);
  delay(200 * unit);
  Motor_R(0);
  Motor_L(0);
  delay(200 * unit);
}

// ===========================
// Navigation Helper Functions
// ===========================

// Turn the robot to face targetOrientation (0 = North, 1 = East, 2 = South, 3 = West)
void turnTo(int targetOrientation) {
  int diff = targetOrientation - currentOrientation;
  if(diff < 0)
    diff += 4;
  if(diff == 0) {
    // Already facing the target
    return;
  } else if(diff == 1) {
    rightTurn(1);
    currentOrientation = (currentOrientation + 1) % 4;
  } else if(diff == 2) {
    // Two right turns
    rightTurn(1);
    rightTurn(1);
    currentOrientation = (currentOrientation + 2) % 4;
  } else if(diff == 3) {
    leftTurn(1);
    currentOrientation = (currentOrientation + 3) % 4;
  }
  Serial.print("New Orientation: ");
  Serial.println(currentOrientation);
}

// Move one tile forward and update the current position based on orientation.
void moveOneStep() {
  forward(1);
  if(currentOrientation == 0)        // Facing North
    currentY++;
  else if(currentOrientation == 1)   // Facing East
    currentX++;
  else if(currentOrientation == 2)   // Facing South
    currentY--;
  else if(currentOrientation == 3)   // Facing West
    currentX--;
  
  Serial.print("New Position: (");
  Serial.print(currentX);
  Serial.print(", ");
  Serial.print(currentY);
  Serial.println(")");
}

// ===========================
// Backtracking Function
// ===========================
// Moves the robot from its current position back along the stored path.
void backtrack() {
  Serial.println("Initiating backtracking...");
  while(stackTop >= 0) {
    // Peek at the top of the stack (target tile)
    StackTile target = backtrackStack[stackTop];
    
    // If already at the target tile, pop it and continue.
    if(currentX == target.x && currentY == target.y) {
      popFromStack();
      continue;
    }
    
    // Compute the difference (assumes adjacent tiles differ by 1)
    int dx = target.x - currentX;
    int dy = target.y - currentY;
    int targetOrientation = currentOrientation;
    
    if(dx == 1)         targetOrientation = 1; // East
    else if(dx == -1)   targetOrientation = 3; // West
    else if(dy == 1)    targetOrientation = 0; // North
    else if(dy == -1)   targetOrientation = 2; // South
    else {
      Serial.println("Error: non-adjacent tile encountered during backtracking.");
      break;
    }
    
    turnTo(targetOrientation);
    moveOneStep();
    delay(500); // Allow time for motion to settle
  }
  Serial.println("Backtracking complete.");
}

// ===========================
// Color Sensor Reading Functions
// ===========================
int getRedPW() {
  // Set sensor to read Red only
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  return pulseIn(sensorOut, LOW);
}

int getGreenPW() {
  // Set sensor to read Green only
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  return pulseIn(sensorOut, LOW);
}

int getBluePW() {
  // Set sensor to read Blue only
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  return pulseIn(sensorOut, LOW);
}

// ===========================
// LED Blinking Function
// ===========================
void blinkLED() {
  digitalWrite(LED_PIN, HIGH);
  delay(100);
  digitalWrite(LED_PIN, LOW);
  delay(100);
}

// ===========================
// Setup and Main Loop
// ===========================
void setup() {
  // --- Color Sensor Setup ---
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);
  pinMode(sensorOut, INPUT);

  // --- Sonar Setup ---
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // --- Motor Setup ---
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // --- LED Setup ---
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(9600);
  Serial.println("Setup Complete (Arduino R4 Uno with remapped pins)");
  Serial.println("Starting at (0,0), facing North.");
}

void loop() {
  // --- Color Detection Logic ---
  // Read color sensor pulse widths and map to a 0-255 range.
  redPW = getRedPW();
  redRaw = map(redPW, redMin, redMax, 255, 0);
  delay(200);

  greenPW = getGreenPW();
  greenRaw = map(greenPW, greenMin, greenMax, 255, 0);
  delay(200);

  bluePW = getBluePW();
  blueRaw = map(bluePW, blueMin, blueMax, 255, 0);
  delay(200);

  char detectedColor = 'N';
  if (redRaw > greenRaw && redRaw > blueRaw) {
    detectedColor = 'R';
    Serial.println("Red detected");
  } else if (greenRaw > redRaw && greenRaw > blueRaw) {
    detectedColor = 'G';
    Serial.println("Green detected");
  } else if (blueRaw > redRaw && blueRaw > greenRaw) {
    detectedColor = 'B';
    Serial.println("Blue detected");
  } else {
    Serial.println("No dominant color detected");
  }

  int distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // --- Navigation and Sequence Logic ---
  // If no obstacle is near, process the current tile.
  if (distance > 10) {
    // Only accept this tile if its color matches the expected sequence
    // and if we have not already used this tile.
    if (detectedColor == sequence[sequenceIndex] && !isTileVisited(currentX, currentY)) {
      // Blink LED to signal correct detection.
      blinkLED();
      // Mark this tile as visited (with its detected color).
      markTileVisited(currentX, currentY, detectedColor);
      // Also push its coordinate onto the backtracking stack.
      pushToStack(currentX, currentY);
      sequenceIndex++;
      Serial.println("Color matched! Moving to next in sequence.");
    } else if (isTileVisited(currentX, currentY)) {
      Serial.println("Duplicate tile detected, skipping.");
    }
    // Move forward one tile.
    moveOneStep();
  } else {
    // If an obstacle is detected, initiate backtracking.
    Serial.println("Obstacle detected! Initiating backtracking.");
    backtrack();
  }

  // If the sequence is complete, halt further execution.
  if (sequenceIndex >= sizeof(sequence) / sizeof(sequence[0])) {
    Serial.println("Sequence complete!");
    while (true);  // Halt
  }
}
