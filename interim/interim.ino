#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);

float red_G, green_G, blue_G; // カラーセンサーで読み取ったRGB値（0-255）
int mode_G; // タスクのモードを表す状態変数
unsigned long timeInit_G, timeNow_G; //  スタート時間，経過時間
int motorR_G, motorL_G;  // 左右のZumoのモータに与える回転力
float Diff_bef = 0; //前回の偏差
float Diff_sum = 0;
void setup()
{
  Serial.begin(9600);
  Wire.begin();
  setupColorSensor(); // カラーセンサーのsetup

  button.waitForButton(); // Zumo buttonが押されるまで待機
  calibrationColorSensorWhite(); // カラーセンサーのキャリブレーション (white)
  button.waitForButton(); // Zumo buttonが押されるまで待機
  calibrationColorSensorBlack(); // カラーセンサーのキャリブレーション (black)

  mode_G = 0;
  button.waitForButton(); // Zumo buttonが押されるまで待機
  timeInit_G = millis();
  motorR_G = 0;
  motorL_G = 0;
}

void loop()
{
  readRGB(); // カラーセンサでRGB値を取得(0-255)
  timeNow_G = millis() - timeInit_G; // 経過時間
  motors.setSpeeds(motorL_G, motorR_G); // 左右モーターへの回転力入力
  sendData(); // データ送信
  //linetrace_bang_bang(); // ライントレース（bang-bang制御）
  linetrace_P(); //ライントレース(P制御)
  //task_A();
  //task_B();
  if (button.isPressed() ){ //Zumoボタンが押されたら
    mode_G = 0; 
    motors.setSpeeds(0,0);
    delay(200);  
  }
}

// 通信方式２
void sendData()
{
  static unsigned long timePrev = 0;
  static int inByte = 0;

  // 50msごとにデータ送信（通信方式２），500msデータ送信要求なし-->データ送信．
  if ( inByte == 0 || timeNow_G - timePrev > 500 || (Serial.available() > 0 && timeNow_G - timePrev > 50)) { // 50msごとにデータ送信
    inByte = Serial.read();
    inByte = 1;

    Serial.write('H');
    Serial.write(mode_G);
    Serial.write((int)red_G );
    Serial.write((int)green_G );
    Serial.write((int)blue_G );

    timePrev = timeNow_G;
  }
}
