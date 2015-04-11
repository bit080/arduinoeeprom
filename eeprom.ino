

/* ARDUINO ATMEGA 2560 
  eeprom read write
  Pin 13 has an LED connected on most Arduino boards.
 */
// 

#define OE 38
#define WE 40
#define WELOW digitalWrite(WE,LOW)
#define WEHIGH digitalWrite(WE,HIGH)

#define OELOW digitalWrite(OE,LOW)
#define OEHIGH digitalWrite(OE,HIGH)


int led = 13;
int t = 1000;
int bfs=128;
int page = 0;
unsigned int k;
unsigned int i;
char ser[30];
int temp;
unsigned char buf[1024];


// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  
  pinMode(led, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(WE, OUTPUT);
  
  digitalWrite(OE, HIGH);
  digitalWrite(WE, HIGH);

  //DDRD = B11111111;  // Define Arduino pinos 1-7 como saídas, o pino 0 como entrada
  DDRA = B11111111; // adrl
  DDRC = B11111111; // adrh
  DDRL = B00000000; // dt
  
  Serial.setTimeout(6000);
  Serial.begin(57600);
  Serial.println(">");
  
}

// the loop routine runs over and over again forever:
void loop() {
  
  //Serial.println(Serial.available() );
  if (Serial.available()>0){
    temp = Serial.readBytesUntil('\n',ser,28);
    ser[temp]='\0';
    command();
  }
  
  /*k=
  ser[29]='\0';
  ser[28]='b';
  Serial.println(ser);
  Serial.println(k);
  for (i=0;i<39;i++){
    ser[i]='c';
  }*/
  ser[29]=0;
  // turn the LED on (HIGH is the voltage level)
  //delay(100);               // wait for a second
  //digitalWrite(led, i&1);    // turn the LED off by making the voltage LOW
  //delay(100);               // wait for a second
  
  
}

void command(){
switch (ser[0])
   {
     case 's': case 'S':
       status();
       break;

     case 'b': case 'B':
       stBufSz();
       break;
       
     case 'p': case 'P':
       stPage();
       break;
     
     case 'r': case 'R':
       rdPage();
       break;
       
     case 'c': case 'C':
       cp();
       break;
       
     case 'm': case 'M':
       bmap();
       break;
       
     case 'v': case 'V':
       vrfPage();
       break;
     
     case 'w': case 'W':
       wrPage();
       break;
       
     case 'h': case 'H':
       help();
       break;

     default:
       Serial.println("\n Erro...");
       break;
   }
   Serial.print("\x03");
}

void wr(unsigned int ad , byte dt){
  return;
  OEHIGH;
  address(ad);
  WELOW;
  DDRL = B11111111;
  PORTL = dt;
  delay(1);
  WEHIGH;
  DDRL = B00000000;
  PORTL = 255;
  
}

byte rd( unsigned int ad ){
  byte x;
  address(ad);
  DDRL = B00000000;
  OELOW;
  x = PINL;
  OEHIGH;
  return x;
  
}

void address(unsigned int ad){
  PORTA = (ad & 255);
  PORTC = (ad >> 8) & 255;
}

void SWProtDis(){
  wr(0x5555,0xAA);
  wr(0x2AAA,0x55);
  wr(0x5555,0x80);
  wr(0x5555,0xAA);
  wr(0x2AAA,0x55);
  wr(0x5555,0x20);
}

void ChipErase(){
  wr(0x5555,0xAA);
  wr(0x2AAA,0x55);
  wr(0x5555,0x80);
  wr(0x5555,0xAA);
  wr(0x2AAA,0x55);
  wr(0x5555,0x10);
}

void wrPage(){
  int z=0;
  Serial.println("Writing...");
 
  wr(0x5555,0xAA);
  wr(0x2AAA,0x55);
  wr(0x5555,0xA0);
  for (z = 0;z<bfs;z++){
    wr(bfs*page+z,buf[z]);
    if (z%16==15) Serial.print(".");
  }
  delay(1000);
  Serial.println("Done!");
}


void stBufSz(){

bfs = ((String)&ser[1]).toInt(); 
Serial.print("Size: ");
Serial.println(bfs);

}

void stPage(){

  page = ((String)&ser[1]).toInt(); 
  Serial.print("Page: ");
  Serial.println(page);

}

void rdPage(){
  int rb;
  Serial.print("Page: ");
  Serial.println(page);
   
  for (rb=0;rb<bfs;rb++){
    Serial.print(rd(bfs*page+rb),HEX);
    Serial.print(";");
  }

}


void vrfPage(){
  int rb;
  int err=0;
  Serial.print("Page: ");
  Serial.println(page);

   
  for (rb=0;rb<bfs;rb++){
    if (rd(bfs*page+rb) != buf[rb]){
      Serial.print(bfs*page+rb);
      Serial.print(" - ");
      Serial.print(buf[rb]);
      Serial.print(" - ");
      Serial.println(rd(bfs*page+rb));
      err = 1;
    } else {Serial.print(".");if (rb%16==15) Serial.println("");}
  } 
  if (err == 0) Serial.println(" OK ");
  
}


void cp(){
  int rb;
  Serial.print("Page: ");
  Serial.println(page);

   
  for (rb=0;rb<bfs;rb++){
    if (rb%8 == 0 ) Serial.println();
    buf[rb] = rd(bfs*page+rb);
    Serial.print(buf[rb]);
    Serial.print(" - ");
    
  } 

}

void help(){
 Serial.println("'s'  'S'   status");
 Serial.println("'b'  'B'   stBufSz");
 Serial.println("'p'  'P'   stPage");
 Serial.println("'r'  'R'   rdPage");
 Serial.println("'c'  'C'   cp");
 Serial.println("'m'  'M'   bmap");
 Serial.println("'v'  'V'   vrfPage");
 Serial.println("'w'  'W'   wrPage");
 Serial.println("'h'  'H'   help");
}

void bmap(){
  
  Serial.println(" map in ");
  int rb;
  
  rb=0;
  while (rb<bfs){
    if (Serial.available()==0) continue;
    temp = Serial.readBytesUntil('\n',ser,28);
    ser[temp]='\0';
    if (ser[0]=='x') break;
    buf[rb] = ((String)ser).toInt(); 
   
    Serial.print(rb);
    Serial.print(" : ");
    Serial.println(buf[rb]);
    rb++;
    
  } 
  Serial.println(" map out ");
}

void status(){
  int rb;
  Serial.print("buffer:");
  
  for (rb=0;rb<bfs;rb++){
    if (rb%8 == 0 ) Serial.println();
    Serial.print(buf[rb]);
    Serial.print(" - ");
    
  }
  Serial.println("");
  Serial.print("Page: ");
  Serial.println(page);
  Serial.print("Size: ");
  Serial.println(bfs);
}
