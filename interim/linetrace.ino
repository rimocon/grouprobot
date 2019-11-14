void linetrace_bang_bang()
{
  static float lightMin = 0; // 各自で設定
  static float lightMax = 255; // 各自で設定
  static float speed = 100; // パラメーター
  static float Kp = 1.9; // パラメーター
  float lightNow;
  float speedDiff;

  lightNow = (red_G + green_G + blue_G ) / 3.0;
  if ( lightNow < (lightMin + lightMax) / 2.0 ) // 右回転
    speedDiff = -Kp * speed;
  else // 左回転
    speedDiff = Kp * speed;
  motorL_G = speed - speedDiff;
  motorR_G = speed + speedDiff;
}

void linetrace_P(){
  static float lightMin = 0;
  static float lightMax = 255;
  static float speed = 250; //
  static float Kp = 1.8; //P制御の比例定数
  static float Ki = 0.3; //I制御の比例定数
  static float Kd = 1.8; //D制御の比例定数

  //speed 250,Kp1.8,Ki0.3,Kd1.8で安定
  float lightNow;
  float Diff; //偏差
  
  lightNow = (red_G + green_G + blue_G ) / 3.0; //赤と緑と青のセンサの値の平均値を取る
  //Diff = map(lightNow,lightMin,lightMax,-speed,speed); //lightNowの値に応じてDiffを変更(白黒ライントレース)
  //Diff = map(green_G,dataG_min,dataG_max,-speed,speed); //green_Gの値に応じてDiffを変更(緑色ライントレース)
  Diff = map(blue_G,0,255,-speed,speed); //blue_Gの値に応じてDiffを変更(青色ライントレース)
  Diff_sum += Diff; //現在の偏差を偏差の累積値として足す
  /*右側トレース
  motorL_G = speed - Kp*Diff;  - Ki*Diff_sum - Kd*(Diff - Diff_bef); //PID制御
  motorR_G = speed + Kp*Diff;  + Ki*Diff_sum + Kd*(Diff - Diff_bef); //PID制御
  */
  motorR_G = speed - Kp*Diff;  - Ki*Diff_sum - Kd*(Diff - Diff_bef); //PID制御
  motorL_G = speed + Kp*Diff;  + Ki*Diff_sum + Kd*(Diff - Diff_bef); //PID制御
  Diff_bef = Diff; //現在の偏差を前回の値として格納
}


void task_A()
{
  static int stop_period; // static変数であることに注意
  static unsigned long startTime; // static変数，時間計測ははunsigned long
  char color;

  switch ( mode_G ) {
    case 0:
      mode_G = 1;
      break;  // break文を忘れない（忘れるとその下も実行される）

    case 1:
      linetrace_P(); // ライントレース（各自で作成）
      color = identify_RGB(); // ラインの色を推定(R:赤，G:緑，B:青，-:それ以外）
      if ( color == 'R' ) { // red
        stop_period = 1000; // 後で停止する期間
        mode_G = 2;
      }
      else if ( color == 'G' ) { // green
        stop_period = 500; // 後で停止する期間
        mode_G = 2;
      }
      break;

    case 2:
      linetrace_P(); // ライントレース
      if ( identify_RGB() == 'B' ) { // brue
        startTime = timeNow_G; // mode_G=3に遷移した時刻を記録
        mode_G = 3;
      }
      break;

    case 3:
      motorL_G = 0; // 停止
      motorR_G = 0;
      if ( timeNow_G - startTime > stop_period ) // 指定時間経過したら
        mode_G = 1;
      break;
  }
}

void task_B(){
  static int run_period = 1000;; // static変数であることに注意
  static unsigned long startTime; // static変数，時間計測ははunsigned long
  static int green_count = 0;
  char color;

  switch ( mode_G ) {
    case 0:
      mode_G = 1;
      break;  // break文を忘れない（忘れるとその下も実行される）

    case 1:
      linetrace_P(); // ライントレース（各自で作成）
      color = identify_RGB(); // ラインの色を推定(R:赤，G:緑，B:青，-:それ以外）
      if ( color == 'R' ) { // red //一回目の赤
          mode_G = 2;
        }
      break;
    case 2:
      linetrace_P();
      color = identify_RGB();
      if( color == 'B' ) {
        mode_G = 3;
      }
      break;
    case 3: 
      linetrace_P(); // ライントレース
      color = identify_RGB();
      if ( color == 'R' ) { //２回目も赤だったら
        startTime = timeNow_G; // mode_G=3に遷移した時刻を記録
        mode_G = 4;
      }
      if ( color == 'G' ) { //2回目が緑だったら
          green_count++;
          mode_G = 5;
      }
      break;
    
    case 4:
      motorL_G = 100;
      motorR_G = 100;
      if ( timeNow_G - startTime > run_period ) // 指定時間経過したら
       mode_G = 1;
      break;
    case 5:
      linetrace_P();
      color = identify_RGB();
      if (color == 'B') {
        mode_G = 6;
      }
      break;
    case 6:
      linetrace_P();
      color = identify_RGB();
      if( color  == 'G') { //緑だったらカウント
        green_count++;
        mode_G=5;
      }
      if ( color == 'R') { //赤だったら終了
        motorL_G = -100;
        motorR_G = 100;
        delay(500);
        mode_G = 7;
      }
      break;
    case 7:
      color = identify_RGB();
      motors.setSpeeds(100,100);
      if( color == 'B') {
        mode_G = 8;
      }
      break;
    case 8:
      color = identify_RGB();
      motors.setSpeeds(100,100);
      if( color == 'W') {
        mode_G = 9;
      }
      break;
    case 9:
      delay(1000);
      for(int i = 0; i < green_count; i++) {
        motors.setSpeeds(100, 100); // 直進
        delay(500);
        motors.setSpeeds(0, 0); // 停止
        delay(500);
      }
      mode_G = 10;
      break;
    case 10:
      motors.setSpeeds(0,0);
      break;
  }
}
// lineの色の推定
char identify_RGB()
{
  float alpha = 1.2; // パラメーター

  if ( blue_G > alpha * red_G && blue_G > alpha * green_G ) // _*_Gはグローバル変数
    return 'B';
  else if ( red_G > alpha * blue_G && red_G > alpha * green_G )
    return 'R';
  else if ( green_G > alpha * red_G && green_G > alpha * blue_G )
    return 'G';
  else if ( green_G < 40 && red_G < 40 && blue_G < 40)
    return 'B';
  else if (green_G > 150 && red_G >150 && blue_G > 150) 
    return 'W';
  else
    return '-';
}

int identify_color( int red, int green, int blue )
{
  float d2;
  float d2_max = 50; // パラメーター（適宜調整）

  d2 = pow(red - red_G, 2) + pow(green - green_G, 2) + pow(blue - blue_G, 2);
  if ( d2 < d2_max * d2_max )
    return 1;
  else
    return 0;
}

int maintainState( unsigned long period )
{
  static int flagStart = 0; // 0:待ち状態，1:現在計測中
  static unsigned long startTime = 0;

  if ( flagStart == 0 ) {
    startTime = timeNow_G; // 計測を開始したtimeNow_Gの値を覚えておく
    flagStart = 1; // 現在計測中にしておく
  }

  if ( timeNow_G - startTime > period ) { // 計測開始からの経過時間が指定時間を越えた
    flagStart = 0; // 待ち状態に戻しておく
    startTime = 0; // なくても良いが，形式的に初期化
    return 1;
  }
  else
    return 0;
}
