#include "MiniKame_hi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <cmath>
#include "esp_timer.h"

#define millis()    (unsigned long) (esp_timer_get_time() / 1000ULL)
#define   Z_AG      80 

#define TAG "MiniKame_hi"


typedef enum {
    SERVO_FL = 0,
    SERVO_FR,
    SERVO_BL,
    SERVO_BR,
} servo_id_t;


static uint16_t fl_angle_neutral = Z_AG;
static uint16_t fr_angle_neutral = Z_AG;
static uint16_t bl_angle_neutral = Z_AG;
static uint16_t br_angle_neutral = Z_AG;

#define FL_ANGLE_STEP_FORWARD       (fl_angle_neutral - 20)
#define FL_ANGLE_STEP_BACKWARD      (fl_angle_neutral + 20)

#define FR_ANGLE_STEP_FORWARD       (fr_angle_neutral + 20)
#define FR_ANGLE_STEP_BACKWARD      (fr_angle_neutral - 20)

#define BL_ANGLE_STEP_FORWARD       (bl_angle_neutral + 20)
#define BL_ANGLE_STEP_BACKWARD      (bl_angle_neutral - 20)


#define BR_ANGLE_STEP_FORWARD       (br_angle_neutral - 20)
#define BR_ANGLE_STEP_BACKWARD      (br_angle_neutral + 20)




//v2
// #define FL_ANGLE_STEP_FORWARD       (fl_angle_neutral + 20)
// #define FL_ANGLE_STEP_BACKWARD      (fl_angle_neutral - 20)

// #define FR_ANGLE_STEP_FORWARD       (fr_angle_neutral - 20)
// #define FR_ANGLE_STEP_BACKWARD      (fr_angle_neutral + 20)

// #define BL_ANGLE_STEP_FORWARD       (bl_angle_neutral - 20)
// #define BL_ANGLE_STEP_BACKWARD      (bl_angle_neutral + 20)

// #define BR_ANGLE_STEP_FORWARD       (br_angle_neutral + 20)
// #define BR_ANGLE_STEP_BACKWARD      (br_angle_neutral - 20)



#define BOW_OFFSET                  50  //Front lying /backward offset angle

#define STEP_OFFSET                 5


void MiniKame_hi::init(){
    // Map between servos and board pins
    // board_pins[0] = 19; // Servo S0
    // board_pins[1] = 18; // Servo S1
    // board_pins[2] = 5; // Servo S2
    // board_pins[3] = 17; // Servo S3
    // board_pins[4] = 16; // Servo S4
    // board_pins[5] = 4; // Servo S5
    // board_pins[6] = 2; // Servo S6
    // board_pins[7] = 15; // Servo S7

    // board_pins[0] = 15; // Servo S0
    // board_pins[1] = 4; // Servo S1
    // board_pins[2] = 16; // Servo S2
    // board_pins[3] = 5; // Servo S3
    // board_pins[4] = 19; // Servo S4
    // board_pins[5] = 21; // Servo S5
    // board_pins[6] = 22; // Servo S6
    // board_pins[7] = 23; // Servo S7

    // board_pins[0] = 12; // Servo S0
    // board_pins[1] = 15; // Servo S1
    // board_pins[2] = 13; // Servo S2
    // board_pins[3] = 0; // Servo S3
    // board_pins[4] = 16; // Servo S4
    // board_pins[5] = 2; // Servo S5
    // board_pins[6] = 17; // Servo S6
    // board_pins[7] = 15; // Servo S7

    board_pins[0] = 43; // Servo S0 FL
    board_pins[1] = 44; // Servo S1 FR
    board_pins[2] = 46; // Servo S2 BL
    board_pins[3] = 3; // Servo S3  BR

    // Trim values for zero position calibration.
    trim[0] = 0;
    trim[1] = 0;
    trim[2] = 0;
    trim[3] = 0;
    // trim[4] = 0;
    // trim[5] = 0;
    // trim[6] = 0;
    // trim[7] = 0;

    // Set reverse movement
    for (int i=0; i<SERVO_NUM; i++) reverse[i] = false;


    
    
    // Init an oscillator for each servo
    for(int i=0; i<SERVO_NUM; i++){
        // oscillator[i].start();
        servo[i].attach(board_pins[i]);
    }
    // zero();
}





//////////////////////////////////


void MiniKame_hi::walk(float steps, int T=100){
    float sw = 20;
    if(T<150){
        sw = 25;
    }
    
    float turn_delay_ms = 50;
    float offset = 5;

    float walk_1[]={ 
                     Z_AG,          Z_AG-sw,
                      ///            ///
                     Z_AG+sw,       Z_AG
                };
    float walk_2[]={ 
                     Z_AG-sw,       Z_AG-sw,
                     ///            ///
                     Z_AG+sw,       Z_AG+sw
                };
    float walk_3[]={ 
                     Z_AG-sw,       Z_AG,
                     ///            ///
                     Z_AG,          Z_AG+sw
                };
    float walk_4[]={ 
                     Z_AG,          Z_AG,
                     ///            ///
                     Z_AG,          Z_AG
                };
    float walk_5[]={ 
                     Z_AG+sw,       Z_AG,
                     ///            ///
                     Z_AG,          Z_AG-sw
                    };
    float walk_6[]={ 
                     Z_AG+sw,       Z_AG+sw,
                     ///            ///
                     Z_AG-sw,      Z_AG-sw
                    };
    float walk_7[]={ 
                     Z_AG,       Z_AG+sw,
                     ///            ///
                     Z_AG-sw,      Z_AG
                    };
    float walk_8[]={ 
                     Z_AG,       Z_AG,
                     ///            ///
                     Z_AG,      Z_AG
                    };

                    // T=100;

    for(int i=0; i<steps; i++){
        moveServos(T, walk_1);
        moveServos(T, walk_2);
        moveServos(T, walk_3);
        moveServos(T, walk_4);
        moveServos(T, walk_5);
        moveServos(T, walk_6);
        moveServos(T, walk_7);
        moveServos(T, walk_8);
        
    }


}

void MiniKame_hi::backward(float steps, int T=100){
    float sw = 20;
    float offset = 5;

    float walk_1[]={ 
                    Z_AG,          Z_AG+sw,
                    ///            ///
                    Z_AG-sw,       Z_AG
                    };
    float walk_2[]={ 
                    Z_AG+sw,       Z_AG-sw,
                    ///            ///
                    Z_AG+sw,       Z_AG-sw
                    };
    float walk_3[]={ 
                    Z_AG+sw,       Z_AG,
                    ///            ///
                    Z_AG,          Z_AG-sw
    };
    float walk_4[]={ 
                    Z_AG,          Z_AG,
                    ///            ///
                    Z_AG,          Z_AG
    };
    float walk_5[]={ 
                    Z_AG-sw,       Z_AG,
                    ///            ///
                    Z_AG,          Z_AG+sw
                    };
    float walk_6[]={ 
                    Z_AG-sw,       Z_AG-sw,
                    ///            ///
                    Z_AG+sw,      Z_AG+sw
                    };  
    float walk_7[]={ 
                    Z_AG,       Z_AG-sw,
                    ///            ///
                    Z_AG+sw,      Z_AG
                    };
    float walk_8[]={ 
                    Z_AG,       Z_AG,
                    ///            ///
                    Z_AG,      Z_AG
                    };

        // T=100;

    for(int i=0; i<steps; i++){
        moveServos(T, walk_1);
        moveServos(T, walk_2);
        moveServos(T, walk_3);
        moveServos(T, walk_4);
        moveServos(T, walk_5);
        moveServos(T, walk_6);
        moveServos(T, walk_7);
        moveServos(T, walk_8);

    }

}


void MiniKame_hi::turn_left(float steps, int T=5000){
    float sw = 20;
    float offset = 10;

    float walk_1[]={ 
            Z_AG-sw,       Z_AG,
            ///            ///
            Z_AG,          Z_AG-sw
            };
    float walk_2[]={ 
            Z_AG-sw,       Z_AG+sw,
            ///            ///
            Z_AG+sw,       Z_AG-sw
            };
    float walk_3[]={ 
            Z_AG,          Z_AG+sw,
            ///            ///
            Z_AG+sw,       Z_AG
    };
    float walk_4[]={ 
            Z_AG,          Z_AG,
            ///            ///
            Z_AG,          Z_AG
    };


// T=100;

    for(int i=0; i<steps; i++){
        moveServos(T, walk_1);
        moveServos(T, walk_2);
        moveServos(T, walk_3);
        moveServos(T, walk_4);
    }
}

void MiniKame_hi::turn_right(float steps, int T=5000){
    float sw = 20;
    float offset = 10;

    float walk_1[]={ 
            Z_AG,          Z_AG+sw,
            ///            ///
            Z_AG+sw,       Z_AG
            };
    float walk_2[]={ 
            Z_AG-sw,       Z_AG+sw,
            ///            ///
            Z_AG+sw,       Z_AG-sw
            };
    float walk_3[]={ 
            Z_AG-sw,       Z_AG,
            ///            ///
            Z_AG,       Z_AG-sw
    };
    float walk_4[]={ 
            Z_AG,          Z_AG,
            ///            ///
            Z_AG,          Z_AG
    };


// T=100;

    for(int i=0; i<steps; i++){
        moveServos(T, walk_1);
        moveServos(T, walk_2);
        moveServos(T, walk_3);
        moveServos(T, walk_4);
    }
}

// Hello
void MiniKame_hi::hello(float steps, int T=100){
    float sw = 50;
    float out= 50;
    float in= 20;


    float walk_1[]={ 
                     Z_AG,          Z_AG-out,
                      ///            ///
                     Z_AG+sw,       Z_AG-sw
                };
    float walk_2[]={ 
                    Z_AG,          Z_AG-in,
                    ///            ///
                    Z_AG+sw,       Z_AG-sw
                };
    float walk_3[]={ 
                    Z_AG,          Z_AG-out,
                     ///            ///
                    Z_AG+sw,       Z_AG-sw
                };
    float walk_4[]={ 
                     Z_AG,          Z_AG,
                     ///            ///
                     Z_AG,          Z_AG
                };


    for(int i=0; i<steps; i++){
        moveServos(T, walk_1);
        moveServos(T, walk_2);
        // moveServos(T, walk_3);
        // moveServos(T, walk_4);
       
        
    }
    moveServos(T, walk_4);
}


//摇摆
void MiniKame_hi::sway(float steps, int T=100){
    float sw = 30;
    float out= 30;
    float in= 10;
    float offset = 5;
    float walk_1[]={ 
            Z_AG+sw,       Z_AG-sw,
            ///            ///
            Z_AG+sw,       Z_AG-sw
    };
    float walk_2[]={ 
            Z_AG-sw,       Z_AG+sw,
            ///            ///
            Z_AG-sw,       Z_AG+sw
    };
    float walk_3[]={ 
            Z_AG,          Z_AG-out,
                ///            ///
            Z_AG+sw,       Z_AG-sw
    };
    float walk_4[]={ 
            Z_AG,          Z_AG,
            ///            ///
            Z_AG,          Z_AG
    };


    for(int i=0; i<steps; i++){
    moveServos(T, walk_1);
    moveServos(T, walk_2);
    // moveServos(T, walk_3);
    // moveServos(T, walk_4);


    }
    moveServos(2*T, walk_4);
}



//点头
void MiniKame_hi::nod(float steps, int T=100){
    float sw = 70;


    float walk_1[]={ 
            Z_AG+sw,       Z_AG-sw,
            ///            ///
            Z_AG,          Z_AG
    };
    float walk_2[]={ 
            Z_AG,          Z_AG,
            ///            ///
            Z_AG,          Z_AG
    };

    for(int i=0; i<steps; i++){
        moveServos(T, walk_1);
        vTaskDelay(pdMS_TO_TICKS(200));
        moveServos(T, walk_2);
        
    
    
    }
}
   





/////////////////////////////////////




void MiniKame_hi::home(){
    int ap = 20;
    int hi = 35;
    // int position[] = {Z_AG+ap,Z_AG-ap,Z_AG-hi,Z_AG+hi,Z_AG-ap,Z_AG+ap,Z_AG+hi,Z_AG-hi};
    int position[] = {120, 0, 120, 60};

    // for (int i=0; i<SERVO_NUM; i++) setServo(i, position[i]);


    zero();



}

void MiniKame_hi::zero(){
    for (int i=0; i<SERVO_NUM; i++) setServo(i, Z_AG);
}

void MiniKame_hi::reverseServo(int id){
    if (reverse[id])
        reverse[id] = 0;
    else
        reverse[id] = 1;
}


void MiniKame_hi::setServo(int id, float target){
    //target原来值在0-180之间，现在映射为0-150
    // target = map(target, 0, 180, 0, 150);

    if (!reverse[id])
        servo[id].writeMicroseconds(angToUsec(target+trim[id]));
    else
        servo[id].writeMicroseconds(angToUsec(180-(target+trim[id])));
    _servo_position[id] = target;
}

float MiniKame_hi::getServo(int id){
    return _servo_position[id];
}

void MiniKame_hi::servo_set_angle(int id, float angle){
    setServo(id, angle);
}




void MiniKame_hi::moveServos(int time, float target[SERVO_NUM]) {
    
    if (time>10){
        for (int i = 0; i < SERVO_NUM; i++)  _increment[i] = (target[i] - _servo_position[i]) / (time / 10.0);
        _final_time =  millis() + time;

        while (millis() < _final_time){
            _partial_time = millis() + 10;
            for (int i = 0; i < SERVO_NUM; i++) setServo(i, _servo_position[i] + _increment[i]);
            // while (millis() < _partial_time); //pause
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    else{
        for (int i = 0; i < SERVO_NUM; i++) setServo(i, target[i]);
    }
    for (int i = 0; i < SERVO_NUM; i++) _servo_position[i] = target[i];
}




int MiniKame_hi::angToUsec(float value){
    return value/180 * (MAX_PULSE_WIDTH-MIN_PULSE_WIDTH) + MIN_PULSE_WIDTH;
}


float MiniKame_hi::map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
