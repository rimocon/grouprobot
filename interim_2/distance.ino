void avoidance(){//衝突回避
  digitalWrite(trig, HIGH); //10μsのパルスを超音波センサのTrigピンに出力
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  interval = pulseIn(echo, HIGH, 10068);  //Echo信号がHIGHである時間(μs)を計測
  distance = 340 * interval / 10000 / 2;  //距離(cm)に変換
  if (distance < 20 && distance > 0) {          //距離が10cm未満なら停止
    mode_G = 5;
  }else if(distance >= 20 && mode_G == 6){
    mode_G = 2;
  }
}
