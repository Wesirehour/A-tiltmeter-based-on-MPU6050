/*
 * author: wesirehour
 * github: www.github.com/wesirehour
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifiiot_errno.h"
#include "wifiiot_gpio.h"
#include "wifiiot_gpio_ex.h"
#include "wifiiot_i2c.h"
#include "wifiiot_i2c_ex.h"
#include "nfc.h"

#include "wifi_connect.h"
#include <queue.h>
#include <oc_mqtt_al.h>
#include <oc_mqtt_profile.h>
#include "E53_SC2.h"
#include <dtls_al.h>
#include <mqtt_al.h>

#include  <math.h>

#define I2C_TASK_STACK_SIZE 1024 * 8
#define I2C_TASK_PRIO 25

#define CONFIG_WIFI_SSID          "mywifi"                            //修改为自己的WiFi 热点账号

#define CONFIG_WIFI_PWD           "12345678909"                        //修改为自己的WiFi 热点密码

// #define CONFIG_APP_SERVERIP       "121.36.42.100"                       //基础版平台对接地址

#define CONFIG_APP_SERVERIP       "117.78.5.125"                       //标准版平台对接地址

#define CONFIG_APP_SERVERPORT     "1883"

#define CONFIG_APP_DEVICEID       "6564bf4f585c81787ad44dd3_2020111409"       //替换为注册设备后生成的deviceid

#define CONFIG_APP_DEVICEPWD      "12345678"                                   //替换为注册设备后生成的密钥

#define CONFIG_APP_LIFETIME       60     ///< seconds

#define CONFIG_QUEUE_TIMEOUT      (5*1000)

#define MSGQUEUE_OBJECTS 16 // number of Message Queue Objects
#define TASK_STACK_SIZE 1024 * 8
#define TASK_PRIO 25

#define PI 3.14159265358979323846

typedef enum
{
    en_msg_cmd = 0,
    en_msg_report,
}en_msg_type_t;

typedef struct
{
    char *request_id;
    char *payload;
} cmd_t;


typedef struct
{
    float Pitch;
    float Roll;
} report_t;

typedef struct
{
    en_msg_type_t msg_type;
    union
    {
        report_t report;
    } msg;
} app_msg_t;

typedef struct
{
    queue_t *app_msg;
    int connected;

}app_cb_t;
static app_cb_t  g_app_cb;

E53_SC2_Data_TypeDef E53_SC2_Data;
int X = 0,Y = 0,Z = 0, Cover_Status = 0;
int init_X, init_Y, init_Z;
float ax_g, ay_g, az_g;
float pitch, roll;
char TEXT[1000]; 

// 由于本人编译时报错不存在atan2函数，于是在这手动实现
double atan2(double y, double x) 
{
    double angle;

    if (x > 0.0) {
        angle = atan(y / x);
    }
    else if (x < 0.0 && y >= 0.0) {
        angle = atan(y / x) + PI;
    }
    else if (x < 0.0 && y < 0.0) {
        angle = atan(y / x) - PI;
    }
    else if (x == 0.0 && y > 0.0) {
        angle = PI / 2;
    }
    else if (x == 0.0 && y < 0.0) {
        angle = -PI / 2;
    }
    else {
        angle = 0.0;
    }

    return angle;
}

static void deal_report_msg(report_t *report)
{
    oc_mqtt_profile_service_t service;
    oc_mqtt_profile_kv_t Pitch;
    oc_mqtt_profile_kv_t Roll;
    oc_mqtt_profile_kv_t Accel_y;
    oc_mqtt_profile_kv_t Accel_z;
    oc_mqtt_profile_kv_t status;

    if(g_app_cb.connected != 1){
        return;
    }

    service.event_time = NULL;
    service.service_id = "Manhole_Cover";
    service.service_property = &Pitch;
    service.nxt = NULL;
    
    Pitch.key = "Pitch";
    Pitch.value = &report->Pitch;
    Pitch.type = EN_OC_MQTT_PROFILE_VALUE_FLOAT;
    Pitch.nxt = &Roll;

    Roll.key = "Roll";
    Roll.value = &report->Roll;
    Roll.type = EN_OC_MQTT_PROFILE_VALUE_FLOAT;
    Roll.nxt = &status;

    status.key = "Cover_Status";
    status.value = Cover_Status ? "Tilt" : "Level";
    status.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    status.nxt = NULL;

    oc_mqtt_profile_propertyreport(NULL,&service);
    return;
}

static int task_main_entry(void)
{
    app_msg_t *app_msg;
    uint32_t ret ;
    
    WifiConnect(CONFIG_WIFI_SSID, CONFIG_WIFI_PWD);
    dtls_al_init();
    mqtt_al_init();
    oc_mqtt_init();
    
    g_app_cb.app_msg = queue_create("queue_rcvmsg",10,1);
    if(NULL ==  g_app_cb.app_msg){
        printf("Create receive msg queue failed");
        
    }
    oc_mqtt_profile_connect_t  connect_para;
    (void) memset( &connect_para, 0, sizeof(connect_para));

    connect_para.boostrap =      0;
    connect_para.device_id =     CONFIG_APP_DEVICEID;
    connect_para.device_passwd = CONFIG_APP_DEVICEPWD;
    connect_para.server_addr =   CONFIG_APP_SERVERIP;
    connect_para.server_port =   CONFIG_APP_SERVERPORT;
    connect_para.life_time =     CONFIG_APP_LIFETIME;
    connect_para.rcvfunc =       NULL;
    connect_para.security.type = EN_DTLS_AL_SECURITY_TYPE_NONE;
    ret = oc_mqtt_profile_connect(&connect_para);
    if((ret == (int)en_oc_mqtt_err_ok)){
        g_app_cb.connected = 1;
        printf("oc_mqtt_profile_connect succed!\r\n");
    }
    else
    {
        printf("oc_mqtt_profile_connect faild!\r\n");
    }
    

    while (1)
    {
        app_msg = NULL;
        (void)queue_pop(g_app_cb.app_msg,(void **)&app_msg,0xFFFFFFFF);
        if (NULL != app_msg)
        {
            switch (app_msg->msg_type)
            {
            case en_msg_report:
                deal_report_msg(&app_msg->msg.report);
                break;
            default:
                break;
            }
            free(app_msg);
        }
    }
    return 0;
}

uint8_t ret;
bool upload_flag = false, save_flag = false;
static void F1_Pressed(char *arg) // 保存 TEXT
{
    (void)arg;
    save_flag = true;
}


static void F2_Pressed(char *arg) // 上传云端 
{
    (void)arg;
    upload_flag = true;
}

static void Example_Task(void)
{

    float Lux;

    E53_SC2_Init();
    //初始化GPIO
    GpioInit();
    //设置GPIO_2的复用功能为普通GPIO
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_2, WIFI_IOT_IO_FUNC_GPIO_2_GPIO);
    //设置GPIO_2为输出模式
    GpioSetDir(WIFI_IOT_GPIO_IDX_2, WIFI_IOT_GPIO_DIR_OUT);

    //GPIO_0复用为I2C1_SDA
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_0, WIFI_IOT_IO_FUNC_GPIO_0_I2C1_SDA);
    //GPIO_1复用为I2C1_SCL
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_1, WIFI_IOT_IO_FUNC_GPIO_1_I2C1_SCL);
    //baudrate: 400kbps
    I2cInit(WIFI_IOT_I2C_IDX_1, 400000);
    I2cSetBaudrate(WIFI_IOT_I2C_IDX_1, 400000);

    //初始化F1按键，设置为下降沿触发中断
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_IO_FUNC_GPIO_11_GPIO);

    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_IO_PULL_UP);
    GpioRegisterIsrFunc(WIFI_IOT_IO_NAME_GPIO_11, WIFI_IOT_INT_TYPE_EDGE, WIFI_IOT_GPIO_EDGE_FALL_LEVEL_LOW, F1_Pressed, NULL);

    //初始化F2按键，设置为下降沿触发中断
    IoSetFunc(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_IO_FUNC_GPIO_12_GPIO);

    GpioSetDir(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_GPIO_DIR_IN);
    IoSetPull(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_IO_PULL_UP);
    GpioRegisterIsrFunc(WIFI_IOT_IO_NAME_GPIO_12, WIFI_IOT_INT_TYPE_EDGE, WIFI_IOT_GPIO_EDGE_FALL_LEVEL_LOW, F2_Pressed, NULL);


    E53_SC2_Read_Data();
    init_X = (int)E53_SC2_Data.Accel[0];
    init_Y = (int)E53_SC2_Data.Accel[1];
    init_Z = (int)E53_SC2_Data.Accel[2];
    ax_g = (float) (init_X / 16384.0);
    ay_g = (float) (init_Y / 16384.0);
    az_g = (float) (init_Z / 16384.0);
    float init_pitch = atan2(ax_g  , sqrt(ay_g * ay_g + az_g * az_g));
    init_pitch = init_pitch * 180 / PI;
    float init_roll =  atan2(ay_g ,  sqrt(ax_g * ax_g + az_g * az_g));
    init_roll = init_roll * 180 / PI;

    while (1)
    {
        E53_SC2_Read_Data();
        int now_x = (int)E53_SC2_Data.Accel[0];
        int now_y = (int)E53_SC2_Data.Accel[1];
        int now_z = (int)E53_SC2_Data.Accel[2];
        int sub_x = now_x  - init_X;
        int sub_y = now_y  - init_Y;
        int sub_z = now_z - init_Z;
        ax_g = (float) (now_x / 16384.0);
        ay_g = (float) (now_y / 16384.0);
        az_g = (float) (now_z / 16384.0);
        pitch =  atan2(ax_g  , sqrt(ay_g * ay_g + az_g * az_g)) * 180 / PI - init_pitch;
        roll =  atan2(ay_g ,  sqrt(ax_g * ax_g + az_g * az_g)) * 180 / PI - init_roll;

        sprintf(TEXT, "Pitch: %.2f, Roll: %.2f", pitch, roll);
        if (save_flag) 
        {
            ret = storeText(NDEFFirstPos, (uint8_t *)TEXT);
            if (ret != 1)
            {
                printf("NFC Write Data Falied :%d\n ", ret);
            }
            save_flag = false;
        }

        if ((X == 0 && Y == 0 && Z == 0) || (fabs(sub_x)  < 25 && fabs( sub_y) < 25 && fabs( sub_z) < 25 ))
        {
            LED_D1_StatusSet(OFF);
            LED_D2_StatusSet(OFF);
            GpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, 0);
            Cover_Status = 1;
        }
        else
        {
            Cover_Status = 0;
            if (now_x - init_X >= 250 )  // left led
            {
                LED_D1_StatusSet(ON);
                LED_D2_StatusSet(OFF);
                GpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, 0);
            }
            else if (now_x - init_X <= -250) // right_led
            {
                LED_D1_StatusSet(OFF);
                LED_D2_StatusSet(ON);
                GpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, 0);
            }
            else if ( now_y - init_Y <= -250) {
                LED_D1_StatusSet(OFF);
                LED_D2_StatusSet(OFF);
                GpioSetOutputVal(WIFI_IOT_GPIO_IDX_2, 1);
            }
        }
        X = now_x;
        Y = now_y;
        Z = now_z;
        if(upload_flag) 
        {
             app_msg_t *app_msg;
            app_msg = malloc(sizeof(app_msg_t));       

            if (NULL != app_msg)
            {
                app_msg->msg_type = en_msg_report;
                app_msg->msg.report.Pitch = pitch;
                app_msg->msg.report.Roll = roll;

                if(0 != queue_push(g_app_cb.app_msg,app_msg,CONFIG_QUEUE_TIMEOUT)){
                    free(app_msg);
                }
            }
            upload_flag = false;
        }
        usleep(500); //每次三轴加速度变化的延时
    }
}



static void ExampleEntry(void)
{
    osThreadAttr_t attr;

    attr.name = "Example_Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = TASK_STACK_SIZE;
    attr.priority = TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)Example_Task, NULL, &attr) == NULL)
    {
        printf("Falied to create Example_Task!\n");
    }
    if (osThreadNew((osThreadFunc_t)task_main_entry, NULL, &attr) == NULL)
    {
        printf("Falied to create task_main_entry!\n");
    }
}

APP_FEATURE_INIT(ExampleEntry);