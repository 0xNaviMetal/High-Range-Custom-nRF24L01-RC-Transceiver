#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// Define pins
#define jB1 2  // Joystick button 1
#define jB2 3  // Joystick button 2
#define t1  4  // Toggle switch 1
#define t2  5  // Toggle switch 2
#define BUZZER 9  // Buzzer pin

RF24 radio(7, 8);  // CE, CSN
const byte address[6] = "00001";

// Struct (must stay < 32 bytes)
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte tSwitch1;
  byte tSwitch2;
};

Data_Package data;
unsigned long lastPrintTime = 0;

// For detecting button/toggle state change
byte prevJB1 = 1, prevJB2 = 1;
byte prevT1 = 1, prevT2 = 1;

void setup() {
  Serial.begin(9600);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);

  pinMode(jB1, INPUT_PULLUP);
  pinMode(jB2, INPUT_PULLUP);
  pinMode(t1, INPUT_PULLUP);
  pinMode(t2, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // Init default values
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j1Button = 1;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j2Button = 1;
  data.pot1 = 127;
  data.tSwitch1 = 1;
  data.tSwitch2 = 1;
}

void loop() {
  // Read analog inputs
  data.j1PotX = map(analogRead(A1), 0, 1023, 0, 255);
  data.j1PotY = map(analogRead(A0), 0, 1023, 0, 255);
  data.j2PotX = map(analogRead(A2), 0, 1023, 0, 255);
  data.j2PotY = map(analogRead(A3), 0, 1023, 0, 255);
  data.pot1   = map(analogRead(A4), 0, 1023, 0, 255);

  // Read digital inputs
  data.j1Button = digitalRead(jB1);
  data.j2Button = digitalRead(jB2);
  data.tSwitch1 = digitalRead(t1);
  data.tSwitch2 = digitalRead(t2);

  // Buzzer: Beep on state change (falling edge)
  if (data.j1Button == 0 && prevJB1 == 1) beep();
  if (data.j2Button == 0 && prevJB2 == 1) beep();
  if (data.tSwitch1 == 0 && prevT1 == 1) beep();
  if (data.tSwitch2 == 0 && prevT2 == 1) beep();

  // Update previous states
  prevJB1 = data.j1Button;
  prevJB2 = data.j2Button;
  prevT1 = data.tSwitch1;
  prevT2 = data.tSwitch2;

  // Send data
  radio.write(&data, sizeof(Data_Package));

  // Serial Print every 200ms
  if (millis() - lastPrintTime >= 200) {
    Serial.println("=== Transmitter Data ===");
    Serial.print("J1 X: "); Serial.print(data.j1PotX);
    Serial.print(" | J1 Y: "); Serial.print(data.j1PotY);
    Serial.print(" | J1 Btn: "); Serial.println(data.j1Button);

    Serial.print("J2 X: "); Serial.print(data.j2PotX);
    Serial.print(" | J2 Y: "); Serial.print(data.j2PotY);
    Serial.print(" | J2 Btn: "); Serial.println(data.j2Button);

    Serial.print("Pot1: "); Serial.print(data.pot1);

    Serial.print(" | Toggle 1: "); Serial.print(data.tSwitch1);
    Serial.print(" | Toggle 2: "); Serial.println(data.tSwitch2);
    Serial.println();

    lastPrintTime = millis();
  }
}

void beep() {
  digitalWrite(BUZZER, HIGH);
  delay(100);
  digitalWrite(BUZZER, LOW);
}
