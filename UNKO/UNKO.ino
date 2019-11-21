#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;

#define ZUMO_NUM 1

//超音波センサ
const int trig = 2;     //TrigピンをArduinoの2番ピンに
const int echo = 4;     //EchoピンをArduinoの4番ピンに

unsigned long interval; //Echo のパルス幅(μs)
int distance;           //距離(cm)


//カラーセンサ
float red_G, green_G, blue_G; // カラーセンサーで読み取ったRGB値（0-255）
int mode_G; // タスクのモードを表す状態変数
unsigned long timeInit_G, timeNow_G; //  スタート時間，経過時間
int motorR_G, motorL_G;  // 左右のZumoのモータに与える回転力
static int countR,countG,countB;
float direction_G;
float mx=0, my=0, mz=0;
float ax=0, ay=0, az=0;

float Diff_sum;//偏差の合計
float Diff_bef;//前回の偏差

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  setupColorSensor(); // カラーセンサーのsetup

  button.waitForButton(); // Zumo buttonが押されるまで待機
  calibrationColorSensorWhite(); // カラーセンサーのキャリブレーション (white)
  button.waitForButton(); // Zumo buttonが押されるまで待機
  calibrationColorSensorBlack(); // カラーセンサーのキャリブレーション (black)

  pinMode(trig, OUTPUT);  //trigを出力ポートに設定
  pinMode(echo, INPUT);   //echoを入力ポートに設定

  mode_G = 0;
  button.waitForButton(); // Zumo buttonが押されるまで待機
  timeInit_G = millis();
  motorR_G = 0;
  motorL_G = 0;
}

void loop()
{
  readRGB(); // カラーセンサでRGB値を取得(0-255)
  direction_G = averageHeadingLP();
  timeNow_G = millis() - timeInit_G; // 経過時間
  //linetrace_bang_bang(); // ライントレース（bang-bang制御）
  linetrace_P();
  avoidance();
  task_B();
  if(button.isPressed()){//Zumoボタンが押されたら
    mode_G = 0;
    motors.setSpeeds(0,0);
    delay(200);
    button.waitForButton();//Zumoボタンが押されるまで待つ
  }
  motors.setSpeeds(motorL_G, motorR_G); // 左右モーターへの回転力入力
  ax = compass.a.x;  ay = compass.a.y;  az = compass.a.z;
  ax = map(ax,-32768,32768,-128,127);
  ay = map(ay,-32768,32768,-128,127);
  az = map(az,-32768,32768,-128,127);
  
  mx = compass.m.x;  my = compass.m.y;  mz = compass.m.z;
  compass.m_min.x=min(compass.m_min.x,mx); compass.m_max.x=max(compass.m_max.x,mx);
  compass.m_min.y=min(compass.m_min.y,my); compass.m_max.y=max(compass.m_max.y,my);
  compass.m_min.z=min(compass.m_min.z,mz); compass.m_max.z=max(compass.m_max.z,mz);
  mx = map(mx,compass.m_min.x,compass.m_max.x,-128,127);
  my = map(my,compass.m_min.y,compass.m_max.y,-128,127);
  mz = map(mz,compass.m_min.z,compass.m_max.z,-128,127); 
  sendData(); // データ送信
  //processing用処理
   //+ 3台のZumoを連携させるための、Processingとの通信処理
  if(Serial.available() >= 2){//２つ以上のデータを受け取ったら
    if(Serial.read() == 'H' ){//先頭の文字が'H'なら
      byte num = Serial.read();//numに受け取った数字を格納
      if(num == ZUMO_NUM)      //送られてきた番号が自分の番号と一致していたら
        mode_G = 0;               //初期mode99からmode0に移行
    }
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
    Serial.write(((int)mode_G)&255);
    
    Serial.write(((int)red_G)&255);
    Serial.write(((int)green_G)&255);
    Serial.write(((int)blue_G)&255);

    Serial.write(((int)(ax+128))&255);
    Serial.write(((int)(ay+128))&255);
    Serial.write(((int)(az+128))&255);
    
    Serial.write(((int)(mx+128))&255);
    Serial.write(((int)(my+128))&255);
    Serial.write(((int)(mz+128))&255);
    
    Serial.write(((int)(motorL_G/2+128))&255); 
    Serial.write(((int)(motorR_G/2+128))&255); 

    timePrev = timeNow_G;
  }
}
