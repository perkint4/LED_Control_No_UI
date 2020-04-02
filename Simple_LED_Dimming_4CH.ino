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

// You need to set these to the pins you will use PWM supported on 3, 5, 6, 9, 10 & 11
const int const_fan_pin   = 4;
const int const_blue_pin  = 5;
const int const_white_pin = 6;
const int const_green_pin = 9;
const int const_red_pin   = 10;

unsigned int current_time = 0;
byte fade_duration = 0;
byte blue_target_PWM  = 0;
byte blue_actual_PWM  = 0;
byte white_target_PWM = 0;
byte white_actual_PWM = 0;
byte green_target_PWM = 0;
byte green_actual_PWM = 0;
byte red_target_PWM   = 0;
byte red_actual_PWM   = 0;
byte now_HH = 0;
byte now_MM = 0;
byte now_SS = 0;

void setup() {
    // don't need serial unless you have it hooked to your PC
    Serial.begin(38400);
    Wire.begin();
    delay(5000);
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
    pinMode(const_green_pin, OUTPUT);
    pinMode(const_red_pin, OUTPUT);
    pinMode(const_fan_pin, OUTPUT);
    now_HH = hour();
    now_MM = minute();
    now_SS = second();
    Serial.print("Time is ");
    Serial.print(now_HH);
    Serial.print(":");
    if (now_MM < 10) {
        Serial.print("0");
    }
    Serial.print(now_MM);
    Serial.print(":");
    if (now_SS < 10) {
        Serial.print("0");
    }
    Serial.println(now_SS);
}
void loop() {
    // Convert the time into a usable number - 8:15 am is 815, ten thirty pm is 2230
    current_time = (hour() * 100) + minute();
    if (current_time >= 2130) {
        blue_target_PWM  = 1;
        white_target_PWM = 0;
        green_target_PWM = 0;
        red_target_PWM   = 0;
        fade_duration = 30;
    }  else if (current_time >= 2115) {
        blue_target_PWM  = 128;
        white_target_PWM = 0;
        green_target_PWM = 0;
        red_target_PWM   = 0;
        fade_duration = 15;
    } else if (current_time >= 1945) {
        blue_target_PWM  = 255;
        white_target_PWM = 128;
        green_target_PWM = 128;
        red_target_PWM   = 128;
        fade_duration = 30;
    } else if (current_time >= 1100) {
        blue_target_PWM  = 255;
        white_target_PWM = 255;
        green_target_PWM = 255;
        red_target_PWM   = 255;
        fade_duration = 30;
    } else if (current_time >= 930) {
        blue_target_PWM  = 128;
        white_target_PWM = 128;
        green_target_PWM = 128;
        red_target_PWM   = 128;
        fade_duration = 30;
    } else if (current_time >= 900) {
        blue_target_PWM  = 128;
        white_target_PWM = 0;
        green_target_PWM = 0;
        red_target_PWM   = 0;
        fade_duration = 30;
    }
    if ((blue_actual_PWM  != blue_target_PWM)  ||
        (white_actual_PWM != white_target_PWM) ||
        (green_actual_PWM != green_target_PWM) ||
        (red_actual_PWM   != red_target_PWM)) {
        fade_LEDs();
    }
}
void fade_LEDs() {
    unsigned long wait_time;
    unsigned long fade_start_millis;
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    int maxDiff = 0;
    // which colour has further to fade in the time?
    i = blue_actual_PWM  - blue_target_PWM;
    j = white_actual_PWM - white_target_PWM;
    k = green_actual_PWM - green_target_PWM;
    l = red_actual_PWM   - red_target_PWM;
    // if the lights are dimming, the value of i/j/k/l will be negative - just get the number & ignore the sign
    i = abs(i);
    j = abs(j);
    k = abs(k);
    l = abs(l);
    Serial.print("i=");
    Serial.print(i);
    Serial.print(" j=");
    Serial.print(j);
    Serial.print(" k=");
    Serial.print(k);
    Serial.print(" l=");
    Serial.println(l);
    // convert fade_duration into milliseconds
    maxDiff = max(i,j);
    maxDiff = max(maxDiff,j);
    maxDiff = max(maxDiff,k);
    maxDiff = max(maxDiff,l);
    wait_time = (fade_duration * 60000) / maxDiff;
    fade_start_millis = millis();
    Serial.print("Wait time=");
    Serial.println(wait_time);
    while ((blue_actual_PWM  != blue_target_PWM)  ||
           (white_actual_PWM != white_target_PWM) ||
           (green_actual_PWM != green_target_PWM) ||
           (red_actual_PWM   != red_target_PWM)) {
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
            if (green_target_PWM > green_actual_PWM) {
                green_actual_PWM += 1;
            } else if (green_target_PWM < green_actual_PWM) {
                green_actual_PWM -= 1;
            }
            if (red_target_PWM > red_actual_PWM) {
                red_actual_PWM += 1;
            } else if (red_target_PWM < red_actual_PWM) {
                red_actual_PWM -= 1;
            }
            analogWrite(const_blue_pin, blue_actual_PWM);
            analogWrite(const_white_pin, white_actual_PWM);
            analogWrite(const_green_pin, green_actual_PWM);
            analogWrite(const_red_pin, red_actual_PWM);
            fade_start_millis = millis();
            now_HH = hour();
            now_MM = minute();
            now_SS = second();
            Serial.print("Time is ");
            Serial.print(now_HH);
            Serial.print(":");
            if (now_MM < 10) {
                Serial.print("0");
            }
            Serial.print(now_MM);
            Serial.print(":");
            if (now_SS < 10) {
                Serial.print("0");
            }
            Serial.print(now_SS);
            Serial.print(" Blue actual ");
            Serial.print(blue_actual_PWM);
            Serial.print(" White actual ");
            Serial.print(white_actual_PWM);
            Serial.print(" Green actual ");
            Serial.print(green_actual_PWM);
            Serial.print(" Red actual ");
            Serial.println(red_actual_PWM);
            }
        // 255 is 25% of all 4 lights on full so if the total exceeds this turn fans on
        if (int(blue_actual_PWM + white_actual_PWM + green_actual_PWM + red_actual_PWM) >= 255) {
            digitalWrite(const_fan_pin, HIGH);
        } else {
            digitalWrite(const_fan_pin, LOW);
        }

    }
}
