
import processing.serial.*;
Serial port1;
Serial port2;
Serial port3;
int n_zumo = 99;
int count=0;
int count_mode = 0;
int LF = 10;                     // LF（Linefeed）のアスキーコード
int height_g, width_g;           // RGBgraphを描画する高さと横幅

int ax = 0, ay = 0, az = 0;
int mx = 0, my = 0, mz = 0;
int CX=250, CY=250;

int[] mode_G  = new int[3];
int[] zflag   = new int[3];  // 一周終わったら1

int[] red_G   = new int[3];
int[] green_G = new int[3];
int[] blue_G  = new int[3];

int[] red_p   = new int[3];
int[] green_p = new int[3];
int[] blue_p  = new int[3];

int[] countR  = new int[3];
int[] countG  = new int[3];
int[] countB  = new int[3];

int[] motorR_G  = new int[3];
int[] motorL_G  = new int[3];

int n;

void setup() {
   size(1200,800);
   width_g = 900; height_g = width_g/3; 
   background(255);
   count = 0;
   count_mode = 1;
   n_zumo = 99;
   
   // 初期化
   for(int i=0;i<3;i++){
     mode_G[i] = 0; zflag[i] = 0;
     red_G[i] = 0; green_G[i] = 0; blue_G[i] = 0; 
     red_p[i] = 0; green_p[i] = 0; blue_p[i] = 0; 
     countR[i] = 0; countG[i] = 0; countB[i] = 0;
     motorR_G[i] = 0; motorL_G[i] = 0;
   }
   
   port1 = new Serial(this, "/dev/ttyUSB0", 9600); //Serial クラスのインスタンスを生成
   port1.clear();
   //port1.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
   //port2 = new Serial(this, "/dev/ttyUSB1", 9600); //Serial クラスのインスタンスを生成
   //port2.clear();
   //port2.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
   //port3 = new Serial(this, "/dev/ttyUSB2", 9600); //Serial クラスのインスタンスを生成
   //port3.clear();
   //port3.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
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

  y_p = map(red_p[n], 0, 255, height_g*0.9, height_g*0.1);
  y = map(red_G[n], 0, 255, height_g*0.9, height_g*0.1);
  stroke(255, 0, 0);
  line((count-1)*10, y_p, (count)*10, y );

  y_p = map(green_p[n], 0, 255, height_g*0.9, height_g*0.1);
  y = map(green_G[n], 0, 255, height_g*0.9, height_g*0.1);
  stroke(0, 255, 0);
  line((count-1)*10, y_p, (count)*10, y );


  y_p = map(blue_p[n], 0, 255, height_g*0.9, height_g*0.1);
  y = map(blue_G[n], 0, 255, height_g*0.9, height_g*0.1);
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
  noStroke(); fill(red_G[n],green_G[n],blue_G[n]); 
  rectMode(CENTER); rect((width-width_g)*0.5, height_g*0.5,(width-width_g)/2,height_g/2);
  rectMode(CORNER);
  popMatrix(); //座標軸の位置をスタックから取り出すし設定する ... この場合(0, 0)
}

void keyPressed() {
  if (key == 's')     
    count_mode = 0;
  else if (key == 'c')
    count_mode = 1;
}

void mousePressed(){
  n_zumo = 1;  // zumo1を開始させる
  port1.write(n_zumo);
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
  popMatrix(); //座標軸の位置をスタックから取り出し設定する ... この場合(0, 0)
}

void drawText(){
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(0,height_g);  //座標軸を移動
  noStroke();  fill(255);  rect(5,5,width-10,height-height_g-10); //白紙に戻す
  stroke(0); strokeWeight(3); 
  
  textSize(50);
  textAlign(CENTER,TOP); // 中央揃え
  
  //Zumo1
  noFill(); rect(0,0,width/3,height-height_g); //黒枠の描画
  
  if(n_zumo == 1) fill(255,0,0); //文字色：赤
  else            fill(0);       //文字色：黒
  text("Zumo1",width/6,50);
  
  fill(0); //文字色：黒
  text("mode:"+ mode_G[0],width/6,100);
  
  if(motorL_G[0] == 0 && motorR_G[0]== 0) text("STOP",width/6,150);
  else text("RUN",width/6,150);
    
  if(mode_G[0] == 3) text("CROSS",width/6,200);
  else if(mode_G[0] == 4) text("Zone1",width/6,200);
  
  if(zflag[0] == 1) text("END",width/6,250);
  /*
  textAlign(LEFT,TOP); 
  fill(255,0,0); text("Red    :"+countR[0],width/6-100,300);
  fill(0,255,0); text("Green :"+countG[0],width/6-100,350);
  fill(0,0,255); text("Blue   :"+countB[0],width/6-100,400);
  textAlign(CENTER,TOP); // 中央揃え
  */
  text("ax :"+ax,width/6-100,300);
  text("ay :"+ay,width/6-100,350);
  text("az :"+az,width/6-100,400);
  text("mx :"+mx,width/6+75,300);
  text("my :"+my,width/6+75,350);
  text("mz :"+mz,width/6+75,400);
  
  //Zumo2
  translate(width/3,0);  //座標軸を移動
  noFill(); rect(0,0,width/3,height-height_g); //黒枠の描画
  
  if(n_zumo == 2) fill(255,0,0); //文字色：赤
  else            fill(0);       //文字色：黒
  text("Zumo2",width/6,50);
  
  fill(0); //文字色：黒
  text("mode:"+ mode_G[1],width/6,100);
  
  if(motorL_G[1] == 0 && motorR_G[1]== 0) text("STOP",width/6,150);
  else text("RUN",width/6,150);
  
  if(mode_G[1] == 3) text("CROSS",width/6,200);
  else if(mode_G[1] == 4) text("Zone3",width/6,200);
  
  if(zflag[1] == 1) text("END",width/6,250);
  
  textAlign(LEFT,TOP); 
  fill(255,0,0); text("Red    :"+countR[1],width/6-100,300);
  fill(0,255,0); text("Green :"+countG[1],width/6-100,350);
  fill(0,0,255); text("Blue   :"+countB[1],width/6-100,400);
  textAlign(CENTER,TOP); // 中央揃え
  
  
  //Zumo3
  translate(width/3,0);  //座標軸を移動
  noFill(); rect(0,0,width/3,height-height_g); //黒枠の描画
  
  if(n_zumo == 3) fill(255,0,0); //文字色：赤
  else                     fill(0);           //文字色：黒
  text("Zumo3",width/6,50);
  
  fill(0); //文字色：黒
  text("mode:"+ mode_G[2],width/6,100);
  
  if(motorL_G[2] == 0 && motorR_G[2]== 0) text("STOP",width/6,150);
  else text("RUN",width/6,150);
  
  if(mode_G[2] == 3) text("CROSS",width/6,200);
  else if(mode_G[2] == 4) text("Zone2",width/6,200);
  
  if(zflag[2] == 1) text("END",width/6,250);
  
  textAlign(LEFT,TOP); 
  fill(255,0,0); text("Red    :"+countR[2],width/6-100,300);
  fill(0,255,0); text("Green :"+countG[2],width/6-100,350);
  fill(0,0,255); text("Blue   :"+countB[2],width/6-100,400);
  textAlign(CENTER,TOP); // 中央揃え
  
  // 元に戻す
  fill(0); //文字色：黒
  textSize(20);
  textAlign(LEFT,TOP); 
  popMatrix(); //座標軸の位置をスタックから取り出し設定する ... この場合(0, 0)
}

void draw() {
  // Zumo番号を 配列で扱う数字n に変換
  if(n_zumo > 3)n = 0;
  else n = n_zumo - 1; 
  
  drawRGBgraph();
  drawColor();
  //drawZumo();
  drawText();

 
}

void serialEvent(Serial p) {
  //zumo1
  if (p == port1 && p.available() >= 17 && p.read() == 'H') {
    red_p[0] = red_G[0]; green_p[0] = green_G[0]; blue_p[0] = blue_G[0]; // 一つ前の色を格納しておく。
    
    mode_G[0] = p.read(); 
    
    red_G[0] = p.read(); green_G[0] = p.read(); blue_G[0] = p.read();
    
    ax = p.read()-128;      ay = p.read()-128;      az = p.read()-128;
    mx = p.read()-128;      my = p.read()-128;      mz = p.read()-128;
    
    countR[0] = p.read(); countG[0] = p.read(); countB[0] = p.read();
    motorL_G[0] = 2*(p.read()-128);  motorR_G[0] = 2*(p.read()-128);
    zflag[0] = p.read();
    
    p.clear(); //念の為クリア
    port1.write(n_zumo);
    
    if ( n_zumo == 1 && count_mode == 1 )
      ++count;
  }
  
  //zumo2
  if (p == port2 && p.available() >= 11 && p.read() == 'H') {
    red_p[1] = red_G[1]; green_p[1] = green_G[1]; blue_p[1] = blue_G[1]; // 一つ前の色を格納しておく。
    
    mode_G[1] = p.read(); red_G[1] = p.read(); green_G[1] = p.read(); blue_G[1] = p.read();
    /*
    ax = p.read()-128;      ay = p.read()-128;      az = p.read()-128;
    mx = p.read()-128;      my = p.read()-128;      mz = p.read()-128;
    */
    countR[1] = p.read(); countG[1] = p.read(); countB[1] = p.read();
    motorL_G[1] = 2*(p.read()-128);  motorR_G[1] = 2*(p.read()-128);
    zflag[1] = p.read();
   
    p.clear(); //念の為クリア
    port2.write(n_zumo);
    
    if ( n_zumo == 2 && count_mode == 1 )
      ++count;
  }
  
  //zumo3
  if (p == port3 && p.available() >= 11 && p.read() == 'H') {
    red_p[2] = red_G[2]; green_p[2] = green_G[2]; blue_p[2] = blue_G[2]; // 一つ前の色を格納しておく。
    
    mode_G[2] = p.read(); red_G[2] = p.read(); green_G[2] = p.read(); blue_G[2] = p.read();
    /*
    ax = p.read()-128;      ay = p.read()-128;      az = p.read()-128;
    mx = p.read()-128;      my = p.read()-128;      mz = p.read()-128;
    */
    countR[2] = p.read(); countG[2] = p.read(); countB[2] = p.read();
    motorL_G[2] = 2*(p.read()-128);  motorR_G[2] = 2*(p.read()-128);
    zflag[2] = p.read();
    
    p.clear(); //念の為クリア
    port3.write(n_zumo);
    
    if ( n_zumo == 3 && count_mode == 1 )
      ++count;
  }
  
  // 動作させるZumo番号の判定
    if(zflag[0] == 1 && zflag[1] == 0 && zflag[2] == 0) n_zumo = 2;
    else if(zflag[0] == 1 && zflag[1] == 1 && zflag[2] == 0) n_zumo = 3;
}
