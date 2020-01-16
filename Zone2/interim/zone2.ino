void modeZone2(){
  static unsigned long startTime; // static変数，時間計測ははunsigned long
  static unsigned long EndTimer;//入ってから出るまでの時間
  char color;
  static unsigned long TimerNow;
  static unsigned long TimerBefore;
  static unsigned long TimerAverage;
  static int countTime;
  static int BeforeMode;
  
  static int Cflag;

  switch (mode_G){
    case 200://開始時初期化
      startTime = timeNow_G;
      EndTimer = timeNow_G;
      mode_G = 201;
      break;
    case 201://Zone2 探し中
      motorL_G = 150;
      motorR_G = -150;
      clash();
      if(mode_G == 203){//見つけた
        startTime = timeNow_G;
      }
      if(timeNow_G - startTime > 2000){
        startTime = timeNow_G;
        mode_G = 204;
      }
      if(timeNow_G - EndTimer > 60000 || cup > 100){
        mode_G = 220;
      }
      break;
    case 203://角度調整
      motorL_G = 150;
      motorR_G = -150;
      if(timeNow_G - startTime > 50){
        startTime = timeNow_G;
        mode_G = 204;
      }
      break;
    case 204://突進
      motorL_G = 150;
      motorR_G = 150;
      if(identify_RGB() == 'R'){//内円
        countTime++;
        TimerNow = timeNow_G - startTime;
        TimerBefore += TimerNow;
        TimerAverage = TimerBefore / countTime;
        startTime = timeNow_G;
        mode_G = 205;
        if(Judge_Catch() == 1){
          Cflag = 1;
        }
      }else if(identify_RGB() == 'K'){//外円
        countTime++;
        TimerNow = timeNow_G - startTime;
        TimerBefore += TimerNow;
        TimerAverage = TimerBefore / countTime;
        startTime = timeNow_G;
        mode_G = 207;
        if(Judge_Catch() == 1){
          Cflag = 1;
          mode_G = 206;
        }
      }
      break;
    case 205://後退
      motorL_G = -150;
      motorR_G = -150;
      if(timeNow_G - startTime > 300){
        startTime = timeNow_G;
        mode_G = 207;
      }
      if(Cflag == 1){
        cup = cup * 10;
        cup = cup + 1;
        Cflag = 0;
      }
      break;
    case 206://反転
      motorL_G = 150;
      motorR_G = -150;
        if(timeNow_G - startTime > 500 + TimerNow/3){
          startTime = timeNow_G;
          mode_G = 201;
          if(Judge_Catch() == 0){
            cup = cup * 10;
            cup = cup + 2;
          }
       }
      break;
    case 207://持ってない時
      motorL_G = 150;
      motorR_G = -150;
        if(timeNow_G - startTime > 500){
          startTime = timeNow_G;
          mode_G = 201;
       }
       break;
    case 220://外枠探し
      motorL_G = 150;
      motorR_G = 150;
        if(identify_RGB() == 'R'){
          startTime = timeNow_G;
          mode_G = 221;
        }else if(identify_RGB() == 'K'){
          startTime = timeNow_G;
          mode_G = 230;
        }
      break;
    case 221://一時後退
      motorL_G = -150;
      motorR_G = -150;
      if(timeNow_G - startTime > 300){
        startTime = timeNow_G;
        mode_G = 222;
      }
      break;
    case 222://一時反転
      motorL_G = 150;
      motorR_G = -150;
      if(timeNow_G - startTime > 1000){
        mode_G = 220;
      }
      break;
    case 230://ライン上にロボの体勢を合わせる
      motorL_G = -150;
      motorR_G = 150;
      if(timeNow_G - startTime > 500){
        mode_G = 231;
      }
      break;
    case 231://外枠をライントレース
      linetrace_P();
      if(identify_RGB() == 'R'){
        startTime = timeNow_G;
        mode_G = 232;
      }
      break;
    case 232://コースに復帰
      motorL_G = 175;
      motorR_G = -175;
      if(timeNow_G - startTime > 500){
        motorL_G = 150;
        motorR_G = 150;
        if(identify_RGB() == 'B'){
          startTime = timeNow_G;
          mode_G = 233;
        }
      }
      break;
    case 233://コースにロボの体勢を合わせる
      motorL_G = 100;
      motorL_G = 100;
      if(timeNow_G - startTime >500){
        motorL_G = 175;
        motorR_G = -175;
        if(timeNow_G - startTime >1000){
          mode_G = 1;
        }
      }
      break;
  }
}
