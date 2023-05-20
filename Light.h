#include <Arduino.h>
#include <math.h>

struct RGB {
	double r; // ∈ [0, 1]
	double g; // ∈ [0, 1]
	double b; // ∈ [0, 1]
};

struct RGB hsv2rgb(double h, double s, double v);

class Light {
	public:
#if defined(ARDUINO_ARCH_ESP32)
		Light(int pin, int channel);
		Light(int red_pin, int green_pin, int blue_pin, int red_channel, int blue_channel, int green_channel);
#else
		Light(int pin);
		Light(int red_pin, int green_pin, int blue_pin);
#endif
		void enable();
		void disable();
		void toggle();
		void increaseLevel();
		void decreaseLevel();
		void setLevel(double level);
		void setColor(double hue, double saturation);//, double brightness);
		void increaseHue();
		void decreaseHue();
		void increaseSaturation();
		void decreaseSaturation();
		void loop();
		void log();
	private:
		bool _is_color_capable = false;
		int _pin;
		int _red_pin, _green_pin, _blue_pin;
#if defined(ARDUINO_ARCH_ESP32)
		int _channel;
		int _red_channel, _green_channel, _blue_channel;
#endif
		bool _enabled = false;
		double _level = 1;
		double _hue = 0;
		double _saturation = 1;
		long _actionTime;
};
