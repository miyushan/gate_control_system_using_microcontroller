#include "UUID.h"

volatile byte command = 0;
UUID uuid;

const int bufferSize = 32;
char stringBuffer[bufferSize];
bool user_input_available = false;
int current_byte = 0;
bool open_gate = false;
void setup(void) {

  // have to send on master in, slave out
  pinMode(MISO, OUTPUT);

  // turn on SPI in slave mode
  SPCR |= _BV(SPE);

  pinMode(7, OUTPUT);

  // turn on interrupts
  SPCR |= _BV(SPIE);
  Serial.begin(9600);
}


// SPI interrupt routine
ISR(SPI_STC_vect) {
  byte c = SPDR;
  if (c == 'g') {
    Serial.println("Welcome");
    open_gate = true;
  } else{
    // Serial.println("User not verified...");
  }

  switch (command) {
    // no command? then this is the command
    case 0:
      if (user_input_available) {
        current_byte = 0;
        command = c;
        SPDR = stringBuffer[current_byte];
        current_byte++;
      }
      break;

    case 'a':
      if (current_byte <= bufferSize) {
        SPDR = stringBuffer[current_byte];
        current_byte++;
      } else {
        current_byte = 0;
        user_input_available = false;
      }
      break;
  }

}  // end of interrupt service routine (ISR) SPI_STC_vect

void loop(void) {
  delay(100);
  if (digitalRead(SS) == HIGH) {
    command = 0;
  }

  if(open_gate){
    // Turn the LED on (HIGH)
    digitalWrite(7, HIGH);
    delay(1000); // Wait for 1 second (1000 milliseconds)

    // Turn the LED off (LOW)
    digitalWrite(7, LOW);
    delay(1000); // Wait for 1 second (1000 milliseconds)

    open_gate = false;
    // Serial.println("Gate closed...");
  }

  if (Serial.available() > 0) {
    // Read the incoming string until a newline character is received
    String receivedString = Serial.readString();

    // Check if the received string has exactly 32 characters
    if (receivedString.length() == bufferSize) {
      // Remove the trailing newline character
      receivedString.trim();

      // Copy the contents of the received string to the buffer
      receivedString.toCharArray(stringBuffer, bufferSize);
      user_input_available = true;
      SPDR = 1;
      // Print the received and stored string for verification
      // Serial.println("Received and transferred: " + String(stringBuffer));
    } else {
      Serial.println("Invalid input. The string must have exactly 32 characters.");
    }
  }
}

String generateUUID() {
  uuid.generate();
  String uuid_str = String(uuid.toCharArray());
  return removeCharFromString(uuid_str, '-');
}

String removeCharFromString(String inputString, char charToRemove) {
  String outputString = "";

  for (int i = 0; i < inputString.length(); i++) {
    if (inputString.charAt(i) != charToRemove) {
      outputString += inputString.charAt(i);
    }
  }

  return outputString;
}