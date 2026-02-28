#include <DHT.h>

#define DHTPIN 23
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensor Pins
const int flamePin = 22;
const int mq2Pin = A0;

// Output Pins
const int relayPower = 24;
const int relaySolenoid = 25;
const int buzzer = 26;
const int led = 27;

float tempThreshold = 50.0;      // temperature limit
int gasThreshold = 400;          // MQ2 threshold
bool fireDetected = false;

void setup() {

  Serial.begin(9600);
  Serial1.begin(115200);   // ESP-01 communication

  dht.begin();

  pinMode(flamePin, INPUT);
  pinMode(relayPower, OUTPUT);
  pinMode(relaySolenoid, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led, OUTPUT);

  digitalWrite(relayPower, HIGH);     // Normal state ON
  digitalWrite(relaySolenoid, HIGH);  // OFF initially
}

void loop() {

  // Read Sensors
  float temperature = dht.readTemperature();
  int gasValue = analogRead(mq2Pin);
  int flameStatus = digitalRead(flamePin);

  Serial.println("Monitoring...");

  // Fire Condition Check
  if ((temperature > tempThreshold) || 
      (gasValue > gasThreshold) || 
      (flameStatus == LOW)) {

      fireDetected = true;
      activateFireSystem();

  } else {

      fireDetected = false;
      normalMonitoring();
  }

  delay(2000);
}

void activateFireSystem() {

  Serial.println("FIRE DETECTED!");

  // Cut Power
  digitalWrite(relayPower, LOW);

  // Activate Suppression
  digitalWrite(relaySolenoid, LOW);

  // Alarm
  digitalWrite(buzzer, HIGH);
  digitalWrite(led, HIGH);

  sendAlert();
}

void normalMonitoring() {

  digitalWrite(relayPower, HIGH);
  digitalWrite(relaySolenoid, HIGH);
  digitalWrite(buzzer, LOW);
  digitalWrite(led, LOW);
}

void sendAlert() {

  Serial1.println("AT+CMGF=1");   // GSM mode if using GSM
  delay(1000);

  // OR WiFi message
  Serial1.println("Fire detected at EV Charging Station!");

  Serial.println("Alert Sent.");
}
