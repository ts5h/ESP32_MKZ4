/*
 * Copyright (c) 2015, Majenko Technologies All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// Full customized by ts5h, 2022

/* Create a WiFi access point and provide a web server on it. */
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClient.h>

/* Set these to your desired credentials. */
const char *ssid = "MKZ4";
const char *password = "";

WebServer server(80);
WebServer server_8080(8080);

/* Set I2C library*/
#include <Wire.h>
#define ADDR1 0x64

#define COMMAND_START 0
#define COMMAND_STOP 1
#define COMMAND_BACK 2
#define FORWARD 0x01
#define REVERSE 0x02

// For SG90 Servo motor
#include <ESP32Servo.h>

// Degrees / You have to adjust your servo for each individual
#define SERVO_PIN 16
const int SERVO_CENTER = 90;
const int SERVO_DEGREE = 16;

Servo servo;
char state = COMMAND_STOP;

#define LED_PIN 2
#define LED_H (digitalWrite(2, HIGH))
#define LED_L (digitalWrite(2, LOW))

const boolean debug = false;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void setup() {
  delay(1000);

  Serial.begin(115200);
  Serial.println("Configuring access point...");

  Wire.begin(4, 14);
  delay(40);

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  server.on("/", handle_root);
  server_8080.on("/stop", handle_stop);
  server_8080.on("/move", handle_move);

  server.begin();
  server_8080.begin();
  Serial.println("HTTP server started");

  servo.attach(SERVO_PIN);
  pinMode(LED_PIN, OUTPUT);
  
  delay(100);
  LED_H;
}

void loop() {
  server.handleClient();
  server_8080.handleClient();
}


// Control directions
void handle_root() {
  extern String form;
  server.send(200, "text/html", form);
}

void handle_stop() {
  if (debug) Serial.println("Stop");
  LED_L;
  servo.write(SERVO_CENTER);
  stop();
  server_8080.send(200, "text/html", "");
  LED_H;
}

void handle_move() {
  if (!server_8080.hasArg("x") || !server_8080.hasArg("y")) return;

  float x = server_8080.arg("x").toFloat();
  float y = server_8080.arg("y").toFloat();

  LED_L;

  // Left / Right
  if (x == 0) {
    servo.write(SERVO_CENTER);
  } else {
    int degree = SERVO_CENTER + round(SERVO_DEGREE * x);
    Serial.println(degree);
    servo.write(degree);
  }

  // Forward / Back
  if (y == 0) {
    stop();
  } else {
    y > 0 ? drive(y) : back(y);
  }

  server_8080.send(200, "text/html", "");
  LED_H;
}

void stop() {
  stop_motor();
  state = COMMAND_STOP;
}

void drive(float y) {
  if (debug) Serial.println("Forward");

  switch (state) {
    case COMMAND_BACK:
      stop_motor();
      delay(10);
      start_motor(y);
      break;

    case COMMAND_STOP:
      start_motor(y);
      break;
  }
  
  state = COMMAND_START;
}

void back(float y) {
  if (debug) Serial.println("Back");

  switch (state) {
    case COMMAND_START:
      stop_motor();
      delay(10);
      reverse_motor(y);
      break;

    case COMMAND_STOP:
      reverse_motor(y);
      break;
  }

  state = COMMAND_BACK;
}

void stop_motor() {
  motor_func(ADDR1, 0x18);
  delay(10);
  motor_func(ADDR1, 0x1B);
  delay(10);
}

void start_motor(float y) {
  char volt = 0x20;

  for (int i = 0; i < 5; i++) {
    volt = volt + ((0x40) * i);
    volt = volt | FORWARD;
    motor_func(ADDR1, volt);
    delay(10);
  }
}

void reverse_motor(float y) {
  char volt = 0x20;

  for (int i = 0; i < 5; i++) {
    volt = volt + ((0x40) * i);
    volt = volt | REVERSE;
    motor_func(ADDR1, volt);
    delay(10);
  }
}

void motor_func(char add, char duty) {
  Wire.beginTransmission(add);
  Wire.write(0x00);
  Wire.write(duty);
  Wire.endTransmission();
}
