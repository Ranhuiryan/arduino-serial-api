#include <AccelStepper.h>
#include <MultiStepper.h>
#include <stdlib.h>
#include <string.h>

#define STEPPER1_DIR_PIN 6
#define STEPPER1_STEP_PIN 7
#define STEPPER2_DIR_PIN 8
#define STEPPER2_STEP_PIN 9
#define INTERRUPT_PIN1 3
#define INTERRUPT_PIN2 2
#define BUTTON_INIT 4
#define BUTTON_SPEED_UP 5
#define BUTTON_ZERO 11
#define BUTTON_REPORT 12
#define ENA_PIN 10 // 使能引脚为引脚10。(低电平电机禁止运动，高电平电机有效)

unsigned long previousMillis = 0;    // 存储上一次打印位置的时间
const unsigned long reportInterval = 1000; // 定义打印位置的时间间隔（毫秒）

AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN); // 配置电机1 的脉冲引脚 方向引脚
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN); // 配置电机2 的脉冲引脚 方向引脚

float targetSpeed = 200;
int targetAcceleration = 200;
float targetPosition = -9999999999;
float startPosition = 0;
long nowPosition = 0;

void remove_quotes(char *str)
{
    size_t len = strlen(str);
    if (str[0] == '"' && str[len - 1] == '"')
    {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

void parse_input(const char *input_string, float *MaxSpeed, int *Acceleration, float *moveto)
{
    char temp_str[100];
    strncpy(temp_str, input_string, sizeof(temp_str));
    temp_str[sizeof(temp_str) - 1] = '\0';

    remove_quotes(temp_str);

    char *token = strtok(temp_str, ",");
    if (token != NULL)
    {
        *MaxSpeed = atof(token);
    }
    else
    {
        Serial.println("Error: MaxSpeed missing");
        return;
    }

    token = strtok(NULL, ",");
    if (token != NULL)
    {
        *Acceleration = atoi(token);
    }
    else
    {
        Serial.println("Error: Acceleration missing");
        return;
    }

    token = strtok(NULL, ",");
    if (token != NULL)
    {
        *moveto = atof(token);
    }
    else
    {
        Serial.println("Error: MoveTo missing");
        return;
    }
}

void buttonMode1()
{
    stepper1.setMaxSpeed(targetSpeed);
    stepper2.setMaxSpeed(targetSpeed);
    stepper1.setAcceleration(targetAcceleration);
    stepper2.setAcceleration(targetAcceleration);
    stepper1.moveTo(targetPosition);
    stepper2.moveTo(targetPosition);
}

void buttonMode2()
{
    stepper1.setMaxSpeed(0);
    stepper2.setMaxSpeed(0);
    stepper1.setAcceleration(0);
    stepper2.setAcceleration(0);
}

void serialEvent()
{
    String inString = "";
    while (Serial.available() > 0)
    {
        inString += char(Serial.read());
        delay(100); // 延时函数用于等待字符完全进入缓冲区，可以尝试没有延时，输出结果会是什么
    }
    // 检查是否接收到数据，如果接收到数据，则输出该数据
    if (inString != "")
    {
        // isSerialControl = true; // 串口控制标志位置为true
        float MaxSpeed, moveto;
        int Acceleration, loc;
        parse_input(inString.c_str(), &MaxSpeed, &Acceleration, &moveto);
        stepper1.setMaxSpeed(MaxSpeed);
        stepper2.setMaxSpeed(MaxSpeed);
        stepper1.setAcceleration(Acceleration);
        stepper2.setAcceleration(Acceleration);
        loc =  nowPosition + moveto;
        Serial.println(-loc);
        stepper1.moveTo(-loc);
        stepper2.moveTo(-loc);
        // speed = MaxSpeed;
    }
}

void setup()
{
    Serial.begin(9600);
    pinMode(BUTTON_INIT, INPUT);
    pinMode(BUTTON_SPEED_UP, INPUT);
    pinMode(BUTTON_ZERO, INPUT);
    pinMode(BUTTON_REPORT, INPUT);
    pinMode(ENA_PIN, OUTPUT);
    digitalWrite(ENA_PIN, HIGH); // 高电平使能电机
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN1), buttonMode1, CHANGE);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN2), buttonMode2, CHANGE);
}

void loop()
{   nowPosition = abs(stepper1.currentPosition());
    if (digitalRead(BUTTON_ZERO) == HIGH)
    {
        startPosition = stepper1.currentPosition();
        //   digitalWrite(BUTTON_ZERO,LOW);
    }
    if (digitalRead(BUTTON_INIT) == HIGH)
    {
        targetSpeed = 800;
        targetAcceleration = 400;
        targetPosition = startPosition;
    }
    if (digitalRead(BUTTON_SPEED_UP) == HIGH)
    {
        targetSpeed = 400;
        targetPosition = -999999999;
        // digitalWrite(BUTTON_SPEED_UP,LOW);
        // Serial.println(speed);
    }
    if (digitalRead(BUTTON_REPORT) == HIGH)
    {
        targetSpeed = 800;
        targetPosition = -999999999;
        // digitalWrite(BUTTON_REPORT,LOW);
    }
    stepper1.run();
    stepper2.run();

    unsigned long currentMillis = millis(); // 获取当前时间（毫秒）

    // 检查是否已过1000毫秒并且步进电机正在运行
    if (currentMillis - previousMillis >= reportInterval && stepper1.isRunning())
    {
        previousMillis = currentMillis; // 更新上一次打印位置的时间

        // 创建包含当前位置和速度的字符串缓冲区
        String buffer = String(stepper1.currentPosition()) + "," + String(targetSpeed);
        // Serial.println(buffer);  // 将缓冲区打印到串口监视器
    }
}

// 每次运行步进电机时，重置串口控制标志位