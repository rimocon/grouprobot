void linetrace_bang_bang()
{
  static float lightMin = 0; // 各自で設定
  static float lightMax = 255; // 各自で設定
  static float speed = 100; // パラメーター
  static float Kp = 2.0; // パラメーター
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
  static float speed = 200;
  static float Kp = 1.8;//P制御の比例定数
  static float Ki = 0.3;//I制御の比例定数
  static float Kd = 1.0;//D制御の比例定数
  float lightNow;
  float speedDiff;

  lightNow = (red_G + green_G + blue_G) / 3.0;//赤と緑と青のセンサの値の平均値を取る
  speedDiff = map(lightNow,lightMin,lightMax,-speed,speed);
  //speedDiff = map(blue_G,0,255,-speed,speed);
  Diff_sum += speedDiff;//現在の偏差を偏差の累積値としてみなす
  
  motorL_G = speed +Kp*speedDiff +Ki*speedDiff + Kd*(speedDiff - Diff_bef);
  motorR_G = speed -Kp*speedDiff -Ki*speedDiff - Kd*(speedDiff - Diff_bef);
  Diff_bef = speedDiff; 
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
      else if ( color == 'G' ){ // green
        stop_period = 500; // 後で停止する期間
        mode_G = 2;
      }
      else if ( color == 'C') { // cyan
        stop_period = 1500;
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
        countR++;
        mode_G = 2;
      }
      else if ( color == 'G' && countR > 0) { // green
        countG++;
        mode_G = 2;
      }
      break;

    case 2:
      linetrace_P(); // ライントレース
      if ( identify_RGB() == 'B' ) { // brue
        startTime = timeNow_G; // mode_G=3に遷移した時刻を記録
        if(countR < 2){
          mode_G = 1;
        }
        if(countR >= 2 &&countG < 1){
          mode_G = 3;
        }
        if(countR >= 2 &&countG >=1){
          mode_G = 4;
        }
      }
      break;
    case 3:
      linetrace_P(); // ライントレース
      motors.setSpeeds(0,0);
      if(timeNow_G - startTime > 1000){
        countR = 0;
        mode_G = 1;
      }
      break;
    case 4:
      countR = 0;
      countG = 0;
      mode_G = 1;
      break;
    case 5:
      startTime = timeNow_G;
      mode_G = 6;
    case 6:
      linetrace_P();//ライントレース
      motors.setSpeeds(0,0);
      if(timeNow_G - startTime > 10){
        mode_G = 2;
      }
      break;
    
  }
}

void task_C(){
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
        countR++;
        mode_G = 2;
      }
      else if ( color == 'B' && countR > 0) { // green
        countG++;
        mode_G = 2;
      }
      break;

    case 2:
      linetrace_P(); // ライントレース
      if ( identify_RGB() == 'G' ) { // brue
        startTime = timeNow_G; // mode_G=3に遷移した時刻を記録
        if(countR < 2){
          mode_G = 1;
        }
        if(countR >= 2 &&countG < 1){
          mode_G = 3;
        }
        if(countR >= 2 &&countG >=1){
          mode_G = 4;
        }
      }
      break;
    case 3:
      linetrace_P(); // ライントレース
      motors.setSpeeds(100,100);
      /*if(identify_RGB() == 'C'){
        countR = 0;
        mode_G = 4;
      }else */if(timeNow_G - startTime > 5000){
        countR = 0;
        mode_G = 1;
      }
      break;
    case 4:
      linetrace_P();//ライントレース
      motors.setSpeeds(150,150);
      if(identify_RGB() == 'G'){
        countR = 0;
        mode_G = 1;
      }
      break;
    case 5:
      countR = 0;
      countG = 0;
      mode_G = 1;
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
  else if ( blue_G > alpha * red_G && green_G > alpha * red_G)
    return 'C';
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
