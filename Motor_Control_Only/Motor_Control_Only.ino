// Define color sensor pins
#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 2
#define OE_PIN 3




//Sonar pins
const int trigPin = 9;
const int echoPin = 10;

//Motor Pins
int EN_A = A0;  // Left Motor Enable
int IN1  = A1;  // Left Motor Control 1
int IN2  = 10;  // Left Motor Control 2
// Right Motor
int IN3  = 11;  // Right Motor Control 1
int IN4  = 12;  // Right Motor Control 2
int EN_B = 13;  // Right Motor Enable (using an analog pin as digital)


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
  //Motor Test
  int step = 1;

  Motor_R(1);
  Motor_(1);



  //------------------------------------------------------------Color detection logic-----------------------------------------

}




















