//
// Zone 3 mode_G = 10から利用
//
#define LINE_COLOR 'B'

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
void zone3()
{
  int done = 0;
  float speed0, diff, et; // et:偏差 
  static float diff_I; // diff_I:偏差の合計
  int r;  //r:目標値 
  static int taskBcount = 0;

  avex = 0.9*avex + 0.1*compass.a.x;

  switch ( mode_G ) {
    case 10: // setupはここ
      speed0 = 0;
      diff = 0;
      diff_I = 0;
      et = 0;
      r = 8000; // 山腹周回の目標値設定はここ
      mode_G = 15;
      break;
    case 11: // ゾーンの中心方向に向く
      speed0 = 0;
      if(LINE_COLOR=='B') diff = -150; // 要調整
      else diff = 150;
      done = waitfor(100*9*PI/4);
      if(done==1){
        diff = 0;
        mode_G = 12;
      }
      break;
    case 12: // 山探し(ここでは直進)
      speed0 = 150;
      diff = -0.02*compass.a.y; // P-制御
      if (avex>4000) { // 登り始めたら
        mode_G = 13;
      }
      break;
    //////////////////////////// 山腹周回(登り・下り共通) /////////////////////////////
                                                                                  //
                                                                                  //
    case 13: // 山腹まで移動
      speed0 = 150;
      diff = -0.02*compass.a.y; // P-制御
      done = waitfor(2000);
      if(done==1)
        mode_G = 14;
      break;
    case 14: // 右に90度回転して山腹周回の姿勢に入る
      speed0 = 0;
      diff = 150;
      done = waitfor(100*9*PI/4);
      if(done==1)
        mode_G = 8;
      break;
    case 15: // 山腹周回(1/4以上周回が必要な為、余裕をみて1/3周回する)
      speed0 = 150;
      et = r - compass.a.x;
      diff = -0.02*et ;//-0.002*diff_I; // PI-制御 r,0.02,0.002 要調整
      diff_I += et;
      if(diff_I > 100000) diff_I = 100000;
      if(diff_I < -100000) diff_I = -100000;
      done = waitfor(3000); // 1/3周するのに必要な時間 要調整
      if(done==1)
        mode_G = 15; //16
      break;
    case 16: // 左に90度回転
      speed0 = 0;
      diff = -150;
      done = waitfor(100*9*PI/4);
      if(done==1){
        taskBcount++;
        if(taskBcount == 1)
          mode_G = 17;
        else
          mode_G = 21;
      }
      break;
                                                                                  //
                                                                                  //
    //////////////////////////// 山腹周回(登り・下り共通) /////////////////////////////
    
    case 17: // 山登り中・頂上へ移動
      speed0 = 150;
      diff = -0.02*compass.a.y; // P-制御
      if ((avex<1500)&&(avex>-1500)) {  // 登頂したら頂上の中心部まで進む
        done = waitfor(100);        // 100ms は要調整
      }
      if (done == 1) {  // 頂上の中心部まで来たら停止
        speed0 = 0;
        diff = 0;
        mode_G = 18;
      }
      break;
    case 18: // 1000ms停止
      speed0 = 0;
      diff = 0;
      done = waitfor(1000);
      if ( done == 1 ){
        mode_G = 19;
      }
      break;
    case 19:  // 登ってきた方向へ 回れ右(180度回転)
      speed0 = 0;
      diff = 150;
      done = waitfor(100*9*PI/2);
      if(done==1)
        mode_G = 20;
      break;
    case 20: // 山下り
      speed0 = 150;
      diff = -0.02*compass.a.y; // P-制御 (直進)
      if (avex<-4000) { // 下り始めたら
        mode_G = 13;// ここで再度山腹周回を行う
      }
      break;
    case 21: // 山下り中
      speed0 = 150;
      diff = -0.02*compass.a.y; // P-制御
      if ((avex<1500)&&(avex>-1500)) {  // 下り終えたら山麓に触れないように少し進む
        done = waitfor(1000);        // 500ms は要調整
      }
      if (done == 1) {  // 山麓に触れない位置まで来たら停止
        speed0 = 0;
        diff = 0;
        mode_G = 22;
      }
      break;
    /////////////////////////// 出口へ進みコース周回へ戻る /////////////////////////////
                                                                                  //
                                                                                  //
    case 22: // 出口の方角から反時計回りに45~90度の方角を向く
      speed0 = 0;
      // Zone3
      if(LINE_COLOR=='B') diff = turnTo(0); // 角度は要調整
      else diff = turnTo(270);
      /* Zone1,2では↑(Zone3~)を消して↓を使う
      // Zone1
      diff = turnTo(225); // 角度は要調整
      // Zone2
      diff = turnTo(315); // 角度は要調整
      */
      if (abs(diff)<=50) {
        diff = 0;
        mode_G = 23;
      }
      break;
    case 23: // 黒線が見つかるまで直進
      speed0 = 150;
      diff = 0;
      if(red_G<30 && green_G<30 && blue_G<30) // 数値は要調整
        mode_G = 24;
      break;
    case 24: // 白色が少し見えるようになるまで右回転
      speed0 = 0;
      diff = 100;
      if(red_G>60 && green_G>60 && blue_G>60) // 数値は要調整
        mode_G = 25;
      break;
    case 25: // 出口を示す色が見つかるまで黒線をライントレース
      speed0 = 0;
      diff = 0;
      linetrace_P();
      if(identify_RGB()==LINE_COLOR) // LINE_COLORの部分には出口を示す色(RGB)を入れる
        // 出口を示す色が見つかったら
        mode_G = 26;
      break;
    case 26: // コースの方向を向く
      speed0 = 0;
      if(LINE_COLOR=='B') diff = turnTo(45);  // 角度は要調整
      else diff = turnTo(315);
      if (abs(diff)<=50) {
        diff = 0;
        mode_G = 27;
      }
      break;
    case 27: // コースのラインへ戻る
      speed0 = 100;
      diff = 0;
      // Zone3
      done = waitfor(700); // 秒数は要調整
      /* Zone1,2では↑(Zone3~)を消して↓を使う
      // Zone1
      if(LINE_COLOR=='B')
        done = waitfor(700); // 秒数は要調整
      else
        done = waitfor(1400); // 秒数は要調整
      // Zone2
      if(LINE_COLOR=='G')
        done = waitfor(700); // 秒数は要調整
      else
        done = waitfor(1400); // 秒数は要調整
      */
      if(done==1)
        mode_G = 28;
      break;
    case 28: // コースの進行方向を向く
      speed0 = 0;
      // Zone3
      diff = 100;
      if(LINE_COLOR=='B') diff *= -1;
      /* Zone1,2では↑(Zone3~)を消して↓を使う
      // Zone1
      diff = -100;
      // Zone2
      diff = 100;
      */
      done = waitfor(100*9*PI/4);
      if(done==1)
         mode_G = 29;
      break;
    case 29: // ライントレースでコースを少しだけ進んでおく
      speed0 = 0;
      diff = 0;
      linetrace_P();
      done = waitfor(1000); // 秒数は要調整
      if(done==1)
        mode_G = 1;
      break;
                                                                                  //
                                                                                  //
    /////////////////////////// 出口へ進みコース周回へ戻る /////////////////////////////
    
    case 99: // 停止
      speed0 = 0;
      diff = 0;
      sflag = 0;
      zflag = 1;
      break;
    default:
      break;
  }
  if(mode_G != 25){ // mode25ではlinetrace_P()内に記述済
    motorL_G = speed0 + diff;
    motorR_G = speed0 - diff;
    // 回転速度が200を超えないように
    if(motorL_G > 200) motorL_G = 200;
    if(motorL_G < -200) motorL_G = -200;
    if(motorR_G > 200) motorR_G = 200;
    if(motorR_G < -200) motorR_G = -200;
  }
}

bool within(int d){ // 前方d(cm)以内に物体があればTrue,なければFalseを返す
  digitalWrite(trig, HIGH); //10μsのパルスを超音波センサのTrigピンに出力
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  interval = pulseIn(echo, HIGH, 10068);  //Echo信号がHIGHである時間(μs)を計測
  distance = 340 * interval / 10000 / 2;  //距離(cm)に変換
  if (distance <= 10 && distance > 0) {   //距離が10cm以内ならTrue
    return true;
  }else
    return false;
}

int waitfor( unsigned long period )
{
  static int flagStart = 0; // 0:待ち状態，1:現在計測中
  static unsigned long startTime = 0;

  if ( flagStart == 0 ) { 
    startTime = timeNow_G;
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
