import processing.serial.*;
Serial port1;
Serial port2;
Serial port3;

static final char LINE_COLOR = 'B';

int n_zumo = 99;
int LF = 10;                     // LF（Linefeed）のアスキーコード
int height_g, width_g;           // RGBgraphを描画する高さと横幅

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

int[] direction  = new int[3];
int[] avex  = new int[3];

int cup;//Zone2で使う

PImage img;  // マップ画像
int posx, posy;
int n;

////zone1追加部分
int high,low;
int drawing_flag; //図形を描画中かどうかを示す．
float global_direction_G; //現在の角度(度，グローバル座標を基準)
float global_direction_G_radian; //現在の角度(ラジアン，グローバル座標を基準)
float distance; 
float old_x, old_y, now_x, now_y = 0; //トレースした図形の描画用

int dec=0,dip=0; // 視点変更用（偏角・伏角）
int dec_i=0,dec_d=0,dip_i=0,dip_d=0; // 長押し処理用

void setup() {
   size(1024,768,P3D);
   background(255);
   strokeWeight(3);
   n_zumo = 99;
   img = loadImage("map.png"); //1198*899
   PFont font = createFont("IPAGothic",50);
   textFont (font);
   posx = 640; posy = 460;
   width_g = width*3/4; height_g = width_g/3;
   
   // 初期化
   for(int i=0;i<3;i++){
     mode_G[i] = 0; zflag[i] = 0;
     red_G[i] = 0; green_G[i] = 0; blue_G[i] = 0; 
     countR[i] = 0; countG[i] = 0; countB[i] = 0;
     motorR_G[i] = 0; motorL_G[i] = 0;
     ax[i] = 0; ay[i] = 0; az[i] = 0;
     mx[i] = 0; my[i] = 0; mz[i] = 0;
     direction[i] = 0; avex[i] = 0;
   }
   drawing_flag = 0;
   //port1 = new Serial(this, "/dev/ttyUSB0", 9600); //Serial クラスのインスタンスを生成
   port3 = new Serial(this,"/dev/cu.usbserial-A90177EP",9600);
   port3.clear();
   //port1.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
   //port2 = new Serial(this, "/dev/ttyUSB1", 9600); //Serial クラスのインスタンスを生成
   //port2.clear();
   //port2.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
   //port3 = new Serial(this, "/dev/ttyUSB2", 9600); //Serial クラスのインスタンスを生成
   //port3.clear();
   //port3.bufferUntil(0x0d); // LF = 0x0d までバッファ いらなさげ
}

void drawZone1() {
  //distance = distance*0.04;
  
  now_x = old_x + distance * cos(global_direction_G_radian);
  now_y = old_y + distance * sin(global_direction_G_radian); //y座標を反転させるのは，描画の時にする
  
  strokeWeight(10); //点の大きさは10ピクセル
  stroke(0, 255, 0); //点の色は緑
  point(old_x + width*1/5, -1 * old_y + height/2);  //1つ前の座標
  stroke(255, 0, 0); //点の色は赤
  point(now_x + width*1/5, -1 * now_y +  height/2); //数学の座標とはy方向が反対になる
  strokeWeight(5); //線の太さは5ピクセル
  stroke(255); //ラインの色は黒
  line(now_x + width*1/5, -1 * now_y +  height/2, old_x + width*1/5, -1 * old_y +  height/2); //線を引く
  stroke(255); //ラインの色は黒
  old_x = now_x;
  old_y = now_y;
  textAlign(CENTER,TOP); // 中央揃え
  text("mode:"+ mode_G[0],width/6,100);
  textAlign(LEFT,TOP); 
  fill(255,0,0); text("Red    :"+red_G[2],width/6-100,300);
  fill(0,255,0); text("Green :"+green_G[2],width/6-100,350);
  fill(0,0,255); text("Blue   :"+blue_G[2],width/6-100,400);
  
}

void drawZone2(){
  textSize(100);
  textAlign(CENTER,TOP); // 中央揃え
  switch (mode_G[1]){
    case 200:
      text("セットアップ",width/8*3,50);
      break;
    case 201:
      text("探索中",width/8*3,50);
      break;
    case 202:
      text("探索中",width/8*3,50);
      break;
    case 203:
      text("探索中",width/8*3,50);
      break;     
    case 204:
      text("突進",width/8*3,50);
      break;
    case 205:
      text("後退",width/8*3,50);
      break;      
    case 206:
      text("反転（所持）",width/8*3,50);
      break;
    case 207:
      text("反転（非所持）",width/8*3,50);
      break;
  }
  if(mode_G[1]>=220){
    text("脱出",width/8*3,50);
  }
    
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(width_g,0);
  stroke(0); noFill(); strokeWeight(3); rect(0,0,width-width_g,height_g);
  
  stroke(0);
  textSize(50);
  textAlign(CENTER,TOP); // 中央揃え
  text("Mode:"+mode_G[1],(width-width_g)/2,50);
  
  popMatrix(); //座標軸の位置をスタックから取り出し設定する ... この場合(0, 0)
  
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(0,height_g);  //座標軸を移動
  noStroke();  fill(255);  rect(5,5,width-10,height-height_g-10); //白紙に戻す
  stroke(0); strokeWeight(3); 
  
  textSize(50);
  textAlign(CENTER,TOP); // 中央揃え
  
  //Cup1
  noFill(); rect(0,0,width/3,height-height_g); //黒枠の描画
  
  fill(255,0,0);
  
  text("Cup1",width/6,100);
  
  fill(0); //文字色：黒
  switch(cup / 100){
    case 0:
      text("Zone",width/6,200);
      break;
    case 1:
      text("In",width/6,200);
      break;
    case 2:
      text("Out",width/6,200);
      break;
  }
  
  
  //Cup2
  translate(width/3,0);  //座標軸を移動
  noFill(); rect(0,0,width/3,height-height_g); //黒枠の描画
  
  fill(0,255,0);
  text("Cup2",width/6,100);
  
  fill(0); //文字色：黒
  switch(cup / 10 % 10){
    case 0:
      text("Zone",width/6,200);
      break;
    case 1:
      text("In",width/6,200);
      break;
    case 2:
      text("Out",width/6,200);
      break;
  }
  
  
  //Cup3
  translate(width/3,0);  //座標軸を移動
  noFill(); rect(0,0,width/3,height-height_g); //黒枠の描画
  
  fill(0,0,255);
  text("Cup3",width/6,100);
  
  fill(0); //文字色：黒
  switch(cup % 10 % 10){
    case 0:
      text("Zone",width/6,200);
      break;
    case 1:
      text("In",width/6,200);
      break;
    case 2:
      text("Out",width/6,200);
      break;
  }
  
  // 元に戻す
  fill(0); //文字色：黒
  textSize(20);
  textAlign(LEFT,TOP); 
  popMatrix(); //座標軸の位置をスタックから取り出し設定する ... この場合(0, 0)
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
  
  //////////////////// 確認用 //////////////////////
  if(key==','){ 
    mode_G[n]++;
  }
  if(key=='.'){ 
    mode_G[n]--;
  }
  if(key=='/'){ 
    mode_G[n]+=10;
  }
  if(key=='0'){ 
    mode_G[n]=0;
  }
  if (key == CODED) {      // コード化されているキーが押された
    if (keyCode == RIGHT)  dec_i = 1;
    if (keyCode == LEFT)   dec_d = 1;
    if (keyCode == UP)     dip_i = 1;
    if (keyCode == DOWN)   dip_d = 1;
  }
}
void keyReleased() {
  if (key == CODED) {      // コード化されているキーが押された
    if (keyCode == RIGHT)  dec_i = 0;
    if (keyCode == LEFT)   dec_d = 0;
    if (keyCode == UP)     dip_i = 0;
    if (keyCode == DOWN)   dip_d = 0;
  }
}

void draw() {
  //扱い易いように、Zumo番号に対応する配列番号に変換
  if(n_zumo <= 3) n = n_zumo - 1;
  else  n = 0;
  
  //確認用
  //mode_G[0]=10;
  if(mode_G[0] >= 10 && mode_G[0] <= 50){
    // ゾーン描画
    background(255);
    drawZone3();
  }else if(mode_G[1] >= 200 && mode_G[1] < 255){
    //ゾーン2描画
    background(255);
    drawZone2();
  }else if(mode_G[2] > 0 && mode_G[2] < 120 ){
    if(drawing_flag == 1){
      drawZone1();
    }
    else{
      background(255);
    }
  }else{
   // コースを回っているときの描画
    background(255);
    drawImage();
    drawText();
    drawZumo();
  }
  
 
}

void serialEvent(Serial p) {
  int a;
//ロボット１
  if (p == port1 && p.available() >= 16 && p.read() == 'H') {
    a = 0;
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
    //direction[a] = p.read();
    //avex[a] = p.read();
    zflag[a] = p.read();
    
    high = p.read();
    low = p.read(); 
    global_direction_G = (high << 8) + low;

    global_direction_G_radian = radians(global_direction_G); //ラジアンに変換

   distance = p.read(); //移動距離
   drawing_flag = p.read(); //図形を描画中かどうかを示す．
   
   p.clear(); //念の為クリア
   port1.write(n_zumo);
  }
  
  //ロボット2
  if (p == port2 && p.available() >= 13 && p.read() == 'H') {
    a = 1;
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
    cup = p.read();
    
    
    p.clear(); //念の為クリア
    port2.write(n_zumo);
  }
  
  //ロボット3
  if (p == port3 && p.available() >= 16 && p.read() == 'H') {
    a = 2;
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
    //direction[a] = p.read();
    //avex[a] = p.read();
    zflag[a] = p.read();
    
    high = p.read();
    low = p.read(); 
    global_direction_G = (high << 8) + low;

    global_direction_G_radian = radians(global_direction_G); //ラジアンに変換

   distance = p.read(); //移動距離
   distance = 1;
   drawing_flag = p.read(); //図形を描画中かどうかを示す．
   /*
   print("  global_direction_G = ");
   println(global_direction_G);
   print("  distance = ");
   println(distance);
   print(" drawing_flag = ");
   println(drawing_flag);
   */
    p.clear(); //念の為クリア
    port3.write(n_zumo);
  }
  // 動作させるZumo番号の判定
    if(zflag[0] == 1 && zflag[1] == 0 && zflag[2] == 0) n_zumo = 2;
    else if(zflag[0] == 1 && zflag[1] == 1 && zflag[2] == 0) n_zumo = 3;
  
}

void drawZone3(){
  int a = 0;
  
  // 左手系から右手系の描画に変換するために
  // z軸逆向き、x軸とy軸の回転を逆向きにしなければいけない（まだできてない）
  // 偏角、伏角をそのまま用いて回転させるなら逆向きにする必要はない
  
  ////確認用
  //mx[a]=mouseX-width/2; my[a]=mouseY-height/2; mz[a]=-200;
  //ax[a]=120; ay[a]=220; az[a]=50;
  //direction[a] = 64;
  //avex[a] = 30;
  //println(direction[a]+" | "+avex[a]);
  
  //// 単位ベクトル化
  //float x,y,z,d;
  //d = sqrt(pow(mx[a],2)+pow(my[a],2)+pow(mz[a],2));
  //x = mx[a]/d;
  //y = my[a]/d;
  //z = mz[a]/d;
  
  //float l,m,n,k;
  //k = sqrt(pow(ax[a],2)+pow(ay[a],2)+pow(az[a],2));
  //l = ax[a]/k;
  //m = ay[a]/k;
  //n = az[a]/k;
  
  //3D描画
  pushMatrix();
    perspective();
    lights();
    // 視点移動
    //camera(200,-100,-200,0,0,0,0,1,0);
    //camera(x*800,y*800,-1000,0,0,0,0,1,0);
    if(dec_i==1) dec++;
    if(dec_d==1) dec--;
    if(dip_i==1) dip++;
    if(dip_d==1) dip--;
    camera(sin(radians(dec))*cos(radians(dip))*1000,
           sin(radians(dec))*sin(radians(dip))*1000,
           cos(radians(dec))*1000,
           0,0,0,0,1,0);
    
    //pushMatrix(); // 加速度
    //  noStroke(); fill(255,0,0,50);
    //  //rotateX(acos(m));
    //  //rotateY(acos(l));
    //  //rotateZ(acos(n));
    //  //rotateZ(radians(45));
    //  float scale=2.0;
    //  translate(0,k*scale/2,0);
    //  pillar(k*scale,k*scale/80,k*scale/80);
    //  translate(0,k*scale/2+k*scale/5/2,0);
    //  pillar(k*scale/5,k*scale/20,1);
        
    //popMatrix();
    
    pushMatrix();
      noFill(); stroke(0,0,0,150); strokeWeight(2);
      //rotateX(atan(z/y));
      //rotateY(atan(x/z));
      //rotateZ(atan(y/x));
      
      //確認用
      //rotateZ(radians(45)); 
      
      //direction,avexを用いたもの
      rotateY(radians(map(direction[a],0,255,0,360)));
      rotateZ(radians(map(avex[a],0,255,0,360)));
      
      box(500,250,350); // Zumoの胴体部分
      
      pushMatrix(); // Zumoの目
        rotateY(HALF_PI); noFill();
        translate(-80,0,250); ellipse(0,0,80,80);
        translate(0,0,20); ellipse(0,0,80,80);
        translate(160,0,0); ellipse(0,0,80,80);
        translate(0,0,-20); ellipse(0,0,80,80);
      popMatrix();
      
      pushMatrix();
        rotateZ(HALF_PI); noStroke(); fill(255,255,0,100);
        translate(0,-260,-80); pillar(20,40,40);
        translate(0,0,160); pillar(20,40,40);
      popMatrix();
      
      for(int i=-1;i<2;i+=2){ // タイヤ
        pushMatrix(); 
          fill(0,0,128,70);
          translate(0,125,165*i);
          arc(-160,0,120,120,-PI*3/2,-PI/2, OPEN);
          noStroke();rect(-160,-60,320,120);
          stroke(0,0,0,150); strokeWeight(2); 
          line(-160,-60,160,-60); line(-160,60,160,60);
          arc(160,0,120,120,-PI/2,PI/2, OPEN);
          
          line(-220,0,0,-220,0,40*i); line(220,0,0,220,0,40*i);
          line(-160,-60,0,-160,-60,40*i); line(160,-60,0,160,-60,40*i);
          line(-160,60,0,-160,60,40*i); line(160,60,0,160,60,40*i);
          line(-202,-42,0,-202,-42,40*i); line(202,-42,0,202,-42,40*i);
          line(-202,42,0,-202,42,40*i); line(202,42,0,202,42,40*i);
          line(-80,-60,0,-80,-60,40*i); line(80,-60,0,80,-60,40*i);
          line(-80,60,0,-80,60,40*i); line(80,60,0,80,60,40*i);
          line(0,-60,0,0,-60,40*i); line(0,60,0,0,60,40*i);
          
          translate(0,0,40*i);
          arc(-160,0,120,120,-PI*3/2,-PI/2, OPEN);
          noStroke();rect(-160,-60,320,120);
          stroke(0,0,0,150); strokeWeight(2); 
          line(-160,-60,160,-60); line(-160,60,160,60);
          arc(160,0,120,120,-PI/2,PI/2, OPEN);
        popMatrix();
        
        pushMatrix();
          rotateX(HALF_PI); noStroke(); fill(0,0,128,30);
          translate(-160,185*i,-125); pillar(42,60,60);
          translate(320,0,0); pillar(42,60,60);
        popMatrix();
      }
    popMatrix();
  
    pushMatrix();
      stroke(0); strokeWeight(1);
      stroke(0,0,255,80); line(0,0,0,0,0,5000);
      stroke(0,255,0,80); line(0,0,0,0,5000,0);
      stroke(255,0,0,80); line(0,0,0,5000,0,0);
    popMatrix();
  popMatrix();
  
  //2D描画
  pushMatrix();
    hint(DISABLE_DEPTH_TEST);
    fill(0); textSize(50);
    if(mode_G[a]<13){
      text("山探し",50,100);
    }else if(mode_G[a]<17){
      text("taskB: 山腹",50,100);
    }else if(mode_G[a]<20){
      text("taskA: 山頂",50,100);
    }else if(mode_G[a]<22){
      text("taskC: 山麓",50,100);
    }else{
      text("ゾーンを出る",50,100);
    }
    text(mode_G[a],50,180); // 確認用
      
    hint(ENABLE_DEPTH_TEST);
  popMatrix();
  
}

void drawText(){
  pushMatrix(); //(0, 0)を原点とする座標軸をスタックに格納
  translate(768,0);  //座標軸を移動
  
  textSize(30);
  textAlign(CENTER,TOP); // 中央揃え
  
  for (int i=0;i<3;i++){
    noFill(); rect(0,0,width-768,height/3); //黒枠の描画
    
    if(n == i) fill(255,0,0); //文字色：赤
    else            fill(0);       //文字色：黒
    text("Zumo"+ (i+1),(width-768)/2,10);
    
    fill(0); //文字色：黒
    text("mode "+ mode_G[i],(width-768)/2,40);
    
    if(motorL_G[i] == 0 && motorR_G[i]== 0) text("STOP",(width-768)/2,70);
    else text("RUN",width/6,80);
      
    if(mode_G[i] == 3) text("CROSS",(width-768)/2,100);
    else if(mode_G[i] == 4) text("Zone",(width-768)/2,100);
    
    if(zflag[i] == 1) text("END",(width-768)/2,100);
    
    textAlign(LEFT,TOP); 
    fill(255,0,0); text("Red    :  "+countR[i],(width-768)/2-80,130);
    fill(0,255,0); text("Green  :  "+countG[i],(width-768)/2-80,160);
    fill(0,0,255); text("Blue   :  "+countB[i],(width-768)/2-80,190);
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
  image(img,0,0,768,576);
  noFill(); rect(0,0,768,576); //黒枠の描画
}

// Zumoの位置を描画
void drawZumo(){
  switch(mode_G[n]){
    case 0:
    case 99:
      posx = 640; posy = 460;
      break;
    case 3:
      switch(countCross[n]){ // ここに交差点の座標を書く
        case 1:
          posx = 608; posy  = 429;
          break;
        case 2:
          posx = 416; posy  = 237;
          break;
        case 3:
          posx = 352; posy  = 224;
          break;
        case 4:
          posx = 608; posy  = 365;
          break;
      }
      break;
    case 4:
      if(LINE_COLOR == 'G'){
        switch(countB[n]){
          case 1:
            posx = 90; posy = 70;
            break;
          case 2:
            posx = 660; posy = 77;
            break;
          case 3:
            posx = 275; posy = 320;
            break;
        }
      }else{
        switch(countG[n]){
          case 1:
            posx = 90; posy = 70;
            break;
          case 2:
            posx = 660; posy = 77;
            break;
          case 3:
            posx = 493; posy = 320;
            break;
        }
      }
      break;
  }
  
  if(LINE_COLOR == 'G' && mode_G[n] != 4){
    posx = 768 - posx;
  }
  
  stroke(0); fill(255,255,0);
  ellipse(posx,posy,40,40);
  
}

//円柱の作成
// length 長さ
// radius 上面の半径
// radius 底面の半径
void pillar(float length, float radius1 , float radius2){
float x,y,z; //座標
pushMatrix();

//上面の作成
beginShape(TRIANGLE_FAN);
y = -length / 2;
vertex(0, y, 0);
for(int deg = 0; deg <= 360; deg = deg + 10){
x = cos(radians(deg)) * radius1;
z = sin(radians(deg)) * radius1;
vertex(x, y, z);
}
endShape();

//底面の作成
beginShape(TRIANGLE_FAN);
y = length / 2;
vertex(0, y, 0);
for(int deg = 0; deg <= 360; deg = deg + 10){
x = cos(radians(deg)) * radius2;
z = sin(radians(deg)) * radius2;
vertex(x, y, z);
}
endShape();

//側面の作成
beginShape(TRIANGLE_STRIP);
for(int deg =0; deg <= 360; deg = deg + 5){
x = cos(radians(deg)) * radius1;
y = -length / 2;
z = sin(radians(deg)) * radius1;
vertex(x, y, z);

x = cos(radians(deg)) * radius2;
y = length / 2;
z = sin(radians(deg)) * radius2;
vertex(x, y, z);

}
endShape();

popMatrix();
}
