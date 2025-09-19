#include "ESP32_Servo.h"


// #include "Servo.h"
#include "driver/ledc.h"

Servo::Servo() : pin(-1), channel(-1), minPulseWidth(500), maxPulseWidth(2500), currentPulseWidth(0), isAttached(false) {}

Servo::~Servo() {
    detach();
}

int Servo::attach(int pin, int minPulseWidth, int maxPulseWidth) {
    this->pin = pin;
    this->minPulseWidth = minPulseWidth;
    this->maxPulseWidth = maxPulseWidth;

    // 分配独立的 LEDC 通道
    static int nextChannel = 0; // 静态变量，用于分配通道
    if (nextChannel >= LEDC_CHANNEL_MAX) {
        return -1; // 如果通道超出最大值，返回错误
    }
    channel = nextChannel++;
    
    configureLEDC();
    isAttached = true;
    return channel;
}

void Servo::detach() {
    if (isAttached) {
        ledc_stop(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, 0);
        isAttached = false;
    }
}

void Servo::write(int value) {
    if (!isAttached) return;

    // 将角度转换为微秒值
    value = value < 0 ? 0 : (value > 180 ? 180 : value);
    currentPulseWidth = minPulseWidth + (value * (maxPulseWidth - minPulseWidth) / 180);
    writeMicroseconds(currentPulseWidth);
}

void Servo::writeMicroseconds(int value) {
    if (!isAttached) return;

    value = value < minPulseWidth ? minPulseWidth : (value > maxPulseWidth ? maxPulseWidth : value);
    currentPulseWidth = value;

    // 设置PWM占空比
    int duty = (value * 8192) / 20000; // 计算占空比，8192为13位分辨率的最大值(2^13)，20000为周期（20ms）
    ledc_set_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel, duty);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, (ledc_channel_t)channel);
}

int Servo::read() {
    if (!isAttached) return -1;

    // 将微秒值转换为角度
    return (currentPulseWidth - minPulseWidth) * 180 / (maxPulseWidth - minPulseWidth);
}

int Servo::readMicroseconds() {
    return isAttached ? currentPulseWidth : -1;
}

bool Servo::attached() {
    return isAttached;
}

void Servo::configureLEDC() {
    // 配置LEDC定时器
    ledc_timer_config_t timerConfig = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_13_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 50, // 舵机频率为50Hz
        .clk_cfg = LEDC_AUTO_CLK
    };
    ledc_timer_config(&timerConfig);

    // 配置LEDC通道 - 按照结构体声明顺序初始化成员
    ledc_channel_config_t channelConfig = {
        .gpio_num = pin,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = (ledc_channel_t)channel,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0
    };
    ledc_channel_config(&channelConfig);
}