#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"

void setDegree(int servoPin, float degree); //Asignar angulo a servo
void setServo(int servoPin, float startDegree); //Inicializar servo
void setPeripherals(); //Inicialializar sistema y perofericos
void setVela(); //Controlar vela
void setTimon(); //Controlar timon

//Variables Funciones del Servo
const float clockDiv = 64;
float wrap = 39062;
const int degree = 0;

//Modo operación (automático, control remoto)
const int opModePin = 10;
bool opMode = true;

//Servomotor 1
const int servoPin1 = 0;
const int timonPin = 28;
uint16_t potenciometro;
float grados1;

//Servomotor 2
const int servoPin2 = 1;
const int rightPin = 12;
const int leftPin = 15;
float grados2 = 90;
bool rightButton = false;
bool leftButton = false;

//LED Interno
const uint LED_PIN = PICO_DEFAULT_LED_PIN;

//Factor de converion del adc
const float conversion_factor = 3.3f / (1 << 12);

int main()
{
    setPeripherals(); 
    while (true)
    {
        opMode = gpio_get(opModePin);
        if(opMode){
            gpio_put(LED_PIN, 0);
            setVela();
            setTimon();
            sleep_ms(50);
        }
        else{
            gpio_put(LED_PIN, 1);
        }
    }
}

//INICIALIZAR
void setPeripherals(){
    //SYS_INIT
    adc_init();
    stdio_init_all();

    //Modo de operación
    gpio_init(opModePin);
    gpio_set_dir(opModePin, GPIO_IN);

    //LED Interno
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    //Potenciometro
    adc_gpio_init(timonPin);
    adc_select_input(2);
    setServo(servoPin1, degree);
    setServo(servoPin2, degree);

    //Pulsadores
    gpio_init(rightPin);
    gpio_init(leftPin);
    gpio_set_dir(rightPin, GPIO_IN);
    gpio_set_dir(leftPin, GPIO_IN); 
}

//CONTROLAR MOTORES
void setVela(){
    potenciometro = adc_read();
    // printf("Raw value: 0x%03x, voltage: %f V\n", potenciometro, potenciometro * conversion_factor);
    grados1 = (potenciometro * conversion_factor) * (55);
    // printf("Grados: %f °\n", grados1);
    setDegree(servoPin1, grados1);
}
void setTimon(){
    rightButton = gpio_get(rightPin);
    leftButton = gpio_get(leftPin);
    if(rightButton & (grados2<=175)){
        // printf("Derecha \n");
        grados2 += 5;
    }
    else if(leftButton & (grados2>=5)){
        // printf("Izquierda \n");
        grados2 -= 5;
    }
    else if(grados2>=95){
        grados2 -= 5;
    }
    else if(grados2<=85){
        grados2 += 5;
    }
    setDegree(servoPin2, grados2);
}

//FUNCIONES DEL SERVO
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