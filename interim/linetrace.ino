void linetrace_P(){
  static float lightMin = 0;
  static float lightMax = 255;
  static float speed = 200; //
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
/*
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
      else if ( color == 'B') { // blue
        mode_G = 99;
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
*/
void task_B(){
  static int stop_period; // static変数であることに注意
  static unsigned long startTime; // static変数，時間計測ははunsigned long
  char color;

  switch ( mode_G ) {
    case 0://待機モード
      mode_G = 1;
      break;  // break文を忘れない（忘れるとその下も実行される）

    case 1://通常走行
      linetrace_P(); // ライントレース（各自で作成）
      startTime = timeNow_G;
      if(timeNow_G - startTime > 1000){
        mode_G = 99;
      }
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

    case 2://赤、緑を検出
      linetrace_P(); // ライントレース
      if ( identify_RGB() == 'B' ) { // brue
        startTime = timeNow_G; // mode_G=3に遷移した時刻を記録
        if(countR < 2){//再度通常走行
          mode_G = 1;
        }
        if(countR >= 2 &&countG < 1){//交差点
          mode_G = 3;
        }
        if(countR >= 2 &&countG >=1){//ゾーンに入った
          mode_G = 4;
        }
      }
      break;
    case 3://交差点
      linetrace_P(); // ライントレース
      motors.setSpeeds(0,0);
      if(timeNow_G - startTime > 1000){
        countR = 0;
        mode_G = 1;
      }
      break;
    case 4://各ゾーンでの行動
      motors.setSpeeds(200,-200);//右回転
      if(timeNow_G - startTime >500){
        motors.setSpeeds(-200,200);//左回転
        if(timeNow_G - startTime > 1500){
          motors.setSpeeds(200,-200);//右回転
          if(timeNow_G - startTime > 2000){
            countR = 0;
            countG = 0;
            mode_G = 1;
          }
        }
      }
      break;
    case 5://時間を取得
      startTime = timeNow_G;
      motors.setSpeeds(0,0);
      mode_G = 6;
      break;
    case 6://衝突回避
      linetrace_P();//ライントレース
      motors.setSpeeds(0,0);
      if(timeNow_G - startTime > 10){
        mode_G = 2;
      }
      break;
    case 98:
      motorR_G = 0;
      motorL_G = 0;
      break;
    case 99:
      motorR_G = 0;
      motorL_G = 0;
      sendData();
      mode_G = 98;
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
