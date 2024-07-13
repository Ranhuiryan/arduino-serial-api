#include <AccelStepper.h>
#include <MultiStepper.h>
#include <stdlib.h>
#include <string.h>

#define STEPPER1_DIR_PIN    6
#define STEPPER1_STEP_PIN   7
#define STEPPER2_DIR_PIN    8
#define STEPPER2_STEP_PIN   9
#define interruptPin1 3
#define interruptPin2 2
#define init   4
#define speed_up 5
#define zero 11
#define report   12
#define CIRCLE_Puls         800
#define Ena_PIN             10  // 使能引脚为引脚10。(低电平电机禁止运动，高电平电机有效)
unsigned long previousMillis = 0;  // 存储上一次打印位置的时间
const unsigned long interval = 1000;  // 定义打印位置的时间间隔（毫秒）


float speed = -200;
int Acceleration = -200;
float move_to = -9999999999;
float position_start = 0;
float position_now = 0;
char lis;

void remove_quotes(char *str) {
    size_t len = strlen(str);
    if (str[0] == '"' && str[len - 1] == '"') {
        memmove(str, str + 1, len - 2);
        str[len - 2] = '\0';
    }
}

void parse_input(const char *input_string, int *MaxSpeed, int *Acceleration, int *moveto) {
    char temp_str[100];
    strncpy(temp_str, input_string, sizeof(temp_str));
    temp_str[sizeof(temp_str) - 1] = '\0';

    remove_quotes(temp_str);

    char *token = strtok(temp_str, ",");
    if (token != NULL) {
        *MaxSpeed = atoi(token);
    } else {
        Serial.println(" ");
        return;
    }

    token = strtok(NULL, ",");
    if (token != NULL) {
        *Acceleration = atoi(token);
    } else {
        Serial.println(" ");
        return;
    }

    token = strtok(NULL, ",");
    if (token != NULL) {
        *moveto = atoi(token);
    } else {
        Serial.println(" ");
        return;
    }
}

AccelStepper stepper1(AccelStepper::DRIVER, STEPPER1_STEP_PIN, STEPPER1_DIR_PIN);  // 配置电机1 的脉冲引脚 方向引脚
AccelStepper stepper2(AccelStepper::DRIVER, STEPPER2_STEP_PIN, STEPPER2_DIR_PIN);  // 配置电机2 的脉冲引脚 方向引脚
void Botton_mode_1() {
            stepper1.setMaxSpeed(speed);
            stepper2.setMaxSpeed(speed);
            stepper1.setAcceleration(Acceleration);
            stepper2.setAcceleration(Acceleration); 
            stepper1.moveTo(move_to);        
            stepper2.moveTo(move_to); 

            
            }
void serialEvent(){
   String inString = "";
    while (Serial.available() > 0) {
        inString += char(Serial.read());
        delay(100);  // 延时函数用于等待字符完全进入缓冲区，可以尝试没有延时，输出结果会是什么
    }
    // 检查是否接收到数据，如果接收到数据，则输出该数据
    if (inString != "") {
        // isSerialControl = true; // 串口控制标志位置为true
        int MaxSpeed, Acceleration, moveto, loc;
        parse_input(inString.c_str(), &MaxSpeed, &Acceleration, &moveto);
        stepper1.setMaxSpeed(-MaxSpeed);
        stepper2.setMaxSpeed(-MaxSpeed);
        stepper1.setAcceleration(-Acceleration);
        stepper2.setAcceleration(-Acceleration);
        loc =  loc + moveto;
        stepper1.moveTo(-moveto);        
        stepper2.moveTo(-moveto);
        // speed = MaxSpeed;
    }
}                   
           
void Botton_mode_2() {
            stepper1.setMaxSpeed(0);
            stepper2.setMaxSpeed(0);
            stepper1.setAcceleration(0);
            stepper2.setAcceleration(0);
            // speed = 0;
            // Acceleration = 0;
            // move_to = 0;
}

void setup() {  
    Serial.begin(9600);
    stepper1.moveTo(0);        
    stepper2.moveTo(0);
    pinMode(Ena_PIN, OUTPUT);
    digitalWrite(Ena_PIN, HIGH);  // 高电平使能电机
    attachInterrupt(digitalPinToInterrupt(interruptPin1), Botton_mode_1,CHANGE);
    attachInterrupt(digitalPinToInterrupt(interruptPin2), Botton_mode_2,CHANGE);
}

void loop() {
    
    if (digitalRead(zero) == HIGH){
      position_start = stepper1.currentPosition();
      digitalWrite(zero,LOW);
          }
    if (digitalRead(init) == HIGH){
      speed = 800;
      Acceleration = 400;
      move_to = position_start;
      digitalWrite(init,LOW);  
       }
    if (digitalRead(speed_up)==HIGH){
      speed = -400;
      digitalWrite(speed_up,LOW);
      // Serial.println(speed);

    }
    if (digitalRead(report)==HIGH){
    speed = -800;
    digitalWrite(report,LOW);
    }
    // if (digitalRead(interruptPin1)==HIGH){
    //   // speed = -200;
    //   Acceleration = -200;
    //   move_to = -9999999999;
    // }
    stepper1.run();        
    stepper2.run();

  unsigned long currentMillis = millis();  // 获取当前时间（毫秒）

  // 检查是否已过1000毫秒并且步进电机正在运行
  if (currentMillis - previousMillis >= interval && stepper1.isRunning()) {
    previousMillis = currentMillis;  // 更新上一次打印位置的时间

    // 创建包含当前位置和速度的字符串缓冲区
    String buffer = String(stepper1.currentPosition()) + "," + String(speed);
    // Serial.println(buffer);  // 将缓冲区打印到串口监视器
  }

      } 
  

  // 每次运行步进电机时，重置串口控制标志位