#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define motorPin 11  // Added motorPin definition
const int trigPin = 9;
const int echoPin = 10;

// Sequence definition
char sequence[] = {'R', 'G', 'B', 'G', 'B'};
int sequenceIndex = 0;

// Color sensor mapping values
int redMin = 0, redMax = 1118;
int greenMin = 0, greenMax = 2200;
int blueMin = 3, blueMax = 2200;

// Variables for Color Pulse Width Measurements
int redPW = 0, greenPW = 0, bluePW = 0;

// Variables for final Color values
int redRaw, greenRaw, blueRaw;

// Visited tiles tracking (dummy coordinate system for now)
struct Tile { int x; int y; };
Tile visited[100];
int visitedCount = 0;

// Stack for backtracking
struct StackTile { int x; int y; };
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

bool isStackEmpty() { return stackTop == -1; }

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motorPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Setup Complete");
}

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

bool isTileVisited(int x, int y) {
  for (int i = 0; i < visitedCount; i++) {
    if (visited[i].x == x && visited[i].y == y)
      return true;
  }
  return false;
}

void markTileVisited(int x, int y) {
  visited[visitedCount].x = x;
  visited[visitedCount].y = y;
  visitedCount++;
}

void moveOneStep() {
  analogWrite(motorPin, 255); // Move forward at full speed
  delay(1000);                // Move for 1 second (1 step)
  analogWrite(motorPin, 0);   // Stop motor
}

void backtrack() {
  Serial.println("Backtracking...");
  while (!isStackEmpty()) {
    StackTile tile = popFromStack();
    Serial.print("Backtracking to tile: ");
    Serial.print(tile.x);
    Serial.print(", ");
    Serial.println(tile.y);
    delay(1000); 
  }
}

void loop() {
  // Read pulse widths using your custom functions
  redPW = getRedPW();
  redRaw = map(redPW, redMin, redMax, 255, 0);
  delay(200);
  
  greenPW = getGreenPW();
  greenRaw = map(greenPW, greenMin, greenMax, 255, 0);
  delay(200);
  
  bluePW = getBluePW();
  blueRaw = map(bluePW, blueMin, blueMax, 255, 0);
  delay(200);

  // Determine dominant color
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

  // Check if the detected color matches the sequence for this tile
  if (distance > 10) {
    if (detectedColor == sequence[sequenceIndex] && !isTileVisited(sequenceIndex, sequenceIndex)) {
      markTileVisited(sequenceIndex, sequenceIndex); // Update with proper coordinates as needed
      pushToStack(sequenceIndex, sequenceIndex);
      sequenceIndex++;
      Serial.println("Color matched! Moving to next in sequence.");
    } else if (isTileVisited(sequenceIndex, sequenceIndex)) {
      Serial.println("Duplicate tile detected, skipping.");
    }
    moveOneStep();
  } else {
    Serial.println("Obstacle detected! Initiating backtracking.");
    backtrack();
  }

  if (sequenceIndex >= sizeof(sequence) / sizeof(sequence[0])) {
    Serial.println("Sequence complete!");
    while (true);  // Halt further execution
  }
}

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
