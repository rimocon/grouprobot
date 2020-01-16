
void linetrace_P(){
  static float lightMin = 0;
  static float lightMax = 255;
  static float speed = 150;
  static float Kp = 0.8;//P制御の比例定数
  static float Ki = 0.3;//I制御の比例定数
  static float Kd = 0.3;//D制御の比例定数
  float lightNow;
  float speedDiff;

  lightNow = (red_G + green_G + blue_G) / 3.0;//赤と緑と青のセンサの値の平均値を取る
  speedDiff = map(lightNow,lightMin,lightMax-20,-speed,speed);
  //speedDiff = map(blue_G,0,255,-speed,speed);
  Diff_sum += speedDiff;//現在の偏差を偏差の累積値としてみなす
  
  motorL_G = speed +Kp*speedDiff +Ki*speedDiff + Kd*(speedDiff - Diff_bef);
  motorR_G = speed -Kp*speedDiff -Ki*speedDiff - Kd*(speedDiff - Diff_bef);
  Diff_bef = speedDiff; 
}

void linetrace_P2(){
  static float lightMin = 0;
  static float lightMax = 200;
  static float speed = 150;
  static float Kp = 0.5;//P制御の比例定数
  static float Ki = 0.3;//I制御の比例定数
  static float Kd = 0.5;//D制御の比例定数
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

void linetrace_P3() { //zone1用のトレース
  static float lightMin = 0;
  static float lightMax = 255;
  static float speed = 90;
  static float speedL;
  static float speedR;
  static float Kp = 1.3;//P制御の比例定数
  static float Ki = 0.7;//I制御の比例定数
  static float Kd = 0.9;//D制御の比例定数
  float lightNow;
  float speedDiff;
  float integral;
  
  count++;
  integral = Diff_sum / count;
  lightNow = (red_G + green_G + blue_G) / 3.0;//赤と緑と青のセンサの値の平均値を取る
  speedDiff = map(lightNow,lightMin,lightMax,-speed,speed);
  //speedDiff = map(blue_G,0,255,-speed,speed);
  Diff_sum += speedDiff;//現在の偏差を偏差の累積値としてみなす
  integral = Diff_sum / count;
  if(speedDiff > speed/2){ // 3/4白の時
    speedL = speed +Kp*(speedDiff+20) ;//+Ki*integral + Kd*(speedDiff - Diff_bef);
    speedR = -speed -Kp*(speedDiff+20) ;//-Ki*integral - Kd*(speedDiff - Diff_bef);
  }
  else if (speedDiff < -speed/2) { //3/4色の時
    speedL = -speed +Kp*(speedDiff -20) ;//+Ki*integral + Kd*(speedDiff - Diff_bef);
    speedR = speed -Kp*(speedDiff-20) ;//-Ki*integral - Kd*(speedDiff - Diff_bef);
  }
  else{
    speedL = speed +Kp*speedDiff +Ki*integral + Kd*(speedDiff - Diff_bef);
    speedR = speed -Kp*speedDiff -Ki*integral - Kd*(speedDiff - Diff_bef);
  }
  
  motorL_G = speedL;
  motorR_G = speedR;
  Diff_bef = speedDiff; 
}

void task_B(){
  static int stop_period; // static変数であることに注意
  static unsigned long startTime; // static変数，時間計測ははunsigned long
  static unsigned long redTimer;//赤色を取得して一定時間経ったらcountRをリセット
  static unsigned long traceTime; //トレースしきれなかった時用
  char color;


  switch ( mode_G ) {
    case 0://待機モード
      mode_G = 1;
      break;  // break文を忘れない（忘れるとその下も実行される）

    case 1://通常走行
      linetrace_P(); // ライントレース（各自で作成）
      color = identify_RGB(); // ラインの色を推定(R:赤，G:緑，B:青，-:それ以外）
      if(timeNow_G - redTimer > 5000){//一定時間経過で赤リセット
        countR = 0;
        countG = 0;
      }
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
      if ( identify_RGB() != 'R' && identify_RGB() != 'G') { // brue
        startTime = timeNow_G; // mode_G=3に遷移した時刻を記録
        if(countR < 2){//再度通常走行
          redTimer = timeNow_G;
          mode_G = 1;
        }else if(countG < 1){//交差点
          mode_G = 3;
          countCross++;
        }else if(countG >=1){//ゾーンに入った
          mode_G = 4;
        }
      }
      break;
    case 3://交差点
      linetrace_P2(); // ライントレース
      avoidance();
      if(timeNow_G - startTime > 1500){
        countR = 0;
        mode_G = 1;
        if(countZone >= 3){
          mode_G = 99;
        }
      }
      break;
    case 4://各ゾーンでの行動
     switch(countG){
      case 1: //zonebangou
      motorL_G = -150;//右回転
      motorR_G = 150;
      if(timeNow_G - startTime >500){
        startTime = timeNow_G;
        mode_G = 100;
      }
      break;
			default:
        countR = 0;
        countG = 0;
				countZone++;
        mode_G = 1;
				break;
     }
     break;
    case 5://衝突回避用の時間待機
      motorL_G = 0;
      motorR_G = 0;
      startTime = timeNow_G;
      mode_G = 6;
      break;
    case 6:
      motorL_G = 0;
      motorR_G = 0;
      if(timeNow_G - startTime > 200){
        avoidance();
      }
      break;
    case 99:
		  zflag = 1;
      sflag = 0;
      motorL_G = 0;
      motorR_G = 0;
      break;  
    case 100: //zone1の内容
      traceTime = timeNow_G;
      motorL_G = 150;
      motorR_G = 150;
      if(timeNow_G - startTime > 1000) mode_G = 101;
      break;  // break文を忘れない（忘れるとその下も実行される）
    case 101: //白色まで走る
    motorL_G = 100;
    motorR_G = 100;
    color = identify_RGB();
    
    if(color == 'W'){
      mode_G = 102;
    }
    break;
  case 102: //図形を探す
    motorL_G = 100;
    motorR_G = 100;
    color = identify_RGB();

    if( color != 'W'){ //白以外の時
      startTime = timeNow_G;
      if(color == 'K') {//黒色だったら回転
        mode_G = 103;
      }
      else if(color == '-'){
        mode_G = 102; //何もしない
      }
      else{//それ以外(三原色+中間色3つ)
        motorL_G = 0; //とりあえず止まらせる
        motorR_G = 0;
        for(int i = 0; i < countTrace+1; i++){
          if(color == tracecolor[i]) flag_samecolor = true;//すでにトレース済みフラグをあげる
        }
        if(flag_samecolor){ //トレース済みの色だったら
          mode_G = 101; //図形探しに戻る
          flag_samecolor = false; //フラグ下げ
        }
        else{ //トレースしたことがない色だったら
          tracecolor[countTrace] = color; //色を格納
          countTrace++; //カウントを増やす
          mode_G = 104; //トレースに移行
        }
       }
      }
    break;
  case 103: //回転用
    motorL_G = 150;
    motorR_G = -150;
    if(timeNow_G - startTime > 1000){ //1秒回転したら
      mode_G = 101; //図形探しに戻る
    }
    break;
  case 104: //図形トレース
    linetrace_P3();
    color = identify_RGB();
    if(timeNow_G - startTime > 15000){ //図形トレース終了
      if(countTrace == 5 || timeNow_G - traceTime >100000) mode_G = 105; //全ての図形をトレースし終わるか時間切れだったら(100秒)
      else mode_G = 101; //まだトレースしきってなかったら図形探しを続行
    }
    if(color == 'K'){ //黒色の時
      startTime = timeNow_G;
      mode_G = 103; //回転の戻る
    }
    break;
  case 105: //黒線見つかるまで直進
    motorL_G = 150;
    motorR_G = 150; //直進
    color = identify_RGB();
    if(color == 'K'){
      startTime = timeNow_G;
      mode_G = 106;
    }
    break;
  case 106: //向き統一のため少し回転
    motorL_G = -150;
    motorR_G = 150;
    if(timeNow_G - startTime > 300) mode_G = 107;
    break;
  case 107: //赤色見つけるまでトレース
    linetrace_P();
    color = identify_RGB();
    if(color == 'R') {
      startTime = timeNow_G;
      mode_G = 108; //出口を指す色が見つかったら
    }
   break;
  case 108: //ちょっと直進
    linetrace_P();
    if(timeNow_G - startTime >500 ) {
      startTime = timeNow_G;
      mode_G = 109; 
    }
    break;
  case 109:
    motorL_G = 150;
    motorR_G = -150;
    if(timeNow_G - startTime > 500){ //45度回転
      startTime = timeNow_G;
      mode_G = 110;
    }
    break;
  case 110:
    motorL_G = 150;
    motorR_G = 150;
    color = identify_RGB();
    if(color == LINE_COLOR) {
      startTime = timeNow_G;
      mode_G = 111;
    }
    else{
      mode_G = 110; //他の色だったら直進
    }
    break;
  case 111:
    motorL_G = -150;
    motorR_G = 150;
    if(timeNow_G - startTime > 500){
      mode_G = 1;
    }
    break;
  }
}

/*
void task_C() {
  static unsigned long startTime; // static変数，時間計測はunsigned long
  char color;
  static unsigned long traceTime; //トレースしきれなかった時用
  
  switch ( mode_G ) {
  case 0://待機モード
    mode_G = 1;
    traceTime = timeNow_G;
    break;  // break文を忘れない（忘れるとその下も実行される）
  case 1: //白色まで走る
    motorL_G = 100;
    motorR_G = 100;
    color = identify_RGB();
    
    if(color == 'W'){
      mode_G = 2;
    }
    break;
  case 2: //図形を探す
    motorL_G = 100;
    motorR_G = 100;
    color = identify_RGB();

    if( color != 'W'){ //白以外の時
      startTime = timeNow_G;
      if(color == 'K') {//黒色だったら回転
        mode_G = 3;
      }
      else if(color == '-'){
        mode_G = 2; //何もしない
      }
      else{//それ以外(三原色+中間色3つ)
        motorL_G = 0; //とりあえず止まらせる
        motorR_G = 0;
        for(int i = 0; i < countTrace+1; i++){
          if(color == tracecolor[i]) flag_samecolor = true;//すでにトレース済みフラグをあげる
        }
        if(flag_samecolor){ //トレース済みの色だったら
          mode_G = 1; //図形探しに戻る
          flag_samecolor = false; //フラグ下げ
        }
        else{ //トレースしたことがない色だったら
          tracecolor[countTrace] = color; //色を格納
          countTrace++; //カウントを増やす
          mode_G = 4; //トレースに移行
        }
       }
      }
    break;
  case 3: //回転用
    motorL_G = 150;
    motorR_G = -150;
    if(timeNow_G - startTime > 1000){ //1秒回転したら
      mode_G = 1; //図形探しに戻る
    }
    break;
  case 4: //図形トレース
    linetrace_P3();
    color = identify_RGB();
    if(timeNow_G - startTime > 5000){ //図形トレース終了
      if(countTrace == 5 || timeNow_G - traceTime >20000) mode_G = 5; //全ての図形をトレースし終わるか時間切れだったら(20秒)
      else mode_G = 1; //まだトレースしきってなかったら図形探しを続行
    }
    if(color == 'K'){ //黒色の時
      startTime = timeNow_G;
      mode_G = 3; //回転の戻る
    }
    break;
  case 5: //黒線見つかるまで直進
    motorL_G = 150;
    motorR_G = 150; //直進
    color = identify_RGB();
    if(color == 'K'){
      startTime = timeNow_G;
      mode_G = 6;
    }
    break;
  case 6: //向き統一のため少し回転
    motorL_G = -150;
    motorR_G = 150;
    if(timeNow_G - startTime > 300) mode_G = 7;
    break;
  case 7: //赤色見つけるまでトレース
    linetrace_P();
    color = identify_RGB();
    if(color == 'R') {
      startTime = timeNow_G;
      mode_G = 8; //出口を指す色が見つかったら
    }
   break;
  case 8: //ちょっと直進
    linetrace_P();
    if(timeNow_G - startTime >500 ) {
      startTime = timeNow_G;
      mode_G = 9; 
    }
    break;
  case 9:
    motorL_G = 150;
    motorR_G = -150;
    if(timeNow_G - startTime > 500){ //45度回転
      startTime = timeNow_G;
      mode_G = 10;
    }
    break;
  case 10:
    motorL_G = 150;
    motorR_G = 150;
    color = identify_RGB();
    if(color == LINE_COLOR) {
      startTime = timeNow_G;
      mode_G = 11;
    }
    else{
      mode_G = 10; //他の色だったら直進
    }
    break;
  case 11:
    motorL_G = -150;
    motorR_G = 150;
    if(timeNow_G - startTime > 500){
      mode_G = 12;
    }
    break;
  case 12:
    linetrace_P();
    break;
  }
}
*/
// lineの色の推定
char identify_RGB()
{
  float alpha = 1.2; // パラメーター

  if ( blue_G > alpha * red_G && blue_G > alpha * green_G && blue_G > 30) // ブルー
    return 'B';
  else if ( red_G > alpha * blue_G && red_G > alpha * green_G && red_G >30) //レッド
    return 'R';
  else if ( green_G > alpha * red_G && green_G > alpha * blue_G  && green_G > 30) //グリーン
    return 'G';
  else if ( blue_G > alpha * red_G && green_G > alpha * red_G) //シアン
    return 'C';
  else if ( red_G > alpha * green_G && blue_G > alpha * green_G) //マジェンタ
    return 'M';
  else if ( green_G > alpha * blue_G && red_G > alpha * blue_G) //イエロー
    return 'Y';
  else if ( (blue_G + red_G + green_G)/3 > 200) //白
    return 'W';
  else if ( (blue_G + red_G + green_G)/3 < 50) //黒
    return 'K';
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

float turnTo(float dir) {
  float heading, diff;

  heading = direction_G;
  if (heading>180) heading -= 360;
  heading -= dir;
  if (heading>180) heading -= 360;
  if (heading<-180) heading += 360;
  diff = -heading*5;            // P-制御
  if (diff>200) diff = 200;     // 飽和
  if (diff<-200) diff = -200;   // 飽和
  return diff;
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
