
#include <Wire.h>
#include <ZumoMotors.h>
#include <Pushbutton.h>
#include <LSM303.h>

ZumoMotors motors;
Pushbutton button(ZUMO_BUTTON);
LSM303 compass;
#define LINE_COLOR 'B'
#define SPEED 100 // Zumoのモータに与える回転力の基準値 
//#define ZUMO_NUM 1 //+ Zumo番号(1から3までの3台)
//#define ZUMO_NUM 2 //+ Zumo番号(1から3までの3台)
#define ZUMO_NUM 3 //+ Zumo番号(1から3までの3台)

//超音波センサ
const int trig = 2;     //TrigピンをArduinoの2番ピンに
const int echo = 4;     //EchoピンをArduinoの4番ピンに

unsigned  long count = 0;
unsigned long interval; //Echo のパルス幅(μs)
int distance;           //距離(cm)

float red_G, green_G, blue_G; // カラーセンサーで読み取ったRGB値（0-255）
int mode_G; // 各ゾーンでのモードを表す状態変数
unsigned long timeInit_G, timeNow_G; //  スタート時間，経過時間
int motorR_G, motorL_G;  // 左右のZumoのモータに与える回転力
static int countR,countG,countB;
float direction_G;
float mx=0, my=0, mz=0;
float ax=0, ay=0, az=0;

float Diff_sum;//偏差の合計
float Diff_bef;//前回の偏差

int n_zumo = 0; // 動作させるズーモ番号を格納   // 乾 追記11.24
int zflag = 0; // 一周終われば 1              // 乾 追記11.24
int sflag = 0;
int countZone;
int countCross;
//追加部分
int countTrace; //何回トレースしたか
char tracecolor[6] = {'A','A','A','A','A','A'} ; //トレースした色を格納するための変数,Aで初期化
bool flag_samecolor; //トレース済みかどうかのフラグ
float global_direction_G; //現在の角度
int drawing_flag;
int high,low;

//速度を求めるための変数を宣言
double velX, velY = 0;
double comVelX, comVelY = 0;

double comAccX,comAccY = 0;  
double gravityX, gravityY = 0;

const double gamma = 0.8;//lowpassfilter
uint32_t timer_vel;
double v = 0; //x-y軸の合成速度
double d = 0; //移動距離

void setup()
{
	Serial.begin(9600);
	Wire.begin();
	button.waitForButton();
	setupColorSensor(); // カラーセンサーのsetup
	calibrationColorSensorWhite(); // カラーセンサーのキャリブレーション
	button.waitForButton();
	calibrationColorSensorBlack(); // カラーセンサーのキャリブレーション

	setupCompass(); // 地磁気センサーのsetup
	compass.read();
	compass.m_min.x = compass.m.x; compass.m_max.x = compass.m_min.x+1;
	compass.m_min.y = compass.m.y; compass.m_max.y = compass.m_min.y+1;
	compass.m_min.z = compass.m.z; compass.m_max.z = compass.m_min.z+1;
	calibrationCompass(); // 地磁気センサーのキャリブレーション
	mode_G = 0;
	zflag = 0;
	n_zumo = 0;
 
	//button.waitForButton();
	timeInit_G = millis();
	motorR_G = 0;
	motorL_G = 0;
  countZone = 0;
  countCross  = 0;
  //追加部分
  countTrace = 0;
  flag_samecolor = false;
  drawing_flag = 0;
  global_direction_G = averageHeadingLP();
  timer_vel = micros();
}

void global_direction_G_func(){
  global_direction_G = averageHeadingLP();
}

void loop()
{
	
  readRGB(); // カラーセンサでRGB値を取得(0-255)
	direction_G = averageHeadingLP();
	 
	timeNow_G = millis() - timeInit_G; // 経過時間
	if(n_zumo == ZUMO_NUM || sflag == 1){  // zumo番号が一致していたらタスクを行う  乾 追記11.24
		task_B(); 
    global_direction_G_func();
	}
 
 if(button.isPressed()){
  sflag = 1;
 }
	
	motors.setSpeeds(motorL_G, motorR_G); // 左右モーターへの回転力入力
  /*/ //いらなそう
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
	*///
	sendData(); // データ送信
}

void sendData()
{
	static unsigned long timePrev = 0;

	// 50msごとにデータ送信（通信方式２），500msデータ送信要求なし-->データ送信．
	if ( n_zumo == 0 ||
			timeNow_G - timePrev > 500 ||
			(Serial.available() > 0 && timeNow_G - timePrev > 50)) { // 50msごとにデータ送信

		n_zumo = Serial.read();  // 動作させるzumoの番号を受信

		Serial.write('H');
		Serial.write(((int)mode_G)&255);

		Serial.write(((int)red_G)&255);
		Serial.write(((int)green_G)&255);
		Serial.write(((int)blue_G)&255);
    Serial.write(((int)countCross)&255);
		/*
		Serial.write(((int)(ax+128))&255);
		Serial.write(((int)(ay+128))&255);
		Serial.write(((int)(az+128))&255);

		Serial.write(((int)(mx+128))&255);
		Serial.write(((int)(my+128))&255);
	  Serial.write(((int)(mz+128))&255);
    */
		Serial.write(((int)countR)&255);
		Serial.write(((int)countG)&255);
		Serial.write(((int)countB)&255);

		Serial.write(((int)(motorL_G/2+128))&255); 
		Serial.write(((int)(motorR_G/2+128))&255);
		Serial.write(((int)zflag)&255);

    high =  ((int)global_direction_G) >> 8;
    low  = ((int)global_direction_G) & 255;
    Serial.write(high); //現在の角度(グローバル座標を基準)
    Serial.write(low); //現在の角度(グローバル座標を基準)
    //Serial.write((int)global_direction_G); //現在の角度(グローバル座標を基準)
    Serial.write(((int)d)&255); //移動距離
    Serial.write(drawing_flag); //図形を描画中かどうかを示す．

		timePrev = timeNow_G;
	}
} 
