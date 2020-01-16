import processing.serial.*;
Serial port1;
Serial port2;
Serial port3;

static final char LINE_COLOR = 'B';

int n_zumo = 99;
int LF = 10;                     // LF（Linefeed）のアスキーコード

int CX=250, CY=250;

int[] mode_G  = new int[3];
int[] zflag   = new int[3];  // 一周終わったら1

int[] red_G   = new int[3];
int[] green_G = new int[3];
int[] blue_G  = new int[3];

int[] countR  = new int[3];
int[] countG  = new int[3];
int[] countB  = new int[3];

int[] countCross  = new int[3];

int[] motorR_G  = new int[3];
int[] motorL_G  = new int[3];

int[] ax  = new int[3];
int[] ay  = new int[3];
int[] az  = new int[3];

int[] mx  = new int[3];
int[] my  = new int[3];
int[] mz  = new int[3];

int[] mx_p  = new int[3];
int[] my_p  = new int[3];
int[] mz_p  = new int[3];

PImage img;  // マップ画像
int posx, posy;
int n;



void setup() {
   size(1600,900);
   background(255);
   strokeWeight(3);
   n_zumo = 99;
   img = loadImage("map.png"); //1198*899
   posx = 1000; posy = 720;
   
   // 初期化
   for(int i=0;i<3;i++){
     mode_G[i] = 0; zflag[i] = 0;
     red_G[i] = 0; green_G[i] = 0; blue_G[i] = 0; 
     countR[i] = 0; countG[i] = 0; countB[i] = 0;
     motorR_G[i] = 0; motorL_G[i] = 0;
     ax[i] = 0; ay[i] = 0; az[i] = 0;
     mx[i] = 0; my[i] = 0; mz[i] = 0;
   }
   
   //port1 = new Serial(this, "/dev/ttyUSB0", 9600); //Serial クラスのインスタンスを生成
   //port1.clear();
   //port1.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
   //port2 = new Serial(this, "/dev/ttyUSB1", 9600); //Serial クラスのインスタンスを生成
   //port2.clear();
   //port2.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
   //port3 = new Serial(this, "/dev/ttyUSB2", 9600); //Serial クラスのインスタンスを生成
   //port3.clear();
   //port3.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
}

void keyPressed(){
  if (key == '1'){
    n_zumo = 1;  // zumo1を開始させる
    //port1.write(n_zumo);
  }
  else if (key == '2'){
    n_zumo = 2;  // zumo2を開始させる
    //port2.write(n_zumo);
  }
  else if (key == '3'){
    n_zumo = 3;  // zumo3を開始させる
    //port3.write(n_zumo);
  }
}


void drawZone3(){
  int a = 0;
  //確認用
  mx[a]=100; my[a]=173; mz[a]=0;
  ax[a]=50; ay[a]=50; az[a]=50;
  
  strokeWeight(1);
  // Draw Acceleration vector
  CX = 250;
  drawVec(ax[a],ay[a],az[a]);
  // Draw Magnetic flux vector
  CX = 750;
  drawVec(mx[a],my[a],mz[a]);
  // Draw Heading direction
  CX = 600;
  float scale = 0.5;
  line(CX-scale*my[a],100+scale*mx[a],CX+scale*my[a],100-scale*mx[a]);
  line(CX+scale*my[a],100-scale*mx[a],CX+0.6*scale*my[a]+0.2*scale*mx[a],100-0.6*scale*mx[a]+0.2*scale*my[a]);
  line(CX+scale*my[a],100-scale*mx[a],CX+0.6*scale*my[a]-0.2*scale*mx[a],100-0.6*scale*mx[a]-0.2*scale*my[a]);
  
  strokeWeight(3);
  stroke(0,255,255);
  box(300);
  CX = 750;
  float s,t,u; // x,y,z軸周りの回転量(ラジアン)
  s = atan((float)mz[a] / (float)my[a]);
  t = atan((float)mx[a] / (float)mz[a]);
  u = atan((float)my[a] / (float)mx[a]);
  int w=100,d=100,h=100;
  float[][] pos = { // 直方体(正面)の頂点の座標{x,y,z}
    {-d/2,-w/2,-h/2},// 奥 左下
    {-d/2,-w/2,h/2}, // 奥 左上
    {-d/2,w/2,h/2},  // 奥 右上
    {-d/2,w/2,-h/2}, // 奥 右下
    {d/2,-w/2,-h/2}, // 手前 左下
    {d/2,-w/2,h/2},  // 手前 左上
    {d/2,w/2,h/2},   // 手前 右上
    {d/2,w/2,-h/2},  // 手前 右下
  };
  //z軸回りにs度,y軸回りにt度 回転移動
  for(int i=0;i<8;i++){
    //pos[i][0] = pos[i][0]*cos(t)*cos(u)+pos[i][1]*(sin(s)*sin(t)*cos(u)-cos(s)*sin(u))+pos[i][2]*(cos(s)*sin(t)*cos(u)+sin(s)*sin(u));
    //pos[i][1] = pos[i][0]*cos(t)*sin(u)+pos[i][1]*(sin(s)*sin(t)*sin(u)+cos(s)*cos(u))+pos[i][2]*(cos(s)*sin(t)*sin(u)-sin(u)*cos(u));
    //pos[i][2] = -pos[i][0]*sin(t)+pos[i][1]*sin(s)*sin(t)+pos[i][2]*cos(s)*cos(t);
    pos[i][0] = pos[i][0]*cos(t)*cos(u)+pos[i][1]*(sin(s)*sin(t)*cos(u)-cos(s)*sin(u))+pos[i][2]*(cos(s)*sin(t)*cos(u)+sin(s)*sin(u));
    pos[i][1] = pos[i][0]*cos(t)*sin(u)+pos[i][1]*(sin(s)*sin(t)*sin(u)+cos(s)*cos(u))+pos[i][2]*(cos(s)*sin(t)*sin(u)-sin(u)*cos(u));
    pos[i][2] = -pos[i][0]*sin(t)+pos[i][1]*sin(s)*sin(t)+pos[i][2]*cos(s)*cos(t);
  }
  line3D(pos[0][0],pos[0][1],pos[0][2],pos[1][0],pos[1][1],pos[1][2]); // 奥 左下 - 奥 左上
  line3D(pos[0][0],pos[0][1],pos[0][2],pos[3][0],pos[3][1],pos[3][2]); // 奥 左下 - 奥 右下
  line3D(pos[0][0],pos[0][1],pos[0][2],pos[4][0],pos[4][1],pos[4][2]); // 奥 左下 - 手前 左下
  line3D(pos[2][0],pos[2][1],pos[2][2],pos[1][0],pos[1][1],pos[1][2]); // 奥 右上 - 奥 左上
  line3D(pos[2][0],pos[2][1],pos[2][2],pos[3][0],pos[3][1],pos[3][2]); // 奥 右上 - 奥 右下
  line3D(pos[2][0],pos[2][1],pos[2][2],pos[6][0],pos[6][1],pos[6][2]); // 奥 右上 - 手前 右上
  line3D(pos[6][0],pos[6][1],pos[6][2],pos[7][0],pos[7][1],pos[7][2]); // 手前 右上 - 手前 右下
  line3D(pos[6][0],pos[6][1],pos[6][2],pos[5][0],pos[5][1],pos[5][2]); // 手前 右上 - 手前 左上
  line3D(pos[4][0],pos[4][1],pos[4][2],pos[7][0],pos[7][1],pos[7][2]); // 手前 左下 - 手前 右下
  line3D(pos[4][0],pos[4][1],pos[4][2],pos[5][0],pos[5][1],pos[5][2]); // 手前 左下 - 手前 左上
  line3D(pos[1][0],pos[1][1],pos[1][2],pos[5][0],pos[5][1],pos[5][2]); // 奥 左上 - 手前 左上
  line3D(pos[3][0],pos[3][1],pos[3][2],pos[7][0],pos[7][1],pos[7][2]); // 奥 右下 - 手前 右下
  
  text(s+"  "+t+"  "+u,50,50);
  text(degrees(s)+"  "+degrees(t)+"  "+degrees(u),50,80);
  
}

void draw() {
  //扱い易いように、Zumo番号に対応する配列番号に変換
  if(n_zumo <= 3) n = n_zumo - 1;
  else  n = 0;
  
  //確認用
 mode_G[0]=10;
  
  if(mode_G[n] < 10){
    //コースを回っているときの描画
    background(255);
    drawImage();
    drawText();
    drawZumo();
  }else{
    background(255);
    drawZone3();
  }
 
}

void serialEvent(Serial p) {
  int a;
  //ロボット１
  if (p == port1){
    a = 0;
    if(mode_G[a]<10){ // コース周回
      if(p.available() >= 12 && p.read() == 'H') {
        
        mode_G[a] = p.read();
        red_G[a] = p.read();
        green_G[a] = p.read();
        blue_G[a] = p.read();
        countCross[a] = p.read();
        countR[a] = p.read();
        countG[a] = p.read();
        countB[a] = p.read();
        motorL_G[a] = 2*(p.read()-128);
        motorR_G[a] = 2*(p.read()-128);
        zflag[a] = p.read();
      }
    }else{ // ゾーンタスク
      if(p.available() >= 11 && p.read() == 'H') {
        mode_G[a] = p.read();
        ax[a] = p.read()-128;
        ay[a] = p.read()-128;      
        az[a] = p.read()-128;
        mx[a] = p.read()-128;      
        my[a] = p.read()-128;      
        mz[a] = p.read()-128;
        motorL_G[a] = 2*(p.read()-128);  
        motorR_G[a] = 2*(p.read()-128);
        zflag[a] = p.read();
      }
    }
      p.clear(); //念の為クリア
      port1.write(n_zumo);
  }
  
  //ロボット２
  if (p == port1){
    a = 1;
    if(mode_G[a]<10){ // コース周回
      if(p.available() >= 12 && p.read() == 'H') {
        
        mode_G[a] = p.read();
        red_G[a] = p.read();
        green_G[a] = p.read();
        blue_G[a] = p.read();
        countCross[a] = p.read();
        countR[a] = p.read();
        countG[a] = p.read();
        countB[a] = p.read();
        motorL_G[a] = 2*(p.read()-128);
        motorR_G[a] = 2*(p.read()-128);
        zflag[a] = p.read();
      }
    }else{ // ゾーンタスク
      if(p.available() >= 11 && p.read() == 'H') {
        mode_G[a] = p.read();
        ax[a] = p.read()-128;
        ay[a] = p.read()-128;      
        az[a] = p.read()-128;
        mx[a] = p.read()-128;      
        my[a] = p.read()-128;      
        mz[a] = p.read()-128;
        motorL_G[a] = 2*(p.read()-128);  
        motorR_G[a] = 2*(p.read()-128);
        zflag[a] = p.read();
      }
    }
      p.clear(); //念の為クリア
      port2.write(n_zumo);
  }
  
  //ロボット３
  if (p == port1){
    a = 2;
    if(mode_G[a]<10){ // コース周回
      if(p.available() >= 12 && p.read() == 'H') {
        
        mode_G[a] = p.read();
        red_G[a] = p.read();
        green_G[a] = p.read();
        blue_G[a] = p.read();
        countCross[a] = p.read();
        countR[a] = p.read();
        countG[a] = p.read();
        countB[a] = p.read();
        motorL_G[a] = 2*(p.read()-128);
        motorR_G[a] = 2*(p.read()-128);
        zflag[a] = p.read();
      }
    }else{ // ゾーンタスク
      if(p.available() >= 11 && p.read() == 'H') {
        mode_G[a] = p.read();
        ax[a] = p.read()-128;
        ay[a] = p.read()-128;      
        az[a] = p.read()-128;
        mx[a] = p.read()-128;      
        my[a] = p.read()-128;      
        mz[a] = p.read()-128;
        motorL_G[a] = 2*(p.read()-128);  
        motorR_G[a] = 2*(p.read()-128);
        zflag[a] = p.read();
      }
    }
      p.clear(); //念の為クリア
      port3.write(n_zumo);
  }
  
  // 動作させるZumo番号の判定
    if(zflag[0] == 1 && zflag[1] == 0 && zflag[2] == 0) n_zumo = 2;
    else if(zflag[0] == 1 && zflag[1] == 1 && zflag[2] == 0) n_zumo = 3;
  
}

void drawText(){
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(1200,0);  //座標軸を移動
  
  textSize(30);
  textAlign(CENTER,TOP); // 中央揃え
  
  for (int i=0;i<3;i++){
    noFill(); rect(0,0,width-1200,height/3); //黒枠の描画
    
    if(n == i) fill(255,0,0); //文字色：赤
    else            fill(0);       //文字色：黒
    text("Zumo"+ (i+1),(width-1200)/2,30);
    
    fill(0); //文字色：黒
    text("mode "+ mode_G[i],(width-1200)/2,60);
    
    if(motorL_G[i] == 0 && motorR_G[i]== 0) text("STOP",(width-1200)/2,100);
    else text("RUN",width/6,100);
      
    if(mode_G[i] == 3) text("CROSS",(width-1200)/2,130);
    else if(mode_G[i] == 4) text("Zone",(width-1200)/2,130);
    
    if(zflag[i] == 1) text("END",(width-1200)/2,130);
    
    textAlign(LEFT,TOP); 
    fill(255,0,0); text("Red    :  "+countR[i],(width-1200)/2-80,180);
    fill(0,255,0); text("Green :  "+countG[i],(width-1200)/2-80,210);
    fill(0,0,255); text("Blue   :  "+countB[i],(width-1200)/2-80,240);
    textAlign(CENTER,TOP); // 中央揃え
    translate(0,height/3);  //座標軸を移動
  }
    
  // 元に戻す
  fill(0); //文字色：黒
  textSize(20);
  textAlign(LEFT,TOP); 
  popMatrix(); //座標軸の位置をスタックから取り出し設定する ... この場合(0, 0)
}

void drawImage(){
  image(img,0,0);
  noFill(); rect(0,0,1200,900); //黒枠の描画
}

// Zumoの位置を描画
void drawZumo(){
  switch(mode_G[n]){
    case 0:
    case 99:
      posx = 1000; posy = 720;
      break;
    case 3:
      switch(countCross[n]){ // ここに交差点の座標を書く
        case 1:
          posx = 950; posy  = 670;
          break;
        case 2:
          posx = 650; posy  = 370;
          break;
        case 3:
          posx = 550; posy  = 350;
          break;
        case 4:
          posx = 950; posy  = 570;
          break;
      }
      break;
    case 4:
      if(LINE_COLOR == 'G'){
        switch(countB[n]){
          case 1:
            posx = 140; posy = 110;
            break;
          case 2:
            posx = 1030; posy = 120;
            break;
          case 3:
            posx = 430; posy = 500;
            break;
        }
      }else{
        switch(countG[n]){
          case 1:
            posx = 140; posy = 110;
            break;
          case 2:
            posx = 1030; posy = 120;
            break;
          case 3:
            posx = 770; posy = 500;
            break;
        }
      }
      break;
  }
  
  if(LINE_COLOR == 'G' && mode_G[n] != 4){
    posx = 1200 - posx;
  }
  
  stroke(0); fill(255,255,0);
  ellipse(posx,posy,50,50);
  
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
