/**
 * @file main.cpp
 * @author Riccardo Iacob
 *
 * @brief Interfacing of a rotary encoder with Arduino, using non-blocking interrupts.
 * @note LOGIC IS INVERTED SINCE ENCODER IS PULLED UP
 * @note Circuit has been modified to include hardware debouncing. https://hackaday.com/2015/12/09/embed-with-elliot-debounce-your-noisy-buttons-part-i/
 *
 * @date 2023-04-06
 * @copyright Copyright (c) 2023
 */
#include <Arduino.h>

// Power pins (for pull-up resistors)
#define PIN_ENC_PWR 5
#define PIN_ENC_GND 6
// Shaft click button pin
#define PIN_ENC_BTN 4
// Rotation pin A
#define PIN_ENC_A 2
// Rotation pin B
#define PIN_ENC_B 3

// Encoder-related data
// If the encoder was updated
volatile bool enc_newvalue = false;
// The variable incremented or decremented by the encoder's rotation
volatile int enc_value = 0;
// Phase of the rotation. 0 is ccw and 1 is cw.
volatile bool enc_phase = 0;

// Encoder ISR
void isr_enc();

void setup()
{
  pinMode(PIN_ENC_PWR, OUTPUT);
  pinMode(PIN_ENC_GND, OUTPUT);
  // Use internal pull-up since there is no pull-up resistor soldered on the module
  pinMode(PIN_ENC_BTN, INPUT_PULLUP);
  // Use external pull-up resistors soldered to the module
  pinMode(PIN_ENC_A, INPUT);
    // Use external pull-up resistors soldered to the module
  pinMode(PIN_ENC_B, INPUT);
  // Pull up the encoder's outputs
  digitalWrite(PIN_ENC_PWR, HIGH);
  digitalWrite(PIN_ENC_GND, LOW);
  // Attach interrupt to rising edge of A pin
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_A), isr_enc, FALLING);
  // Begin serial
  Serial.begin(9600);
}

void loop()
{
  // If the encoder was moved, print the new value to serial
  if (enc_newvalue)
  {
    enc_newvalue = false;
    Serial.println(enc_value);
  }
}

void isr_enc()
{
  // Disable interrupts because we're accessing a multi-byte variable (enc_value)
  noInterrupts();
  enc_newvalue = true;
  // We know that A = true, else we would not be in this ISR.
  // If B = A, the knob is turned counterclockwise
  if (!digitalRead(PIN_ENC_B))
  {
      enc_value--;
      enc_phase = 0;
  }
  // If B != A, then the knob is turned clockwise
  else
  {
    enc_value++;
  }
  // Re-enable interrupts
  interrupts();
}