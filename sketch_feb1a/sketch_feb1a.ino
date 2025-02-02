// ===========================
// Calibration Constants (adjust as needed)
// ===========================
#define TILE_DURATION 2000    // Time in ms to traverse one tile
#define TURN_DURATION 1000     // Time in ms for a 90° turn

// ===========================
// Hardware Pin Definitions
// ===========================

// --- Color Sensor Pins ---
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 2
#define OE_PIN 3

// --- Sonar Pins ---
const int trigPin = 8;
const int echoPin = 9;

// --- Motor Pins for L298N ---
// Left Motor
int EN_A = A0;  // Left Motor Enable
int IN1  = A1;  // Left Motor Control 1
int IN2  = 10;  // Left Motor Control 2
// Right Motor
int IN3  = 11;  // Right Motor Control 1
int IN4  = 12;  // Right Motor Control 2
int EN_B = 13;  // Right Motor Enable

// --- LED Pin ---
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

// Global robot position and orientation (grid coordinates)
// Starting at (0,0), facing North.
int currentX = 0;
int currentY = 0;
int currentOrientation = 0;  // 0 = North, 1 = East, 2 = South, 3 = West

// -------------------------------
// Visited Tile Data Structure
// -------------------------------
struct Tile { 
  int x; 
  int y; 
  char color;  // the detected color (for duplicate checking)
};
Tile visited[100];
int visitedCount = 0;

// -------------------------------
// Backtracking Stack Data Structure
// -------------------------------
// Each decision point stores its grid coordinates plus a bitmask of attempted directions.
// Bit mapping: bit0 = North, bit1 = East, bit2 = South, bit3 = West.
struct StackTile { 
  int x; 
  int y; 
  int attempted;
};
StackTile backtrackStack[100];
int stackTop = -1;

void pushToStack(int x, int y) {
  stackTop++;
  backtrackStack[stackTop].x = x;
  backtrackStack[stackTop].y = y;
  backtrackStack[stackTop].attempted = 0; // no directions attempted yet
}

StackTile popFromStack() {
  StackTile tile = backtrackStack[stackTop];
  stackTop--;
  return tile;
}

bool isStackEmpty() { 
  return stackTop == -1; 
}

// Check if the current tile (x, y) has already been used for a correct color detection.
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
// Sensor Functions (unchanged)
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
// Motor Control Functions (Refactored for L298N)
// ===========================
//
// For the L298N, to move a motor you set the IN pins appropriately,
// and set the corresponding EN pin HIGH (to run) or LOW (to stop).
//
// Note: We assume “unit” movements where the delay represents a full-tile traverse or a 90° turn.

// Move forward for "units" tiles.
void forwardMove(float units) {
  // Set left motor forward: IN1 HIGH, IN2 LOW.
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(EN_A, 120);
  // Set right motor forward: IN3 HIGH, IN4 LOW.
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(EN_B, 120);

  delay(TILE_DURATION * units);
  // Stop motors.
  digitalWrite(EN_A, LOW);
  digitalWrite(EN_B, LOW);
  delay(200);
}

// Move backward for "units" tiles.
void backwardMove(float units) {
  // Left motor backward: IN1 LOW, IN2 HIGH.
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(EN_A, 120);
  // Right motor backward: IN3 LOW, IN4 HIGH.
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(EN_B, 120);

  delay(TILE_DURATION * units);
  digitalWrite(EN_A, LOW);
  digitalWrite(EN_B, LOW);
  delay(200);
}

// Turn right (90°) for "units" (units typically = 1).
void rightTurnMove(float units) {
  // To turn right, run left motor forward and right motor backward.
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(EN_A, 120);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  digitalWrite(EN_B, 120);

  delay(TURN_DURATION * units);
  digitalWrite(EN_A, LOW);
  digitalWrite(EN_B, LOW);
  delay(200);
}

// Turn left (90°) for "units".
void leftTurnMove(float units) {
  // To turn left, run left motor backward and right motor forward.
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(EN_A, 120);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  digitalWrite(EN_B, 120);
  delay(TURN_DURATION * units);
  digitalWrite(EN_A, LOW);
  digitalWrite(EN_B, LOW);
  delay(200);
}

// ===========================
// Navigation Helper Functions
// ===========================

// Turn the robot to face targetOrientation (0 = North, 1 = East, 2 = South, 3 = West).
// The currentOrientation is updated accordingly.
void turnTo(int targetOrientation) {
  int diff = targetOrientation - currentOrientation;
  if (diff < 0)
    diff += 4;
  if (diff == 0) {
    return; // Already facing target.
  } else if (diff == 1) {
    rightTurnMove(1);
    currentOrientation = (currentOrientation + 1) % 4;
  } else if (diff == 2) {
    rightTurnMove(1);
    rightTurnMove(1);
    currentOrientation = (currentOrientation + 2) % 4;
  } else if (diff == 3) {
    leftTurnMove(1);
    currentOrientation = (currentOrientation + 3) % 4;
  }
  Serial.print("New Orientation: ");
  Serial.println(currentOrientation);
}

// Move one tile forward (using calibrated forwardMove) and update grid coordinates.
void moveOneStep() {
  forwardMove(1);
  if (currentOrientation == 0)         // North
    currentY++;
  else if (currentOrientation == 1)    // East
    currentX++;
  else if (currentOrientation == 2)    // South
    currentY--;
  else if (currentOrientation == 3)    // West
    currentX--;
  
  Serial.print("New Position: (");
  Serial.print(currentX);
  Serial.print(", ");
  Serial.print(currentY);
  Serial.println(")");
}

// ===========================
// Robust Backtracking Algorithm
// ===========================
// When an obstacle is detected, this algorithm examines the current decision point (top of stack)
// and attempts to find an untried adjacent tile. If none is found, it pops the decision point and
// moves backward one tile, updating the current grid coordinates.
void backtrack() {
  Serial.println("Initiating backtracking...");
  
  while (stackTop >= 0) {
    // Look at the top of the stack: current decision point.
    StackTile currentTile = backtrackStack[stackTop];
    bool foundAlternative = false;
    
    // Check each unattempted direction (0 = North, 1 = East, 2 = South, 3 = West).
    for (int dir = 0; dir < 4; dir++) {
      if (!(currentTile.attempted & (1 << dir))) {
        // Mark this direction as attempted.
        backtrackStack[stackTop].attempted |= (1 << dir);
        
        // Turn to face that direction.
        turnTo(dir);
        int d = getDistance();
        if (d > 10) {  // Clear path.
          // Compute neighbor tile coordinates.
          int newX = currentTile.x;
          int newY = currentTile.y;
          if (dir == 0) newY++;       // North
          else if (dir == 1) newX++;  // East
          else if (dir == 2) newY--;  // South
          else if (dir == 3) newX--;  // West
          
          // Check that this neighbor is not already in our backtracking stack (avoid loops).
          bool inStack = false;
          for (int i = 0; i <= stackTop; i++) {
            if (backtrackStack[i].x == newX && backtrackStack[i].y == newY) {
              inStack = true;
              break;
            }
          }
          if (!inStack) {
            // Move into the new tile.
            moveOneStep();
            pushToStack(currentX, currentY);
            foundAlternative = true;
            Serial.print("Alternative route found. New position: (");
            Serial.print(currentX);
            Serial.print(", ");
            Serial.print(currentY);
            Serial.println(")");
            break;  // Exit loop – alternative found.
          }
        }
      }
    }
    
    if (foundAlternative) {
      Serial.println("Exiting backtracking.");
      return;
    } else {
      // No alternative at this decision point; pop it and move backward one tile.
      Serial.println("No alternative found at current tile, backing up.");
      popFromStack();
      backwardMove(1);
      // Update current grid coordinates to the new top-of-stack, if available.
      if (stackTop >= 0) {
        currentX = backtrackStack[stackTop].x;
        currentY = backtrackStack[stackTop].y;
        Serial.print("Backtracked to tile: (");
        Serial.print(currentX);
        Serial.print(", ");
        Serial.print(currentY);
        Serial.println(")");
      } else {
        Serial.println("Backtracked to start; no alternatives available.");
        break;
      }
    }
  }
  Serial.println("Backtracking complete.");
}

// ===========================
// Color Sensor Reading Functions (unchanged)
// ===========================
int getRedPW() {
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  return pulseIn(sensorOut, LOW);
}

int getGreenPW() {
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  return pulseIn(sensorOut, LOW);
}

int getBluePW() {
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

  // --- LED Setup ---
  pinMode(LED_PIN, OUTPUT);

  // --- Motor Pins Setup for L298N ---
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN_B, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  Serial.begin(9600);
  Serial.println("Setup Complete (L298N with robust backtracking)");
  Serial.println("Starting at (0,0), facing North.");

  // Initialize the backtracking stack with the starting tile.
  pushToStack(currentX, currentY);
}

void loop() {
  // --- Color Detection Logic ---
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
  // If path is clear (distance > 10), process the tile.
  if (distance > 10) {
    // Only accept the tile if the detected color matches the next expected color
    // and if this tile hasn’t been used before.
    if (detectedColor == sequence[sequenceIndex] && !isTileVisited(currentX, currentY)) {
      blinkLED();
      markTileVisited(currentX, currentY, detectedColor);
      pushToStack(currentX, currentY);
      sequenceIndex++;
      Serial.print("Color matched! Sequence index: ");
      Serial.println(sequenceIndex);
      if (sequenceIndex >= sizeof(sequence) / sizeof(sequence[0])) {
        Serial.println("Sequence complete!");
        while (true); // Halt execution.
      }
    } else if (isTileVisited(currentX, currentY)) {
      Serial.println("Duplicate tile detected, skipping.");
    } else {
      Serial.println("Color mismatch, skipping.");
    }
    // Move forward one tile.
    moveOneStep();
  } else {
    // If an obstacle is detected (distance <= 10), initiate backtracking.
    Serial.println("Obstacle detected! Initiating backtracking.");
    backtrack();
  }
}