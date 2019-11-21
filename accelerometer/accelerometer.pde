//更新日時 10/31/授業中

import processing.serial.*;
Serial port1;
Serial port2;
Serial port3;
int n_zumo = 1;
int count=0;
int count_mode = 0;
int LF = 10;                                                    // LF（Linefeed）のアスキーコード
int height_g, width_g;           // RGBgraphを描画する高さと横幅

int ax = 0, ay = 0, az = 0;
int mx = 0, my = 0, mz = 0;
int CX=250, CY=250;
int mode_G=0;  //
int red_G=0, green_G=0, blue_G=0, green_p=0, red_p=0, blue_p=0;  //
int motorR_G=0, motorL_G=0;  //

void setup() {
   size(1600,1000);
   width_g = 1200; height_g = width_g/3; 
   background(255);
   count = 0;
   red_p = 0; 
   green_p = 0; 
   blue_p = 0;
   count_mode = 1;
  port1 = new Serial(this, "/dev/ttyUSB0", 9600); //Serial クラスのインスタンスを生成
  port1.clear();
  //port1.bufferUntil(0x0d);                                      // LF = 0x0d までバッファ
  //port2 = new Serial(this, "/dev/ttyUSB1", 9600); //Serial クラスのインスタンスを生成
  //port2.clear();
  //port2.bufferUntil(0x0d);                                      // LF = 0x0d までバッファ
  //port3 = new Serial(this, "/dev/ttyUSB2", 9600); //Serial クラスのインスタンスを生成
  //port3.clear();
  //port3.bufferUntil(0x0d);                                      // LF = 0x0d までバッファ
    sendZumoData();
}

void line3D(float x0, float y0,float z0,float x1,float y1,float z1) {
  float X0 = CX+y0-0.5*x0, Y0 = CY + 1.7320508*x0/2-z0;
  float X1 = CX+y1-0.5*x1, Y1 = CY + 1.7320508*x1/2-z1;
  line(X0,Y0,X1,Y1);
}

void drawVec(float x, float y, float z) {
  stroke(128);
  line3D(0,0,0,250,0,0);  line3D(0,0,0,0,250,0);  line3D(0,0,0,0,0,250);
  stroke(0);
  line3D(0,0,0,x,y,0);
  line3D(x,y,0,x,y,z);
  line3D(0,0,0,x,0,0);
  line3D(x,0,0,x,y,0);
  line3D(x,y,0,0,y,0);
  line3D(0,y,0,0,0,0);
  stroke(255,0,0);  line3D(0,0,0,x,y,z);
  fill(0);  text(x,CX-80,490);  text(y,CX,490);  text(z,CX+80,490);
}

void drawRGBgraph(){
  float y_p, y;
  
  stroke(0);
  strokeWeight(3);
  noFill();
  rect(0,0,width_g,height_g);

  stroke(200, 200, 200);
  strokeWeight(1);
  line(0, height_g*0.1, width_g, height_g*0.1);
  line(0, height_g*0.9, width_g, height_g*0.9);

  y_p = map(red_p, 0, 255, height_g*0.9, height_g*0.1);
  y = map(red_G, 0, 255, height_g*0.9, height_g*0.1);
  stroke(255, 0, 0);
  line((count-1)*10, y_p, (count)*10, y );

  y_p = map(green_p, 0, 255, height_g*0.9, height_g*0.1);
  y = map(green_G, 0, 255, height_g*0.9, height_g*0.1);
  stroke(0, 255, 0);
  line((count-1)*10, y_p, (count)*10, y );


  y_p = map(blue_p, 0, 255, height_g*0.9, height_g*0.1);
  y = map(blue_G, 0, 255, height_g*0.9, height_g*0.1);
  stroke(0, 0, 255);
  line((count-1)*10, y_p, (count)*10, y );
  
  if ( count == width_g/10 ) {
    count = 0;
    noStroke();  fill(255);  rect(0,0,width_g,height_g);
  }
}

void drawColor(){
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(width_g,0);
  stroke(0); noFill(); strokeWeight(3); rect(0,0,width-width_g,height_g);
  noStroke(); fill(red_G,green_G,blue_G); 
  rectMode(CENTER); rect((width-width_g)*0.5, height_g*0.5,200,200);
  rectMode(CORNER);
  popMatrix(); //座標軸の位置をスタックから取り出すし設定する ... この場合(0, 0)
}

void keyPressed() {
  if (key == 's')     
    count_mode = 0;
  else if (key == 'c')
    count_mode = 1;
}

void drawZumo(){
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(0,height_g);  //座標軸を移動
  stroke(0); noFill(); strokeWeight(3); rect(0,0,1000,550);
  
  translate(-50,20);   //座標軸を移動
  noStroke();  fill(255);  rect(60,0,980,497);
  // Draw Acceleration vector
  CX = 250;
  drawVec(ax,ay,az);
  // Draw Magnetic flux vector
  CX = 750;
  drawVec(mx,my,mz);
  // Draw Heading direction
  CX = 600;
  float scale = 0.5;
  line(CX-scale*my,100+scale*mx,CX+scale*my,100-scale*mx);
  line(CX+scale*my,100-scale*mx,CX+0.6*scale*my+0.2*scale*mx,100-0.6*scale*mx+0.2*scale*my);
  line(CX+scale*my,100-scale*mx,CX+0.6*scale*my-0.2*scale*mx,100-0.6*scale*mx-0.2*scale*my);
  popMatrix(); //座標軸の位置をスタックから取り出すし設定する ... この場合(0, 0)
}

void drawText(){
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(1000,height_g);  //座標軸を移動
  noStroke();  fill(255);  rect(5,5,550,500); //白紙に戻す
  stroke(0); noFill(); strokeWeight(3); rect(0,0,600,550); //黒枠の描画
  
  fill(0); //文字色：黒
  
  //zumo番号を描画
  textSize(50);                                                  //文字の大きさを 20 に 
  //println(n_zumo);
  if(n_zumo == 1){
    text("Zumo1",1200,500);
  }
  else if(n_zumo == 2){
    text("Zumo2",1200,500);
  }
  else if(n_zumo == 3){
    text("Zumo3",1200,500);
  }
   textSize(20);  
  
  popMatrix(); //座標軸の位置をスタックから取り出すし設定する ... この場合(0, 0)
}

void draw() {
  drawRGBgraph();
  drawColor();
  drawZumo();
  drawText();

 
}

void serialEvent(Serial p) {
  if (p.available() >=13 ) {
    if (p.read() == 'H') {
      red_p = red_G; green_p = green_G; blue_p = blue_G; // 一つ前の色を格納しておく。
      
      mode_G = p.read();    red_G = p.read();    green_G = p.read();    blue_G = p.read();
      ax = p.read()-128;      ay = p.read()-128;      az = p.read()-128;
      mx = p.read()-128;      my = p.read()-128;      mz = p.read()-128;
      motorL_G = 2*(p.read()-128);  motorR_G = 2*(p.read()-128);
      
      if(mode_G == 99){                    // 現在動作中Zumoの動作が終了したら
        n_zumo++;                              //次のZumoへ移行
        if(n_zumo > 3) n_zumo = 1;
      }
      sendZumoData();
      p.clear(); //念の為クリア
      p.write("A");
      if ( count_mode == 1 )
        ++count;
    }
  }
  
}

void sendZumoData(){
  //port1.write('H');            //port1に’H'を送信
  //port1.write(n_zumo);  //port1にn_zumoの内容を送信
  //port2.write('H');           //port2に’H'を送信
  //port2.write(n_zumo); //port2にn_zumoの内容を送信
  //port3.write('H');          //port3に’H'を送信
  //port3.write(n_zumo); //port3にn_zumoの内容を送信
}
