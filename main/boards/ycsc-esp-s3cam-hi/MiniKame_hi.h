#ifndef minikame_h
#define minikame_h

#include "./ESP32_Servo.h"
#include <esp_log.h>
// #include "./Octosnake.h"

#define SERVO_NUM       4     //舵机个数

class MiniKame_hi{
public:
    void init();
    void walk(float steps, int T);
    void backward(float steps, int T);
    void turn_left(float steps, int T);
    void turn_right(float steps, int T);
    void hello(float steps, int T);
    //摇摆
    void sway(float steps, int T);
    //点头
    void nod(float steps, int T);
    
    void home();
    void zero();
    

    void setServo(int id, float target);
    void servo_set_angle(int id, float angle);
    void reverseServo(int id);
    float getServo(int id);
    void moveServos(int time, float target[SERVO_NUM]);


private:
    // Oscillator oscillator[SERVO_NUM];
    Servo servo[SERVO_NUM];
    int board_pins[SERVO_NUM];
    int trim[SERVO_NUM];
    bool reverse[SERVO_NUM];
    unsigned long _init_time;
    unsigned long _final_time;
    unsigned long _partial_time;
    float _increment[SERVO_NUM];
    float _servo_position[SERVO_NUM];

    int angToUsec(float value);
    void execute(float steps, int period[8], int amplitude[8], int offset[8], int phase[8]);
    float map(long x, long in_min, long in_max, long out_min, long out_max);
};

#endif
