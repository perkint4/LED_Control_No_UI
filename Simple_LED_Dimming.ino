// Very simple sketch to fade two LED channels on and off at hard coded times
// Ensure the RTC time is already set - use the Arduino example:
// File\Examples\RTClib\ds1307
// or
// File\Examples\RTClib\ds3231
// By Tim Perkins
// Support thread:http://www.ultimatereef.net/threads/need-easy-simple-two-channel-arduino-led-controller.816975

#include <Wire.h>
#include <TimeLib.h>
#include <DS1307RTC.h>

// You need to set these to the pins you will use
const int const_blue_pin  = 5;
const int const_white_pin = 6;
const int const_fan_pin   = 7;

unsigned int current_time = 0;
byte fade_duration = 0;
byte blue_target_PWM = 0;
byte blue_actual_PWM = 0;
byte white_target_PWM = 0;
byte white_actual_PWM = 0;
byte now_HH = 0;
byte now_MM = 0;
byte now_SS = 0;

void setup() {
    // don't need serial unless you have it hooked to your PC
    // Serial.begin(38400);
    Wire.begin();
    delay(10000);
    // Time the Time function to use the RTC
    setSyncProvider(RTC.get);
    // Time the Time function to sync with the RTC every minute
    setSyncInterval(60);
    // This can be useful for checking if things aren't working!
    // if (timeStatus() != timeSet) {
    //     Serial.println("Unable to sync with the RTC");
    // } else {
    //     Serial.println("RTC has set the system time");
    // }
    pinMode(const_blue_pin, OUTPUT);
    pinMode(const_white_pin, OUTPUT);
    pinMode(const_fan_pin, OUTPUT);
}
void loop() {
    // Convert the time into a usable number - 8:15 am is 815, ten thirty pm is 2230
    current_time = (hour() * 100) + minute();

    if (current_time >= 2130) {
        blue_target_PWM = 1;
        white_target_PWM = 0;
        fade_duration = 30;
    }  else if (current_time >= 2115) {
        blue_target_PWM = 128;
        white_target_PWM = 0;
        fade_duration = 15;
    } else if (current_time >= 1945) {
        blue_target_PWM = 255;
        white_target_PWM = 128;
        fade_duration = 30;
    } else if (current_time >= 1100) {
        blue_target_PWM = 255;
        white_target_PWM = 255;
        fade_duration = 30;
    } else if (current_time >= 930) {
        blue_target_PWM = 128;
        white_target_PWM = 128;
        fade_duration = 30;
    } else if (current_time >= 900) {
        blue_target_PWM = 128;
        white_target_PWM = 0;
        fade_duration = 30;
    }
    if ((blue_actual_PWM != blue_target_PWM) ||
        (white_actual_PWM != white_target_PWM)) {
        fade_LEDs();
    }
}
void fade_LEDs() {
    unsigned long wait_time;
    unsigned long fade_start_millis;
    byte i;
    byte j;
    // which colour has further to fade in the time?
    i = blue_actual_PWM - blue_target_PWM;
    j = white_actual_PWM - white_target_PWM;
    // convert fade_duration into milliseconds
    if (i > j) {
        wait_time = (fade_duration * 60000) / i;
    } else {
        wait_time = (fade_duration * 60000) / j;
    }
    fade_start_millis = millis();
    while ((blue_actual_PWM != blue_target_PWM) ||
           (white_actual_PWM != white_target_PWM)) {
        if (wait_time <= (millis() - fade_start_millis)) {
            if (blue_target_PWM > blue_actual_PWM) {
                blue_actual_PWM += 1;
            } else if (blue_target_PWM < blue_actual_PWM) {
                blue_actual_PWM -= 1;
            }
            if (white_target_PWM > white_actual_PWM) {
                white_actual_PWM += 1;
            } else if (white_target_PWM < white_actual_PWM) {
                white_actual_PWM -= 1;
            }
            digitalWrite(const_blue_pin, blue_actual_PWM);
            digitalWrite(const_white_pin, white_actual_PWM);
            fade_start_millis = millis();
        }
        // 128 is 25% of both lights on full so if the total exceeds this turn fans on
        if (int(blue_actual_PWM + white_actual_PWM) >= 128) {
            digitalWrite(const_fan_pin, HIGH);
        } else {
            digitalWrite(const_fan_pin, LOW);
        }
    }
}
