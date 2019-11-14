void avoidance(){//衝突回避
  digitalWrite(trig, HIGH); //10μsのパルスを超音波センサのTrigピンに出力
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  interval = pulseIn(echo, HIGH, 23068);  //Echo信号がHIGHである時間(μs)を計測
  distance = 340 * interval / 10000 / 2;  //距離(cm)に変換
  if (distance < 10 && distance > 0) {          //距離が10cm未満なら後退
    mode_G = 5;
  }
}
