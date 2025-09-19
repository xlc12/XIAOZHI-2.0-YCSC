/*
    Otto机器人控制器 - MCP协议版本
*/

#include <cJSON.h>
#include <esp_log.h>

#include <cstring>

#include "application.h"
#include "board.h"
#include "config.h"
#include "mcp_server.h"

#include "sdkconfig.h"
#include "settings.h"

#include "ESP32_Servo.h"
#include "MiniKame_hi.h"

#define TAG "HiController"

class HiController {
private:
    // Otto otto_;
    MiniKame_hi robot;
    TaskHandle_t action_task_handle_ = nullptr;
    QueueHandle_t action_queue_;
    bool is_action_in_progress_ = false;

    struct OttoActionParams {
        int action_type;
        int steps;
        int speed;
        int direction;
        int amount;
    };

    enum ActionType {
        ACTION_WALK = 1,
        ACTION_BACKWARD = 2,
        ACTION_TRUN_L = 3,
        ACTION_TRUN_R = 4,
        ACTION_HELLO = 5,
        ACTION_SWAY = 6,
        ACTION_NOD=7,

        ACTION_HOME = 12,
        ACTION_ZERO = 13,

        
        
    };

    /******** ActionTask ********/
    static void ActionTask(void* arg) {
        HiController* controller = static_cast<HiController*>(arg);
        OttoActionParams params;
        // controller->otto_.AttachServos();
        controller->robot.init();
        controller->robot.zero();

        while (true) {
            if (xQueueReceive(controller->action_queue_, &params, pdMS_TO_TICKS(1000)) == pdTRUE) {
                ESP_LOGI(TAG, "执行动作: %d", params.action_type);
                controller->is_action_in_progress_ = true;

                switch (params.action_type) {
                    /***** 动作1 ACTION_RUN 跑步 *****/
                    case ACTION_HOME:
                    controller->robot.home();
                    ESP_LOGI(TAG, "666666666:111111111111");
                        break;
                    
                    case ACTION_ZERO:
                    controller->robot.zero();
                    ESP_LOGI(TAG, "666666666:22222222222");
                        break;


                    case ACTION_WALK:
                    controller->robot.walk(params.steps, params.speed);
                    ESP_LOGI(TAG, "666666666:333333333333");
                        break;

                    case ACTION_BACKWARD:
                    controller->robot.backward(params.steps, params.speed);
                    ESP_LOGI(TAG, "666666666:444444444444");
                        break;

                    case ACTION_TRUN_L:
                    controller->robot.turn_left(params.steps, params.speed);
                    ESP_LOGI(TAG, "666666666:555555555555");
                        break;

                    case ACTION_TRUN_R:
                    controller->robot.turn_right(params.steps, params.speed);
                    ESP_LOGI(TAG, "666666666:666666666666");
                        break;

                    case ACTION_HELLO:
                    controller->robot.hello(params.steps, params.speed);
                    ESP_LOGI(TAG, "666666666:777777777777");
                        break;

                    case ACTION_SWAY:
                    controller->robot.sway(params.steps, params.speed);
                    ESP_LOGI(TAG, "666666666:888888888888");
                        break;
                    
                    case ACTION_NOD:
                    controller->robot.nod(params.steps, params.speed);
                    ESP_LOGI(TAG, "666666666:999999999999");
                        break;
                    
                }


                

                
                controller->is_action_in_progress_ = false;
                vTaskDelay(pdMS_TO_TICKS(20));
            }
            ESP_LOGI(TAG, "666666666:d");
        }
    }
    /******** ActionTask end********/

    void StartActionTaskIfNeeded() {
        if (action_task_handle_ == nullptr) {
            xTaskCreate(ActionTask, "otto_action", 1024 * 3, this, configMAX_PRIORITIES - 1,
                        &action_task_handle_);
        }
    }

    void QueueAction(int action_type, int steps, int speed, int direction, int amount) {
        

        ESP_LOGI(TAG, "动作控制: 类型=%d, 步数=%d, 速度=%d, 方向=%d, 幅度=%d", action_type, steps,
                 speed, direction, amount);

        OttoActionParams params = {action_type, steps, speed, direction, amount};
        xQueueSend(action_queue_, &params, portMAX_DELAY);
        StartActionTaskIfNeeded();
    }

    

public:
    HiController() {
      
        
  
        robot.init();
        robot.zero();

        action_queue_ = xQueueCreate(10, sizeof(OttoActionParams));

        QueueAction(ACTION_HOME, 1, 1000, 1, 0);  // direction=1表示复位手部

        RegisterMcpTools(); 
    }

    void RegisterMcpTools() {
        auto& mcp_server = McpServer::GetInstance();

        ESP_LOGI(TAG, "开始注册MCP工具...");

        // 基础移动动作
        /***** 动作1 ACTION_RUN 跑步 *****/
        mcp_server.AddTool("self.otto.action_run",
            "前进。steps: 前进步数(1-15); speed: 前进速度(50-800，数值越小越快); ",
            PropertyList({Property("steps", kPropertyTypeInteger, 3, 1, 15),
                          Property("speed", kPropertyTypeInteger, 100, 50, 800)
                         }),
            [this](const PropertyList& properties) -> ReturnValue {
             float steps = (float)properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                QueueAction(ACTION_WALK, steps, speed, 0, 0);
                return true;
            });

            /***** 动作2 ACTION_BACKWARD 后退 *****/
        mcp_server.AddTool("self.otto.action_backward",
            "后退。steps: 后退步数(1-15); speed: 后退速度(50-800，数值越小越快); ",
            PropertyList({Property("steps", kPropertyTypeInteger, 3, 1, 15),
                          Property("speed", kPropertyTypeInteger, 100, 50, 800)
                         }),
            [this](const PropertyList& properties) -> ReturnValue {
             float steps = (float)properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                QueueAction(ACTION_BACKWARD, steps, speed, 0, 0);
                return true;
            });

            /***** 动作3 ACTION_TRUN_L 左转 *****/
        mcp_server.AddTool("self.otto.action_trun_l",
            "左转。steps: 左转步数(1-10); speed: 左转速度(50-800，数值越小越快); ",
            PropertyList({Property("steps", kPropertyTypeInteger, 3, 1, 10),
                          Property("speed", kPropertyTypeInteger, 100, 50, 800)
                         }),
            [this](const PropertyList& properties) -> ReturnValue {
                float steps = (float)properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                QueueAction(ACTION_TRUN_L, steps, speed, 0, 0);
                return true;
            });

            /***** 动作4 ACTION_TRUN_R 右转 *****/
        mcp_server.AddTool("self.otto.action_trun_r",
            "右转。steps: 右转步数(1-10); speed: 右转速度(50-800，数值越小越快); ",
            PropertyList({Property("steps", kPropertyTypeInteger, 3, 1, 10),
                          Property("speed", kPropertyTypeInteger, 100, 50, 800)
                         }),
            [this](const PropertyList& properties) -> ReturnValue {
                float steps = (float)properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                QueueAction(ACTION_TRUN_R, steps, speed, 0, 0);
                return true;
            });

         /***** 动作5 ACTION_HELLO 打招呼hello *****/
        mcp_server.AddTool("self.otto.action_hello",
            "打招呼。steps: 打招呼次数(1-10); speed: 打招呼速度(50-800，数值越小越快); ",
            PropertyList({Property("steps", kPropertyTypeInteger, 3, 1, 10),
                          Property("speed", kPropertyTypeInteger, 100, 50, 800)
                         }),
            [this](const PropertyList& properties) -> ReturnValue {
                float steps = (float)properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                QueueAction(ACTION_HELLO, steps, speed, 0, 0);
                return true;
            });

        /***** 动作6 ACTION_SWAY 摇摆 *****/
        mcp_server.AddTool("self.otto.action_sway",
            "摇摆。steps: 摇摆次数(1-10); speed: 摇摆速度(50-800，数值越小越快); ",
            PropertyList({Property("steps", kPropertyTypeInteger, 3, 1, 10),
                          Property("speed", kPropertyTypeInteger, 100, 50, 800)
                         }),
            [this](const PropertyList& properties) -> ReturnValue {
                float steps = (float)properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                QueueAction(ACTION_SWAY, steps, speed, 0, 0);
                return true;
            });

        /***** 动作7 ACTION_NOD 点头 *****/
        mcp_server.AddTool("self.otto.action_nod",
            "点头。steps: 点头次数(1-10); speed: 点头速度(50-800，数值越小越快); ",
            PropertyList({Property("steps", kPropertyTypeInteger, 3, 1, 10),
                          Property("speed", kPropertyTypeInteger, 100, 50, 800)
                         }),
            [this](const PropertyList& properties) -> ReturnValue {
                float steps = (float)properties["steps"].value<int>();
                int speed = properties["speed"].value<int>();
                QueueAction(ACTION_NOD, steps, speed, 0, 0);
                return true;
            });

        






/***** 动作2 ACTION_HOME 复位 *****/
        mcp_server.AddTool("self.otto.action_home",
            "复位。",
            PropertyList(),
            [this](const PropertyList& properties) -> ReturnValue {
                QueueAction(ACTION_HOME, 0, 0, 0, 0);
                return true;
            });

/***** 动作3 ACTION_ZERO 校准 *****/
        mcp_server.AddTool("self.otto.action_zero",
            "归零",
            PropertyList(),
            [this](const PropertyList& properties) -> ReturnValue {
                QueueAction(ACTION_ZERO, 0, 0, 0, 0);
                return true;
            });
        

       

        ESP_LOGI(TAG, "MCP工具注册完成");
    }

    ~HiController() {
        if (action_task_handle_ != nullptr) {
            vTaskDelete(action_task_handle_);
            action_task_handle_ = nullptr;
        }
        vQueueDelete(action_queue_);
    }
};

static HiController* g_otto_controller = nullptr;

void InitializeHiController() {

    if (g_otto_controller == nullptr) {
        g_otto_controller = new HiController();
        ESP_LOGI(TAG, "Otto控制器已初始化并注册MCP工具");
    }
}
