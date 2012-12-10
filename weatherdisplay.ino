/* Upload this sketch into Seeeduino and press reset*/
 
#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 6
#define TxD 7
#define True 1
#define False 0
 
#define DEBUG_ENABLED  1

//In RGB order for every light

/* Pin constants */
const int temp[] = {2,3,4};
const int description[] = {8,9,10};
const int precip[] = {11,12,13};
const int wind[] = {A0, A1, A2};
const int humidity[] = {A3, A4, A5};

/*int b1 = 2;
int g1 = 3;
int r1 = 4;
int b2 = 8;
int g2 = 9;
int r2 = 10;
int b3 = 11;
int g3 = 12;
int r3 = 13;
int b4 = A5;
int g4 = A4;
int r4 = A3;
int b5 = A2;
int g5 = A1;
int r5 = A0;*/

/* Input buffer */
int size_of_input = 200;
char *cloud;
char *wd;
char* inputs[6];
int i;
 
SoftwareSerial blueToothSerial(RxD,TxD);

void setup() { 
  Serial.begin(9600);
  /*for (i=0; i<3; i++) {
    pinMode(temp[i], OUTPUT);
    pinMode(description[i], OUTPUT);
    pinMode(precip[i], OUTPUT);
    pinMode(wind[i], OUTPUT);
    pinMode(humidity[i], OUTPUT);
  }*/
  /*pinMode(RxD, INPUT);
  pinMode(TxD, OUTPUT);
  pinMode(b1, OUTPUT);
  pinMode(g1, OUTPUT);
  pinMode(r1, OUTPUT);
  pinMode(b2, OUTPUT);
  pinMode(g2, OUTPUT);
  pinMode(r2, OUTPUT);
  pinMode(b3, OUTPUT);
  pinMode(g3, OUTPUT);
  pinMode(r3, OUTPUT);
  pinMode(b4, OUTPUT);
  pinMode(g4, OUTPUT);
  pinMode(r4, OUTPUT);
  pinMode(b5, OUTPUT);
  pinMode(g5, OUTPUT);
  pinMode(r5, OUTPUT);*/
  setupBlueToothConnection();
  /*digitalWrite(b1, HIGH);
  digitalWrite(g1, LOW);
  digitalWrite(r1, LOW);
  digitalWrite(b2, LOW);
  digitalWrite(g2, HIGH);
  digitalWrite(r2, LOW);
  digitalWrite(b3, LOW);
  digitalWrite(g3, LOW);
  digitalWrite(r3, HIGH);
  analogWrite(b4, 255);
  analogWrite(g4, 255);
  analogWrite(r4, 0);
  analogWrite(b5, 255);
  analogWrite(g5, 255);
  analogWrite(r5, 255);*/
} 

 
void loop() { 
  
  char recvChar;
  int transmission_complete = True;
  int transmission_count = 0;
  char input_buffer[size_of_input];
  while(1){
    // check if there's any data sent from the remote bluetooth shield	
    if(blueToothSerial.available()) {
      if (transmission_complete) {
	// start new transmission
	transmission_complete = False;
	transmission_count = 0;
      }
      recvChar = blueToothSerial.read();
      input_buffer[transmission_count++] = recvChar;
      if(recvChar == ';') {
	transmission_complete = True;
	//Serial.print(input_buffer);
	parse_update(input_buffer);
        //blink_ack();
      }
      //Serial.print(recvChar);
    }
    if(transmission_complete) {
      //update lights
    }
    //check if there's any data sent from the local serial terminal, you can add the other applications here
    if(Serial.available()){
      recvChar  = Serial.read();
      //blueToothSerial.print(recvChar);
    }
  }
}

void parse_update(char* input_buffer) {
  // Still not functional, but all the pieces are in place
  //Serial.print(input_buffer);
  char tmp[(size_of_input/2)];
  int k;
  char *t;

  cloud  = strtok_r(input_buffer, ",", &t);
  inputs[0] = cloud;
  
  wd     = strtok_r(NULL, ",", &t);
  inputs[1] = wd;

  for (k=2; k<6; k++) {
    inputs[k] = strtok_r(NULL, ",", &t);
  }

  k = 0;
  for (k = 0; k < 6; k++) {
    Serial.println("Variable: "+ k);
    Serial.println(inputs[k]);
  }
}
 
void setupBlueToothConnection()
{
  blueToothSerial.begin(38400); //Set BluetoothBee BaudRate to default baud rate 38400
  blueToothSerial.print("\r\n+STWMOD=0\r\n"); //set the bluetooth work in slave mode
  blueToothSerial.print("\r\n+STNA=SeeedBTSlave\r\n"); //set the bluetooth name as "SeeedBTSlave"
  blueToothSerial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  blueToothSerial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  delay(2000); // This delay is required.
  blueToothSerial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable 
  Serial.println("The slave bluetooth is inquirable!");
  delay(2000); // This delay is required.
  blueToothSerial.flush();
}
