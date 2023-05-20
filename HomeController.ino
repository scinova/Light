#include "Light.h"

#include <Arduino.h>
#include <math.h>
#define LED_BUILTIN 2

#define DECODE_NEC
#include <IRremote.hpp>

//#define DEBUG

#define IR_PIN 23 //12
#define WHITE_PIN 22 //9
#define RED_PIN 19 //3
#define GREEN_PIN 18 //5
#define BLUE_PIN 5 //6

Light rgblight = Light(RED_PIN, GREEN_PIN, BLUE_PIN, 1, 2, 3);
//Light light = Light(WHITE_PIN);
long _commandTime;

void setup() {
#ifdef DEBUG
	Serial.begin(115200);
#endif
	IrReceiver.begin(IR_PIN);
	rgblight.setColor(0, 1);
}

#define KEYESTUDIO_REMOTE
#define REPEAT_IGNORE_DURATION 1000

#ifndef KEYESTUDIO_REMOTE
#define KEY_ON 0xa2
#define KEY_UP 2
#define KEY_DOWN 152
#define KEY_LEFT 224
#define KEY_RIGHT 144
#define KEY_STAR 104
#define KEY_POUND 176
#else
#define KEY_ON 64
#define KEY_UP 70
#define KEY_DOWN 21
#define KEY_LEFT 68
#define KEY_RIGHT 67
#define KEY_STAR 66
#define KEY_POUND 74
#endif

void loop() {
	long now = millis();
	if (IrReceiver.decode()) {
		int command = IrReceiver.decodedIRData.command;
		if (command == KEY_ON && _commandTime + REPEAT_IGNORE_DURATION > now) {
			IrReceiver.resume();
			return;
		}
		_commandTime = now;
#ifdef DEBUG
		Serial.println(command);
#endif
		switch (command) {
			case KEY_ON:
				//light.toggle();
				rgblight.toggle();
				break;
			case KEY_UP:
				//light.increaseLevel();
				rgblight.increaseLevel();
				break;
			case KEY_DOWN:
				//light.decreaseLevel();
				rgblight.decreaseLevel();
				break;
			case KEY_LEFT:
				rgblight.decreaseHue();
				break;
			case KEY_RIGHT:
				rgblight.increaseHue();
				break;
			case KEY_STAR:
				rgblight.decreaseSaturation();
				break;
			case KEY_POUND:
				rgblight.increaseSaturation();
				break;
		}
		IrReceiver.resume();
	}
#define BLINK_DURATION 100
	if (now - _commandTime < BLINK_DURATION)
		digitalWrite(LED_BUILTIN, HIGH);
	else
		digitalWrite(LED_BUILTIN, LOW);

	//light.loop();
	rgblight.loop();
#ifdef DEBUG
	//light.log();
	rgblight.log();
	delay(100);
#endif
}
