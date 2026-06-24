#include <Servo.h>

Servo scanner;
Servo aimServo;

// --- PINS ---
#define FLAME 2
#define PUMP_PIN 4
#define SCAN_SERVO_PIN 13
#define AIM_SERVO_PIN 12

// Motors (Standard Pins)
int IN1 = 8; int IN2 = 9;
int IN3 = 10; int IN4 = 11;

// --- TIMING & SCANNING VARIABLES ---
int scanAngle = 45; 
int scanStep = 2;              
unsigned long lastScanMillis = 0;
const int scanSpeed = 20;      

unsigned long movementTimer = 0;
bool isMoving = true;

void setup() {
  Serial.begin(9600);
  
  pinMode(FLAME, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);

  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);

  scanner.attach(SCAN_SERVO_PIN);
  aimServo.attach(AIM_SERVO_PIN);
  
  scanner.write(scanAngle);
  aimServo.write(90);
  
  movementTimer = millis(); // Start the first 5-second timer
}

// --- MOVEMENT ---
void forward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void stopCar() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// --- MAIN LOOP ---
void loop() {
  
  if (millis() - lastScanMillis >= scanSpeed) {
    lastScanMillis = millis();
    scanAngle += scanStep;
    if (scanAngle >= 135 || scanAngle <= 45) {
      scanStep = -scanStep; 
    }
    scanner.write(scanAngle);
  }

  // 2. FLAME DETECTION 
  if (digitalRead(FLAME) == LOW) {
    stopCar();
    aimServo.write(scanAngle); // Aim nozzle at detected fire
    delay(500);
    digitalWrite(PUMP_PIN, HIGH);
    delay(3000); // Spray for 3 seconds
    digitalWrite(PUMP_PIN, LOW);
    aimServo.write(90);
    movementTimer = millis(); // Reset movement timer after firefighting
  }

  if (millis() - movementTimer >= 5000) {
    isMoving = !isMoving;     
    movementTimer = millis(); 
    
    if (isMoving) {
      Serial.println("Moving Forward...");
    } else {
      Serial.println("Stopping for 5 seconds...");
    }
  }

  // Execute the current state
  if (isMoving) {
    forward();
  } else {
    stopCar();
  }
}
