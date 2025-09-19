#include "driver/ledc.h"

#ifndef SERVO_H
#define SERVO_H




// // TowerPro MG995 大舵机（以及许多其他业余爱好者使用的舵机）的默认参数
// #define DEFAULT_uS_LOW 1000        // 1000微秒，舵机脉冲宽度下限
// #define DEFAULT_uS_HIGH 2000      // 2000微秒，舵机脉冲宽度上限

//TowerPro SG90 小舵机的参数，默认注释掉，若使用该舵机可取消注释
#define DEFAULT_uS_LOW 400
#define DEFAULT_uS_HIGH 2400

  
#define DEFAULT_TIMER_WIDTH 16    // 默认的PWM定时器宽度
#define DEFAULT_TIMER_WIDTH_TICKS 65536   // 默认定时器宽度对应的计数值
  
// 本库的软件版本号
#define ESP32_Servo_VERSION           1 
       
 
 
#define MIN_PULSE_WIDTH       500       // 发送给舵机的最短脉冲宽度
 
#define MAX_PULSE_WIDTH      2500      // 发送给舵机的最长脉冲宽度
 
#define DEFAULT_PULSE_WIDTH  1500      // 默认的脉冲宽度   
 
#define DEFAULT_PULSE_WIDTH_TICKS 4825 // 默认的脉冲宽度对应的计数值
 
#define REFRESH_CPS            50        // 舵机刷新频率（赫兹）
 
#define REFRESH_USEC         20000      // 舵机刷新频率对应的微秒数
 
#define MAX_SERVOS              16        // 最多允许的舵机数量

class Servo {
public:
    Servo();
    ~Servo();

    int attach(int pin, int minPulseWidth = 500, int maxPulseWidth = 2500);
    void detach();
    void write(int value);       // 设置角度 (0-180度)
    void writeMicroseconds(int value); // 设置脉宽 (微秒)
    int read();                  // 返回当前角度
    int readMicroseconds();      // 返回当前脉宽 (微秒)
    bool attached();             // 检查是否已连接

private:
    int pin;                     // GPIO引脚
    int channel;                 // LEDC通道
    int minPulseWidth;           // 最小脉宽 (微秒)
    int maxPulseWidth;           // 最大脉宽 (微秒)
    int currentPulseWidth;       // 当前脉宽 (微秒)
    bool isAttached;             // 是否已连接

    void configureLEDC();        // 配置LEDC
};

#endif // SERVO_H