// Define color sensor pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8
#define OE_PIN 3




//Sonar pins
const int trigPin = 9;
const int echoPin = 10;

//Motor Pins
int EN_A = 11; //Enable pin for first motor
int IN1 = 9; //control pin for first motor
int IN2 = 8; //control pin for first motor
int IN3 = 7; //control pin for second motor
int IN4 = 6; //control pin for second motor
int EN_B = 10; //Enable pin for second motor


// Calibration Values
// *Get these from Calibration Sketch
int redMin = 0; // Red minimum value
int redMax = 1118; // Red maximum value
int greenMin = 0; // Green minimum value
int greenMax = 2200; // Green maximum value
int blueMin = 3; // Blue minimum value
int blueMax = 2200; // Blue maximum value

// Variables for Color Pulse Width Measurements
int redPW = 0;
int greenPW = 0;
int bluePW = 0;

// Variables for final Color values
int redRaw;
int greenRaw;
int blueRaw;

int redVal;
int greenVal;
int blueVal;

void setup() {
	// Set S0 - S3 as outputs
	pinMode(S0, OUTPUT);
	pinMode(S1, OUTPUT);
	pinMode(S2, OUTPUT);
	pinMode(S3, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  digitalWrite(OE_PIN, LOW);

  //sonar sensor
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);  

	// Set Sensor output as input
	pinMode(sensorOut, INPUT);

	// Set Frequency scaling to 20%
	digitalWrite(S0,HIGH);
	digitalWrite(S1,LOW);

	// Setup Serial Monitor
	Serial.begin(9600);

  //Initializing the motor pins as output
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);
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

// Function to read Red Pulse Widths
int getRedPW() {
	// Set sensor to read Red only
	digitalWrite(S2,LOW);
	digitalWrite(S3,LOW);
	// Define integer to represent Pulse Width
	int PW;
	// Read the output Pulse Width
	PW = pulseIn(sensorOut, LOW);
	// Return the value
	return PW;
}

// Function to read Green Pulse Widths
int getGreenPW() {
	// Set sensor to read Green only
	digitalWrite(S2,HIGH);
	digitalWrite(S3,HIGH);
	// Define integer to represent Pulse Width
	int PW;
	// Read the output Pulse Width
	PW = pulseIn(sensorOut, LOW);
	// Return the value
	return PW;
}

// Function to read Blue Pulse Widths
int getBluePW() {
	// Set sensor to read Blue only
	digitalWrite(S2,LOW);
	digitalWrite(S3,HIGH);
	// Define integer to represent Pulse Width
	int PW;
	// Read the output Pulse Width
	PW = pulseIn(sensorOut, LOW);
	// Return the value
	return PW;
}

void Motor_R(int speed){ //RIGHT MOTOR
//Move forward --- Low Speed
  if (speed == -1 ) {
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
	analogWrite(EN_B, 150);
  }

  if (speed == 0 ) {
	digitalWrite(IN3, LOW);
	digitalWrite(IN4, HIGH);
	analogWrite(EN_B, 0);
  }

  if (speed == 1 ) {
	digitalWrite(IN3, HIGH);
	digitalWrite(IN4, LOW);
	analogWrite(EN_B, 150);
  }
}

void Motor_L(int speed){ //RIGHT MOTOR
//Move forward --- Low Speed
  if (speed == -1) {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	analogWrite(EN_A, 150);
  }

  if (speed == 0) {
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, HIGH);
	analogWrite(EN_A, 0);
  }

  if (speed == 1 ) {
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	analogWrite(EN_A, 150);
  }

}


void forward(float unit){
  Motor_R(1);
  Motor_L(1);

  delay(200*unit);

  Motor_R(0);
  Motor_L(0);

  delay(200*unit);
}

void backward(float unit){
  Motor_R(-1);
  Motor_L(-1);

  delay(200*unit);

  Motor_R(0);
  Motor_L(0);

  delay(200*unit);

}

void right(float unit){
  Motor_R(-1);
  Motor_L(1);

  delay(200*unit);

  Motor_R(0);
  Motor_L(0);

  delay(200*unit);

}

void left(float unit){
  Motor_R(1);
  Motor_L(-1);

  delay(200*unit);

  Motor_R(0);
  Motor_L(0);

  delay(200*unit);

}



///-------------------------------------------------------------------------------------------------------------------------------------------------------------/////
void loop() {
/*
  int distance = getDistance();//-----------------------------------------------------------Distance Logic

  //------------------------------------------------------------Color detection logic-----------------------------------------
	// Read Red value
	redPW = getRedPW();
	// Map to value from 0-255
	redRaw = map(redPW, redMin,redMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Read Green value
	greenPW = getGreenPW();
	// Map to value from 0-255
	greenRaw = map(greenPW, greenMin,greenMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Read Blue value
	bluePW = getBluePW();
	// Map to value from 0-255
	blueRaw = map(bluePW, blueMin,blueMax,255,0);
	// Delay to stabilize sensor
	delay(200);

	// Print output to Serial Monitor
  if (redRaw > greenRaw && redRaw > blueRaw){
    Serial.print("Red = ");
	  Serial.print("1");
    Serial.print("\n");
    redVal = 1;
  }else if (blueRaw > greenRaw && blueRaw > redRaw){
    Serial.print("blue = ");
	  Serial.print("1");
    Serial.print("\n");
    blueVal = 1;
  }else if (greenRaw > blueRaw && greenRaw > redRaw){
    Serial.print("green = ");
	  Serial.print("1");
    Serial.print("\n");
    greenVal = 1;
  }

*/
  //Motor Test
  int step = 1;

  forward(step);
  right(step);
  forward(step);
  left(step);
  forward(step);

  backward(step);


  //INFINIT STOP!!!!!!!!!!!!!!!!!!!!!!!!!!
  while(true){
    int x = 5;
  }
  //------------------------------------------------------------Color detection logic-----------------------------------------

}




















