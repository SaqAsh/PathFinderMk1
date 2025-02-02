const int trigPin = 9;
const int echoPin = 10;

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT);  
  Serial.begin(9600);       
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

void loop() {
  int distance = getDistance(); 
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(500); 
}
