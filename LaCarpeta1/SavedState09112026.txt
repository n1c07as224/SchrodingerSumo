/*

>>--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--<<
|                    Schrodinger Sumo                   |
>>--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--*--<<
______________________________
|____________________________|
 |                          |
 |        /\__/\            |
 |       /`    '\           |
 |      == x  0 ==          |
 |       \  --  /           |
 |      /        \          |
 |     /          \         |
 |    |            |        |
 |     \  ||  ||  /         |
 |      \_oo__oo_/#######o  |
 |__________________________|

Autores:
Nicolas Arredondo Jaime
Christopher Guijosa Vaca
Leonardo Zavala Angeles

Referencias:
1-Robojax, "Using Four VL53L0X Laser Distance Meters with Arduino", https://robojax.com/tutorial_view.php?lang=en&id=301#code-section-direct
2-GeeksforGeeks, "Understanding constexpr Specifier in C++", https://www.geeksforgeeks.org/cpp/understanding-constexper-specifier-in-cpp/
3-Descubrearduino, "Cómo hacer un PID con Arduino", https://descubrearduino.com/como-hacer-un-pid-con-arduino/
4-Del Código a la Arquitectura, "Máquinas de Estado: El patrón para eliminar los "If" anidados", https://youtu.be/sTyRxey86Jw?si=h-pryMeM7pt8JGrz 
5-W3schools, "C++ Enumeration (enum)", https://www.w3schools.com/cpp/cpp_enum.asp
6-Pkolt, "pkolt/vl53l0x README", https://components.espressif.com/components/pkolt/vl53l0x/versions/1.0.0/readme#api
7- CPP Reference, "C++ Reference", https://cppreference.com/
8- https://www.aleksandrhovhannisyan.com/blog/implementing-a-finite-state-machine-in-cpp/#what-is-a-finite-state-machine

Cosas a tomar en cuenta:
- Los GPIO estan declarados para que el codigo sea ejecutado en un esp32s3
- Todas las medidas usan el sistema internacional de unidades
- Todas las funciones y declaraciones encargadas del control del harware se encuentran en el directorio /Hardware


*/

// definir conversion de encoders y conexcion
//implementar la lectura de la velocidad de los encoders y la conversion a RPM


//------------------------------------------SETUP------------------------------------------

//----------------------------Librerias----------------------------
#include <cmath>//
#include "esp_timer.h"//

#include <stdio.h>
#include <cstdint>
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "Hardware/motor.h"
//#include "Hardware/distance_sensor.h"
//#include "Hardware/line_sensor.h"
//#include "Comunication/i2c.h"

//---------------------Propiedades del robot---------------------
// Medidas fisicas del robot
constexpr float r_llantas = 0.030f; // m
constexpr float wheelBase = 0.180f; // m    MAY CHANGE I DONT KNOW

// Constantes y variables matematicas
struct VectorDirection {
    float v; //Speed  m/s
    float w; //Angle Rad/s
};

struct WheelRPM{
    float left;
    float right;
};

VectorDirection target = {0.0f, 0.0f};   //0

struct PID {
    float Kp;
    float Ki;
    float Kd;
    float previous_error;
    float previous_measurement;
    float integral;
};
 
PID izq = {0.30f, 1.00f, 0.00f, 0.0f, 0.0f, 0.0f};
PID der = {0.30f, 1.00f, 0.00f, 0.0f, 0.0f, 0.0f};
 
void PID_reset(PID &pid) {
    pid.previous_error = 0.0f;
    pid.previous_measurement = 0.0f;
    pid.integral = 0.0f;
}
 
// error = set_point - medicion (ambos en RPM). Devuelve correccion en PWM, limitada a +-out_limit.



// Frecuencia en la lectura de sensores
constexpr uint16_t dDS = 150;
constexpr uint16_t dDS_fast = 50;
constexpr uint16_t dIMU = 150;
constexpr uint16_t dIS = 150;

constexpr uint32_t loop_time = 10;   

constexpr float PWM_MAX            = 255.0f;   
constexpr float MAX_WHEEL_RPM      = 450.0f;   
constexpr float K_FF               = PWM_MAX / MAX_WHEEL_RPM; 
constexpr float RPM_LIMIT_CMD      = MAX_WHEEL_RPM * 0.9f;    
constexpr float RPM_FILTER_ALPHA   = 0.3f;     
 
// Relacion entre el eje del encoder y la rueda 
constexpr float GEAR_REDUCTION = 32.0f;
constexpr float ENC_TO_WHEEL_RATIO = 1.0f/GEAR_REDUCTION;
// Ajustar en pruebas
constexpr float ENC_SIGN_L         = 1.0f;
constexpr float ENC_SIGN_R         = -1.0f;
 
// Limites de aceleracion del vector
constexpr float V_ACCEL_MAX        = 4.0f;     
constexpr float W_ACCEL_MAX        = 30.0f;    
 
constexpr float W_MAX              = 6.0f;     
// Limitar dento de un rango 
static inline float clampf(float x, float lo, float hi) {
    return x < lo ? lo : (x > hi ? hi : x);
}
WheelRPM calculateWheelRPM(VectorDirection vel, float wheelRadius, float wheelBase) {
    const float k = 60.0f / (2.0f * (float)M_PI * wheelRadius);   
    WheelRPM out;
    out.left  = (vel.v - vel.w * wheelBase * 0.5f) * k;
    out.right = (vel.v + vel.w * wheelBase * 0.5f) * k;

    float maxAbs = fmaxf(fabsf(out.left), fabsf(out.right));
    if (maxAbs > RPM_LIMIT_CMD) {
        float s = RPM_LIMIT_CMD / maxAbs;
        out.left  *= s;
        out.right *= s;
    }
    return out;
}
VectorDirection rampVector(VectorDirection current, VectorDirection target, float dt) {
    current.v += clampf(target.v - current.v, -V_ACCEL_MAX * dt, V_ACCEL_MAX * dt);
    current.w += clampf(target.w - current.w, -W_ACCEL_MAX * dt, W_ACCEL_MAX * dt);
    return current;
}



//---------------------Estados y prioridades---------------------
enum State {

    IDLE,
    SEARCHING,
    ENEMY_FOUND,
    ENEMY_LOST_L,
    ENEMY_LOST_R,
    PUSHING,
    PUSHING_LOSING,
    LINE_FL,
    LINE_FR,
    LINE_RL,
    LINE_RR,
    CLOSE_TO_LINE,
    COUNT               //enumeration trick

};

// Prioridad que tiene cada estado
struct StateConfig {

    int state_num;
    int state_priority;

};

constexpr StateConfig robotStatePriority[COUNT] {

    {IDLE, 5},
    {SEARCHING, 1},
    {ENEMY_FOUND, 2},
    {ENEMY_LOST_L, 2},
    {ENEMY_LOST_R, 2},
    {PUSHING, 2},
    {PUSHING_LOSING, 3},
    {LINE_FL, 4},
    {LINE_FR, 4},
    {LINE_RL, 4},
    {LINE_RR, 4},
    {CLOSE_TO_LINE, 1}

};

// Historial de estados
State robotState = IDLE;
State lastRobotState = IDLE;
State beforeLastRobotState = IDLE;




//---------------------GPIOs---------------------
// Pines I2C
constexpr gpio_num_t i2c_sda_pin  = GPIO_NUM_8;
constexpr gpio_num_t i2c_scl_pin  = GPIO_NUM_9;
constexpr gpio_num_t i2c_sda2_pin = GPIO_NUM_4;
constexpr gpio_num_t i2c_scl2_pin = GPIO_NUM_5;

// Salidas digitales
constexpr gpio_num_t XSHUT_1 = GPIO_NUM_14;
constexpr gpio_num_t XSHUT_2 = GPIO_NUM_15;
constexpr gpio_num_t XSHUT_3 = GPIO_NUM_16;
constexpr gpio_num_t XSHUT_4 = GPIO_NUM_17;
constexpr gpio_num_t XSHUT_5 = GPIO_NUM_18;
constexpr gpio_num_t XSHUT_6 = GPIO_NUM_21;

// Entradas digitales
constexpr gpio_num_t LS_FL = GPIO_NUM_1;
constexpr gpio_num_t LS_FR = GPIO_NUM_2;
constexpr gpio_num_t LS_RL = GPIO_NUM_6;
constexpr gpio_num_t LS_RR = GPIO_NUM_7;

// Salidas PWM
constexpr gpio_num_t MTR1_L_PIN = GPIO_NUM_12;
constexpr gpio_num_t MTR1_R_PIN = GPIO_NUM_13;
constexpr gpio_num_t MTR2_L_PIN = GPIO_NUM_40;
constexpr gpio_num_t MTR2_R_PIN = GPIO_NUM_41;

// Canales PWM
constexpr ledc_channel_t MTR1_L = LEDC_CHANNEL_0;
constexpr ledc_channel_t MTR1_R = LEDC_CHANNEL_1;
constexpr ledc_channel_t MTR2_L = LEDC_CHANNEL_2;
constexpr ledc_channel_t MTR2_R = LEDC_CHANNEL_3;

// Entradas ADC
constexpr gpio_num_t IS1_L_PIN = GPIO_NUM_39;
constexpr gpio_num_t IS1_R_PIN = GPIO_NUM_38;
constexpr gpio_num_t IS2_L_PIN = GPIO_NUM_42;
constexpr gpio_num_t IS2_R_PIN = GPIO_NUM_47;

// Pines Encoders
constexpr gpio_num_t ENC1_PIN = GPIO_NUM_10;
constexpr gpio_num_t ENC2_PIN = GPIO_NUM_11;

// Pines RRTT
constexpr gpio_num_t RDY_PIN = GPIO_NUM_45;
constexpr gpio_num_t GO_PIN = GPIO_NUM_48;

// Canales ADC
static adc_oneshot_unit_handle_t adc1_handle = nullptr;
static adc_oneshot_unit_handle_t adc2_handle = nullptr;
constexpr adc_channel_t IS1_L = ADC_CHANNEL_0;
constexpr adc_channel_t IS1_R = ADC_CHANNEL_1;
constexpr adc_channel_t IS2_L = ADC_CHANNEL_2;
constexpr adc_channel_t IS2_R = ADC_CHANNEL_3;

// ADC Encoders
constexpr adc_channel_t ENC1_CH = ADC_CHANNEL_9;
constexpr adc_channel_t ENC2_CH = ADC_CHANNEL_0;

// Encoders I2C
static i2c_master_bus_handle_t i2c_bus_1 = nullptr;
static i2c_master_bus_handle_t i2c_bus_2 = nullptr;

static i2c_master_dev_handle_t as5600_left = nullptr;
static i2c_master_dev_handle_t as5600_right = nullptr;

constexpr uint8_t AS5600_ADDRESS = 0x36;
constexpr uint8_t AS5600_RAW_ANGLE_HIGH = 0x0C;
constexpr uint8_t AS5600_RAW_ANGLE_LOW = 0x0D;







//---------------------Inicializar funciones---------------------
void init_gpio_adc_pwm();
int Read_ENC1();
int Read_ENC2();

constexpr float MIN_PWM = 20.0f;   // Ajustar

static void pwm_write(ledc_channel_t ch, uint32_t duty) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, ch, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, ch);
}

// fwdCh = RPWM, revCh = LPWM
static void driver_write(ledc_channel_t fwdCh, ledc_channel_t revCh, float pwm) {
    pwm = clampf(pwm, -PWM_MAX, PWM_MAX);
    if (fabsf(pwm) < MIN_PWM) pwm = 0.0f;
    uint32_t duty = (uint32_t)fabsf(pwm);

    if (pwm >= 0.0f) { pwm_write(revCh, 0);    pwm_write(fwdCh, duty); }
    else             { pwm_write(fwdCh, 0);    pwm_write(revCh, duty); }
}

void motor_set(float izqPWM, float derPWM) {
    driver_write(MTR1_R, MTR1_L, izqPWM);   // izquierdo
    driver_write(MTR2_R, MTR2_L, derPWM);   // derecho
}

void updateMotors(float pwm_izq, float pwm_der) {
    motor_set(pwm_izq, pwm_der);
}


VectorDirection getStateMotor(State s){
    switch (s) {
        case IDLE:
            return {0.0f, 0.0f};
        case SEARCHING:
            return {0.5f, 0.0f};
        case ENEMY_FOUND:
            return {1.0f, 0.0f};
        case ENEMY_LOST_L:
            return {0.5f, 0.5f};
        case ENEMY_LOST_R:
            return {0.5f, -0.5f};
        case PUSHING:
            return {1.0f, 0.0f};    
        case PUSHING_LOSING:
            return {0.5f, 0.0f};
        case LINE_FL:
            return {0.0f, 0.5f};
        case LINE_FR:
            return {0.0f, -0.5f};
        case LINE_RL:
            return {0.0f, 0.5f};
        case LINE_RR:
            return {0.0f, -0.5f};
        case CLOSE_TO_LINE:
            return {0.0f, 0.0f};
        default:
            return {0.0f, 0.0f};
    }
}


float PID_calc(PID &pid, float set_point, float measured, float dtime, float out_limit) {
 
    if (dtime <= 0.0f) return 0.0f;     // evita division entre cero
 
    float error = set_point - measured;
 
    // Proporcional
    float p = pid.Kp * error;
 
    // Derivada sobre la medicion (evita "derivative kick" cuando cambia el set_point)
    float d = -pid.Kd * (measured - pid.previous_measurement) / dtime;
 
    // Integral tentativa
    float integral_candidate = pid.integral + error * dtime;
    float i = pid.Ki * integral_candidate;
 
    float unsat = p + i + d;
    float sat   = clampf(unsat, -out_limit, out_limit);
 
    // Anti-windup: solo acumula si no esta saturado, o si el error ayuda a salir de la saturacion
    bool saturated_high = unsat >  out_limit;
    bool saturated_low  = unsat < -out_limit;
    if ((!saturated_high && !saturated_low) ||
        (saturated_high && error < 0.0f) ||
        (saturated_low  && error > 0.0f)) {
        pid.integral = integral_candidate;
    }
 
    pid.previous_error = error;
    pid.previous_measurement = measured;
 
    return sat;
}


void init_as5600();
// Lectura de angulo de los encoders 
uint16_t read_as5600_raw_angle(
    i2c_master_dev_handle_t device
)
{
    uint8_t register_address =
        AS5600_RAW_ANGLE_HIGH;

    uint8_t data[2] = {0, 0};

    esp_err_t result =
        i2c_master_transmit_receive(
            device,
            &register_address,
            1,
            data,
            2,
            100
        );

    if (result != ESP_OK) {
        printf(
            "Error leyendo AS5600: %s\n",
            esp_err_to_name(result)
        );

        return 0;
    }

    uint16_t raw_angle =
        (static_cast<uint16_t>(data[0]) << 8) |
        static_cast<uint16_t>(data[1]);

    return raw_angle & 0x0FFF;
}
// Conversion de angulo a grados
float read_as5600_angle_degrees(
    i2c_master_dev_handle_t device
)
{
    uint16_t raw_angle =
        read_as5600_raw_angle(device);

    return (
        static_cast<float>(raw_angle) *
        360.0f /
        4096.0f
    );
}
// Funciones individuales
float Read_ANGLE1()
{
    return read_as5600_angle_degrees(
        as5600_left
    );
}

float Read_ANGLE2()
{
    return read_as5600_angle_degrees(
        as5600_right
    );
}

//Lectura y acumulacion de encoders
static bool as5600_read_raw(i2c_master_dev_handle_t device, uint16_t &out) {
    uint8_t reg = AS5600_RAW_ANGLE_HIGH;
    uint8_t data[2] = {0, 0};
    if (i2c_master_transmit_receive(device, &reg, 1, data, 2, 5) != ESP_OK) {
        return false;
    }
    out = ((static_cast<uint16_t>(data[0]) << 8) | data[1]) & 0x0FFF;
    return true;
}

struct EncAccum
{
    volatile int32_t count = 0;
    uint16_t last = 0;
    bool init = false;
};
EncAccum accL, accR;

static void accumulate(EncAccum &e, uint16_t raw){
    if (!e.init) {
        e.last = raw;
        e.init = true;
        return;
    }
    int16_t delta = (int16_t)(raw - e.last);
    if (delta > 2048) delta -= 4096;
    else if (delta < -2048) delta += 4096;
    e.last = raw;
    e.count += delta;
}

// Task 1kHz
static void enc_timer_cb(void *){
        uint16_t raw;
    if (as5600_read_raw(as5600_left, raw)) {
        accumulate(accL, raw);
    }
    if (as5600_read_raw(as5600_right, raw)) {
        accumulate(accR, raw);
    }
}
static void start_enc_sample(){
    esp_timer_create_args_t args = {};
    args.callback = enc_timer_cb;
    args.name = "enc_timer";
    esp_timer_handle_t timer;
    ESP_ERROR_CHECK(esp_timer_create(&args, &timer));
    ESP_ERROR_CHECK(esp_timer_start_periodic(timer, 1000));
}

// RPM de rueda
struct WheelRPMState{
    int32_t prev = 0;
    float RPM = 0.0f;
};

WheelRPMState rsL, rsR;

float wheel_rpm(EncAccum &e, WheelRPMState &s, float sign, float dt){
    if(dt <= 0.0f) return s.RPM;
    int32_t current = e.count;
    int32_t delta = current - s.prev;
    s.prev = current;
    float rpm_raw = (delta / 4096.0f)/ dt * 60.0f * sign * ENC_TO_WHEEL_RATIO;
    s.RPM += RPM_FILTER_ALPHA * (rpm_raw - s.RPM);
    return s.RPM;
}




//------------------------------------------FUNCTIONS------------------------------------------

//---------------------Configurar GPIOs---------------------
void init_gpio_adc_pwm() {

    // Salidas digitales
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_1, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_2, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_3, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_4, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_5, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(XSHUT_6, GPIO_MODE_OUTPUT));

    // Apagar VL53L0X
    ESP_ERROR_CHECK(gpio_set_level(XSHUT_1, 0));
    ESP_ERROR_CHECK(gpio_set_level(XSHUT_2, 0));
    ESP_ERROR_CHECK(gpio_set_level(XSHUT_3, 0));
    ESP_ERROR_CHECK(gpio_set_level(XSHUT_4, 0));
    ESP_ERROR_CHECK(gpio_set_level(XSHUT_5, 0));
    ESP_ERROR_CHECK(gpio_set_level(XSHUT_6, 0));

    // Entradas digitales
    ESP_ERROR_CHECK(gpio_set_direction(LS_FL, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LS_FR, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LS_RL, GPIO_MODE_INPUT));
    ESP_ERROR_CHECK(gpio_set_direction(LS_RR, GPIO_MODE_INPUT));

    // Salidas PWM
    ledc_channel_config_t MTR1_L_Config{};
    MTR1_L_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR1_L_Config.channel = MTR1_L;
    MTR1_L_Config.timer_sel = LEDC_TIMER_0;
    MTR1_L_Config.gpio_num = MTR1_L_PIN;
    MTR1_L_Config.duty = 0;
    MTR1_L_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR1_L_Config));


    ledc_channel_config_t MTR1_R_Config{};
    MTR1_R_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR1_R_Config.channel = MTR1_R;
    MTR1_R_Config.timer_sel = LEDC_TIMER_0;
    MTR1_R_Config.gpio_num = MTR1_R_PIN;
    MTR1_R_Config.duty = 0;
    MTR1_R_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR1_R_Config));


    ledc_channel_config_t MTR2_L_Config{};
    MTR2_L_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR2_L_Config.channel = MTR2_L;
    MTR2_L_Config.timer_sel = LEDC_TIMER_0;
    MTR2_L_Config.gpio_num = MTR2_L_PIN;
    MTR2_L_Config.duty = 0;
    MTR2_L_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR2_L_Config));


    ledc_channel_config_t MTR2_R_Config{};
    MTR2_R_Config.speed_mode = LEDC_LOW_SPEED_MODE;
    MTR2_R_Config.channel = MTR2_R;
    MTR2_R_Config.timer_sel = LEDC_TIMER_0;
    MTR2_R_Config.gpio_num = MTR2_R_PIN;
    MTR2_R_Config.duty = 0;
    MTR2_R_Config.hpoint = 0;

    ESP_ERROR_CHECK(ledc_channel_config(&MTR2_R_Config));

    //RRTT     Creo que algo va mal con esto, no se si es necesario
    ESP_ERROR_CHECK(gpio_set_direction(RDY_PIN, GPIO_MODE_OUTPUT));
    ESP_ERROR_CHECK(gpio_set_direction(GO_PIN, GPIO_MODE_INPUT));

    ESP_ERROR_CHECK(gpio_set_pull_mode(GO_PIN, GPIO_PULLDOWN_ONLY)); // Configura la resistencia pull-down para el pin GO_PIN
    // Entradas ADC
    adc_oneshot_chan_cfg_t adc_channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    /*ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS1_L, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS1_R, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS2_L, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, IS2_R, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc1_handle, ENC1_CH, &adc_channel_config));
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc2_handle, ENC2_CH, &adc_channel_config));*/

}
void init_as5600()
{
    i2c_device_config_t as5600_config = {};

    as5600_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    as5600_config.device_address = 0x36;
    as5600_config.scl_speed_hz = 400000;

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            i2c_bus_1,
            &as5600_config,
            &as5600_left
        )
    );

    ESP_ERROR_CHECK(
        i2c_master_bus_add_device(
            i2c_bus_2,
            &as5600_config,
            &as5600_right
        )
    );
}

//Encoders ADC
int Read_ENC1() {
    int adc_reading = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc1_handle, ENC1_CH, &adc_reading));
    return adc_reading;
}

int Read_ENC2() {
    int adc_reading = 0;
    ESP_ERROR_CHECK(adc_oneshot_read(adc2_handle, ENC2_CH, &adc_reading));
    return adc_reading;
}
//Encoders I2C





class SumoRobotClass {
    private: 
        State currentstates;
        //buscar la priodidad de cada estado 
        int getPriority (State state) {
            for (int i = 0; i < COUNT; ++i) {
                if (robotStatePriority[i].state_num == state) {
                    return robotStatePriority[i].state_priority;
                }
            }
            return -1; // Estado no encontrado
        }
    public:
        SumoRobotClass() : currentstates(IDLE) {}

        //Cambia de estado segun su prioridad
        void changeState(State newState) {
            
            // Si el nuevo estado es IDLE, siempre se permite el cambio
            if (newState == IDLE) {
                beforeLastRobotState = lastRobotState;
                lastRobotState = currentstates;
                currentstates = newState;
                return;
            }
            if (currentstates == IDLE && newState != IDLE) {
                beforeLastRobotState = lastRobotState;
                lastRobotState = currentstates;
                currentstates = newState;
                return;
            } 

            int currentPriority = getPriority(currentstates);
            int newPriority = getPriority(newState);

            if (newPriority >= currentPriority) {
                beforeLastRobotState = lastRobotState;
                lastRobotState = currentstates;
                currentstates = newState;
            }
            
        }
        //actualiza el estado del robot
        void updateState() {
            switch(currentstates){
                case State::IDLE:
                    // Implement logic for IDLE state
                    break;
                case State::SEARCHING:
                    // Implement logic for SEARCHING state
                    break;
                case State::ENEMY_FOUND:
                    // Implement logic for ENEMY_FOUND state
                    break;
                case State::ENEMY_LOST_L:   //should it be fused with Lost R?
                    // Implement logic for ENEMY_LOST_L state  
                    break;
                case State::ENEMY_LOST_R:
                    // Implement logic for ENEMY_LOST_R state
                    break;
                case State::PUSHING:
                    // Implement logic for PUSHING state
                    break;
                case State::PUSHING_LOSING:
                    // Implement logic for PUSHING_LOSING state
                    break;
                case State::LINE_FL:  //fuse them?
                    // Implement logic for LINE_FL state
                    break;
                case State::LINE_FR:
                    // Implement logic for LINE_FR state
                    break;
                case State::LINE_RL:
                    // Implement logic for LINE_RL state
                    break; 
                case State::LINE_RR:
                    // Implement logic for LINE_RR state
                    break;
                default:
                    // Handle unexpected state
                    break;
            }
        }
        State getState() const {
            return currentstates;
        }
};
/*void execute_robotStateActions(State robotState) {

    switch (robotState) {
    case IDLE:
        // code 

        break;

    case SEARCHING:
    

        break;

    case ENEMY_FOUND:
    

        break;

    case ENEMY_LOST_L:
    case ENEMY_LOST_R:
    

        break;

    case PUSHING:
    

        break;

    case PUSHING_LOSING:
    

        break;

    case LINE_FL:
    case LINE_FR:
    case LINE_RL:
    case LINE_RR:
    

        break;

    case CLOSE_TO_LINE:
    

        break;

    default:
    
    
        break;
    }
}
float PID_calc(PID &pid, int set_point, int rpm_read, float dtime) {

            float error = set_point - rpm_read;
            //integral
            pid.integral += error * dtime;

            // Derivada
            float deriv_error = (error - pid.previous_error) / dtime;
        

            float result = pid.Kp * error + pid.Ki * pid.integral + pid.Kd * deriv_error;

            pid.previous_error = error;
            pid.previous_time = pid.current_time;

            return result;
        }*/

SumoRobotClass robotSumo;
//=======================================================MAIN-START=======================================================
extern "C" void app_main(void)
{

    //---------------------Configurar protocolos de comunicacion---------------------

    // I2C
    // SDA 8 y SCL 9
    i2c_master_bus_config_t i2c_bus_config1 = {};
    i2c_bus_config1.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_config1.i2c_port = I2C_NUM_0;
    i2c_bus_config1.scl_io_num = i2c_scl_pin;
    i2c_bus_config1.sda_io_num = i2c_sda_pin;
    i2c_bus_config1.glitch_ignore_cnt = 7;
    i2c_bus_config1.flags.enable_internal_pullup = true;

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_config1,&i2c_bus_1));


    // SDA 4 y SCL 5
    i2c_master_bus_config_t i2c_bus_config_2 = {};

    i2c_bus_config_2.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_bus_config_2.i2c_port = I2C_NUM_1;
    i2c_bus_config_2.sda_io_num = i2c_sda2_pin;
    i2c_bus_config_2.scl_io_num = i2c_scl2_pin;
    i2c_bus_config_2.glitch_ignore_cnt = 7;
    i2c_bus_config_2.flags.enable_internal_pullup = true;

    ESP_ERROR_CHECK(
        i2c_new_master_bus(
            &i2c_bus_config_2,
            &i2c_bus_2
        )
    );

    

    // PWM timer
    ledc_timer_config_t timer_config{};
    timer_config.speed_mode = LEDC_LOW_SPEED_MODE;
    timer_config.timer_num = LEDC_TIMER_0;
    timer_config.duty_resolution = LEDC_TIMER_8_BIT;
    timer_config.freq_hz = 5000;
    timer_config.clk_cfg = LEDC_AUTO_CLK;

    ESP_ERROR_CHECK(ledc_timer_config(&timer_config));

    // ADC
    adc_oneshot_unit_init_cfg_t adc1_init_config = {};
    adc1_init_config.unit_id = ADC_UNIT_1;
    adc1_init_config.clk_src = ADC_RTC_CLK_SRC_DEFAULT;
    adc1_init_config.ulp_mode = ADC_ULP_MODE_DISABLE;

    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc1_init_config, &adc1_handle));

    adc_oneshot_unit_init_cfg_t adc2_init_config = {};
    adc2_init_config.unit_id = ADC_UNIT_2;
    adc2_init_config.clk_src = ADC_RTC_CLK_SRC_DEFAULT;
    adc2_init_config.ulp_mode = ADC_ULP_MODE_DISABLE;
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&adc2_init_config, &adc2_handle));

    //---------------------Inicializar sensores y motores---------------------
    init_as5600();

    init_gpio_adc_pwm();

    start_enc_sample();
    
    //DistanceS:: initAll();

    VectorDirection currentVec = {0.0f, 0.0f};

    //vTaskDelay(pdMS_TO_TICKS(3000));  //3s para dejar el robot

    rsL.prev = accL.count;
    rsR.prev = accR.count;

    uint64_t previous_time = esp_timer_get_time();   


    while (true) {
        uint64_t now = esp_timer_get_time();
        float dTime = (now - previous_time) / 1e6f;
        previous_time = now;

        uint32_t now_ms = (uint32_t)(now / 1000);
        //robotState = tests(now_ms - t_start_ms);             // Pruebas
        bool go = gpio_get_level(GO_PIN);     // revisa la polaridad en tu módulo
        if (!go) {
            robotSumo.changeState(IDLE);
            currentVec = {0.0f, 0.0f};
        } else if (robotSumo.getState() == IDLE) {
            robotSumo.changeState(SEARCHING);
        }

        robotState = robotSumo.getState();

        static int loop_counter = 0;   // Contador para imprimir cada 20 iteraciones  SEGUN EL LOOP TIME DE 10ms, SE IMPRIME CADA 200ms
        loop_counter = loop_counter + 1;
        if (loop_counter % 20)
        {
            loop_counter = 0;
            printf("Robot State: %d, GO: %d, PWM_L: %.2f, PWM_R: %.2f, RPM_L: %.2f, RPM_R: %.2f\n", robotState, go, K_FF * currentVec.v - currentVec.w * wheelBase * 0.5f, K_FF * currentVec.v + currentVec.w * wheelBase * 0.5f, rsL.RPM, rsR.RPM);
        }

        //RPM
        float rpmL = wheel_rpm(accL, rsL, ENC_SIGN_L, dTime);
        float rpmR = wheel_rpm(accR, rsR, ENC_SIGN_R, dTime);

        //DistanceS::distances medidas = DistanceS::readAll();      // SE QUITA EL COMENTARIO PARA QUE NO SE LEA EL SENSOR DE DISTANCIA, YA QUE NO SE ESTA USANDO EN EL CODIGO ACTUAL PARA QUE COMPILE
        
        // Vector objetivo
        VectorDirection targetVec = getStateMotor(robotState);
        currentVec = rampVector(currentVec, targetVec, dTime);
        WheelRPM sp = calculateWheelRPM(currentVec, r_llantas, wheelBase);

        // Control PID
        constexpr float PID_LIMIT = PWM_MAX * 0.5f;   
        float pwmL = K_FF * sp.left  + PID_calc(izq, sp.left,  rpmL, dTime, PID_LIMIT);
        float pwmR = K_FF * sp.right + PID_calc(der, sp.right, rpmR, dTime, PID_LIMIT);

        // IDLE
        if (robotState == IDLE) {
            PID_reset(izq);
            PID_reset(der);
            pwmL = 0.0f;
            pwmR = 0.0f;
        }

        updateMotors(pwmL, pwmR);

        vTaskDelay(pdMS_TO_TICKS(loop_time));
    }
};
//=======================================================MAIN-END=======================================================
