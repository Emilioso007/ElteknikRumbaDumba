#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CE, CSN

const byte address[6] = "00022";

bool lValue = false, lValue1 = false, lValue2 = true;

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(4, INPUT);
  digitalWrite(4, HIGH);

}

void loop() {
  int xValue = analogRead(A1);
  int yValue = analogRead(A0);

  lValue2 = !lValue1;

  lValue1 = !digitalRead(4);

  lValue = (lValue1 && lValue2);

  Serial.print("X = ");
  Serial.print(xValue);
  Serial.print(" Y = ");
  Serial.print(yValue);
  Serial.print(" lValue = ");
  Serial.println(lValue);

 char text[20];
  sprintf(text, "%d,%d,%d", xValue, yValue, lValue);
  radio.write(&text, sizeof(text));
  delay(10);
}
