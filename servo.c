#include "pico/stdlib.h"
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/clocks.h"

const float clockDiv = 64;
float wrap = 39062;

void setDegree(int servoPin, float degree)
{
    float millis;
    millis = ((100/9)*degree) + 400;
    pwm_set_gpio_level(servoPin, (millis/20000.f)*wrap);
}

void setServo(int servoPin, float startDegree)
{
    gpio_set_function(servoPin, GPIO_FUNC_PWM);

    uint slice_num = pwm_gpio_to_slice_num(servoPin);

    pwm_config config = pwm_get_default_config();
    
    uint64_t clockspeed = clock_get_hz(5); //Get the current frequency of the specified clock

    wrap = clockspeed/clockDiv/50;

    pwm_config_set_clkdiv(&config, clockDiv);
    pwm_config_set_wrap(&config, wrap);
    pwm_init(slice_num, &config, true);

    float startMillis;
    startMillis = ((100/9)*startDegree) + 400;
    setDegree(servoPin, startMillis);
}