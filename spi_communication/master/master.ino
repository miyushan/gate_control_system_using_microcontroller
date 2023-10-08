#include <SPI.h>
#include "UUID.h"

#define max_tokens 50               // maximum number of tokens that can be stored in the slave
#define token_length 32             // length of the token
#define SS_2 10                     // Slave Select pin for the second slave

UUID uuid;

// token strings for both slaves
String gate_1_tokens[max_tokens];
String gate_2_tokens[max_tokens];

// initial position of the tokens
int gate_1_index = 0;
int gate_2_index = 0;

void setup(void) {
  Serial.begin(115200);

  pinMode(SS_2, OUTPUT);
  digitalWrite(SS, HIGH); 
  digitalWrite(SS_2, HIGH);
  SPI.begin();

  SPI.setClockDivider(SPI_CLOCK_DIV8);  // slow down the master
}

byte transferAndWait(const byte data) {
  byte a = SPI.transfer(data);
  delayMicroseconds(20);                // wait for slave to process
  return a;
}

void loop(void) {
  process_slave_1();
  process_slave_2();

  // token generation/ register a user
  if (Serial.available() > 0) {
    String receivedString = Serial.readStringUntil('\n');   // read the incoming data as string until '\n' is received

    if (receivedString.length() == 1) {
      receivedString.trim();                                // remove any leading and trailing whitespaces
      if (receivedString == "1") {
        register_user(1);
      } else if (receivedString == "2") {
        register_user(2);
      } else{
        Serial.println("Invalid Slave!!!");
      }
    }
    else{
      Serial.println("Invalid Length!!!");
    }
  }
}

void process_slave_1() {
  byte slave_input[token_length];
  bool valid_user = false;

  digitalWrite(SS, LOW);                                          // enable Slave Select

  byte slave_response = transferAndWait('a');                     // response from slave

  if (slave_response == 1) {
    // adding bytes to "slave_input" array
    for (int i = 0; i < token_length; i++) {
      slave_input[i] = transferAndWait(i);
    }
    slave_input[token_length-1] = 'a';
    Serial.println("");

    String slave_input_str = convertByteArrToString(slave_input); // convert byte array into a string 

    for (int i = 0; i < max_tokens; i++) {
      if (slave_input_str == gate_1_tokens[i]) {
        valid_user = true;                                        // verify whether uuid is present in the slave_1_tokens
      }
    }
  }

  digitalWrite(SS, HIGH);                                         // disable Slave Select
  delay(500);

  if (valid_user) {
    verify_slave1();                                              // send reply to the slave_1
  }
}

void process_slave_2() {
  String result = "";
  byte slave_input[token_length];
  bool valid_user = false;

  digitalWrite(SS_2, LOW);                                        // enable Slave Select

  byte slave_response = transferAndWait('a');                     // response from slave

  if (slave_response == 1) {
    // adding bytes to "slave_input" array
    for (int i = 0; i < token_length; i++) {
      slave_input[i] = transferAndWait(i);
    }
    slave_input[token_length-1] = 'a';

    String slave_input_str = convertByteArrToString(slave_input);

    for (int i = 0; i < max_tokens; i++) {
      if (slave_input_str == gate_2_tokens[i]) {
        valid_user = true;                                        // verify whether uuid is present in the slave_1_tokens
      }
    }
  }

  digitalWrite(SS_2, HIGH);                                       // disable Slave Select
  delay(500);

  if (valid_user) {
    verify_slave2();                                              // send reply to the slave_2
  }
}

void verify_slave1() {
  digitalWrite(SS, LOW);
  transferAndWait('g');
  digitalWrite(SS, HIGH);
  delay(500);
}
void verify_slave2() {
  digitalWrite(SS_2, LOW);
  transferAndWait('g');
  digitalWrite(SS_2, HIGH);
  delay(500);
}

void not_verify_slave1() {
  digitalWrite(SS, LOW);
  transferAndWait('n');
  digitalWrite(SS, HIGH);
  delay(500);
}
void not_verify_slave2() {
  digitalWrite(SS_2, LOW);
  transferAndWait('n');
  digitalWrite(SS_2, HIGH);
  delay(500);
}

String convertByteArrToString(byte byteArray[]) {
  String result = "";

  for (int i = 0; i < token_length; i++) {
    result += char(byteArray[i]);
  }
  return result;
}

String generateToken() {
  String finalToken = "";

  uuid.generate();
  String token = String(uuid.toCharArray());

  for (int i = 0; i < token.length(); i++) {
    if (token.charAt(i) != '-') {
      finalToken += token.charAt(i);
    }
  }

  return finalToken;
}

void register_user(int slave_id) {
  if (slave_id == 1) {
    String temp_token = generateToken();
    temp_token[token_length-1] = 'a';
    // Serial.print("UUID: ");
    Serial.println(temp_token);
    gate_1_tokens[gate_1_index] = temp_token;

    if (gate_1_index == max_tokens - 1) {
      gate_1_index = 0;
    } else {
      gate_1_index++;
    }
  } else if (slave_id == 1) {
    String temp_token = generateToken();
    temp_token[token_length-1] = 'a';
    // Serial.print("UUID: ");
    Serial.println(temp_token);
    gate_2_tokens[gate_2_index] = temp_token;

    if (gate_2_index == max_tokens - 1) {
      gate_2_index = 0;
    } else {
      gate_2_index++;
    }
  }
}