#include "Light.h"
#include <math.h>

#define FADEIN_DURATION 1000
#define FADEOUT_DURATION 1500
#define LEVEL_STEPS 10
#define HUE_STEPS 32
#define SATURATION_STEPS 32

struct RGB hsv2rgb(double h, double s, double v) {
	double r, g, b;
	double p, q, t, fract;
	(h == 360.) ? (h = 0.) : (h /= 60.);
	fract = h - floor(h);
	p = v * (1. - s);
	q = v * (1. - s * fract);
	t = v * (1. - s * (1. - fract));
	if (0 <= h && h < 1) {
		r = v; g = t; b = p;
		} else if (1 <= h && h < 2) {
		r = q; g = v; b = p;
		} else if (2 <= h && h < 3) {
		r = p; g = v; b = t;
	  } else if (3 <= h && h < 4) {
		r = p; g = q; b = v;
	} else if (4 <= h && h < 5) {
		r = t; g = p; b = v;
	} else if (5 <= h && h < 6) {
		r = v; g = p; b = q;
	} else {
		r = 0; g = 0; b = 0;
		}
		struct RGB rgb;
		rgb.r = r; rgb.g = g; rgb.b = b;
	return rgb;
}

#if defined(ARDUINO_ARCH_ESP32)
int _num_channels=0;
#endif

Light::Light(int pin) {
	_is_color_capable = false;
	_pin = pin;
#if defined(ARDUINO_ARCH_ESP32)
	_channel = _num_channels++;
	ledcSetup(_channel, 5000, 12);
	ledcAttachPin(pin, _channel);
#else
	pinMode(pin, OUTPUT);
#endif
}

Light::Light(int red_pin, int green_pin, int blue_pin) {
	_is_color_capable = true;
	_red_pin = red_pin;
	_green_pin = green_pin;
	_blue_pin = blue_pin;
#if defined(ARDUINO_ARCH_ESP32)
	_red_channel = _num_channels++;
	ledcSetup(_red_channel, 5000, 12);
	ledcAttachPin(red_pin, _red_channel);
	_green_channel = _num_channels++;
	ledcSetup(_green_channel, 5000, 12);
	ledcAttachPin(green_pin, _green_channel);
	_blue_channel = _num_channels++;
	ledcSetup(_blue_channel, 5000, 12);
	ledcAttachPin(blue_pin, _blue_channel);
#else
	pinMode(red_pin, OUTPUT);
	pinMode(green_pin, OUTPUT);
	pinMode(blue_pin, OUTPUT);
#endif
}

void Light::log() {
	char buf[100];
	char level[6], hue[6], sat[6];
	dtostrf(_level, 4, 2, level);
	if (_is_color_capable) {
		dtostrf(_hue, 4, 2, hue);
		dtostrf(_saturation, 4, 2, sat);
		sprintf(buf, "en:%d lev:%s hue:%s sat:%s", _enabled, level, hue, sat);
	} else
		 sprintf(buf, "en:%d lev:%s", _enabled, level);
		Serial.println(buf);
}

void Light::enable() {
	_actionTime = millis();
	_enabled = true;
}

void Light::disable() {
	_actionTime = millis();
	_enabled = false;
}

void Light::toggle() {
	if (_enabled)
		disable();
	else
		enable();
}

void Light::increaseLevel() {
	_level += 1.0 / LEVEL_STEPS;
	if (_level > 1)
		_level = 1;
}

void Light::decreaseLevel() {
	_level -= 1.0 / LEVEL_STEPS;
	if (_level < 0)
		_level = 0;
}

void Light::increaseHue() {
	_hue += 360. / HUE_STEPS;
	if (_hue >= 360)
		_hue -= 360;
}

void Light::decreaseHue() {
	_hue -= 360. / HUE_STEPS;
	if (_hue < 0)
		_hue += 360;
}

void Light::increaseSaturation() {
	_saturation += 1.0 / SATURATION_STEPS;
	if (_saturation > 1)
		_saturation = 1;
}

void Light::decreaseSaturation() {
	_saturation -= 1.0 / SATURATION_STEPS;
	if (_saturation < 0)
		_saturation = 0;
}

void Light::setLevel(double level) {
	_level = level;
}

void Light::setColor(double hue, double saturation) {
	_hue = hue;
	_saturation = saturation;
}

int correct(double value) {
	return int(pow(pow(255, 0.33) * ((float)(value * 100)/(float)100), 1/0.33) + 0.5); 
}

#if defined(ARDUINO_ARCH_ESP32)
void analogWrite(uint8_t channel, uint32_t value) {
	uint32_t duty = (4095 / 255.) * min(value, (uint32_t)255);
	ledcWrite(channel, duty);
}
#endif

void Light::loop() {
	long now = millis();
	double alpha;
	if (!_actionTime)
		return;
	if (_enabled) {
		if (now - _actionTime < FADEIN_DURATION)
			alpha = (double)(now - _actionTime) / FADEIN_DURATION;
		else
			alpha = 1;
	} else {
		if (now - _actionTime < FADEOUT_DURATION)
			alpha = 1 - (double)(now - _actionTime) / FADEOUT_DURATION;
		else
			alpha = 0;
	}
	if (_is_color_capable) {
		RGB rgb = hsv2rgb(_hue, _saturation, _level * alpha);
#if defined(ARDUINO_ARCH_ESP32)
		analogWrite(_red_channel, correct(rgb.r));
		analogWrite(_green_channel, correct(rgb.g));
		analogWrite(_blue_channel, correct(rgb.b));
#else
		analogWrite(_red_pin, correct(rgb.r));
		analogWrite(_green_pin, correct(rgb.g));
		analogWrite(_blue_pin, correct(rgb.b));
#endif
	} else {
#if defined(ARDUINO_ARCH_ESP32)
		analogWrite(_channel, correct(_level * alpha));
#else
		analogWrite(_pin, correct(_level * alpha));
#endif
	}
}
