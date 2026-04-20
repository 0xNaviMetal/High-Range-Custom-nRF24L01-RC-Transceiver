
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftPWM.h>

RF24 radio(A1, A2);
const byte address[6] = "00001";

struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
};

Data_Package data;

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

const int pwmPins[6] = {3, 5, 6, 9, 10, A3};
const int digitalPins[4] = {4, 7, 8, A4};

void setup() {
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  for (int i = 0; i < 5; i++) pinMode(pwmPins[i], OUTPUT);
  for (int i = 0; i < 4; i++) pinMode(digitalPins[i], OUTPUT);

  SoftPWM.begin();
  SoftPWM.set(pwmPins[5], 0);

  resetData();
}

void loop() {
  if (radio.available()) {
    radio.read(&data, sizeof(Data_Package));
    lastReceiveTime = millis();
  }

  currentTime = millis();
  if (currentTime - lastReceiveTime > 1000) resetData();

  Serial.print("J1X: "); Serial.print(data.j1PotX);
  Serial.print(" | J1Y: "); Serial.print(data.j1PotY);
  Serial.print(" | J2X: "); Serial.print(data.j2PotX);
  Serial.print(" | J2Y: "); Serial.print(data.j2PotY);
  Serial.print(" | P1: "); Serial.print(data.pot1);
  Serial.print(" | P2: "); Serial.print(data.pot2);
  Serial.print(" | J1Btn: "); Serial.print(data.j1Button);
  Serial.print(" | J2Btn: "); Serial.print(data.j2Button);
  Serial.print(" | T1: "); Serial.print(data.tSwitch1);
  Serial.print(" | T2: "); Serial.println(data.tSwitch2);

  for (int i = 0; i < 5; i++) analogWrite(pwmPins[i], (&data.j1PotX)[i]);
  SoftPWM.set(pwmPins[5], data.pot2);

  digitalWrite(digitalPins[0], data.j1Button == 0 ? HIGH : LOW);
  digitalWrite(digitalPins[1], data.j2Button == 0 ? HIGH : LOW);
  digitalWrite(digitalPins[2], data.tSwitch1 == 0 ? HIGH : LOW);
  digitalWrite(digitalPins[3], data.tSwitch2 == 0 ? HIGH : LOW);
}

void resetData() {
  data.j1PotX = 127;
  data.j1PotY = 127;
  data.j2PotX = 127;
  data.j2PotY = 127;
  data.j1Button = 1;
  data.j2Button = 1;
  data.pot1 = 127;
  data.pot2 = 127;
  data.tSwitch1 = 1;
  data.tSwitch2 = 1;
}
