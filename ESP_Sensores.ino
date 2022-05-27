//********************************Variables para Configuración AccessPoint*********************
#include <WiFi.h>
const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";
WiFiServer server(80);
String header;

//Entradas
#define PE1 digitalRead(pe1)
#define PE2 digitalRead(pe2)
#define PE3 digitalRead(pe3)


//Salidas
#define   TS0   4         //Pines para enviar al Arduino Uno
#define   TS1   0          //Pines para enviar al Arduino Uno
#define   TS2   2           //Pines para enviar al Arduino Uno
#define   TS3   15

//Detección de color
#define S0 32         // S0 a pin 4
#define S1 33         // S1 a pin 5
#define S2 25         // S2 a pin 6
#define S3 26         // S3 a pin 7
#define salidaTCS 27  // salidaTCS a pin 8

// DECLARACION DE VARIABLES PARA PINES
const int pe1 = 19;
const int pe2 = 18;
const int pe3 = 5;
const int pintrigger = 21;
// VARIABLES PARA CALCULOS
unsigned int distancia1, distancia2, distancia3;

// varaibles globales
short obs;                  //Obstaculos
short objP = 3;             //Posición de la pelota
boolean limC = false;       //Detección de límite de cancha
boolean Nav = false;        //Bandera de Navegacion
boolean evObs = false;      //Bandera de Evación de obstáculos
short i;                    //Varaible Auxiliar

//Variables detección de color
int rva[3] = {0,0,0};
int rvaExterior[3] = {87,67,38};

void setup() {
  // put your setup code here, to run once:
  // PREPARAR LA COMUNICACION SERIAL
  Serial.begin(9600);
  // Entradas
  pinMode(pe1, INPUT);          //Pines Ultrasónico
  pinMode(pe2, INPUT);
  pinMode(pe3, INPUT);      
  
  //Salidas
  pinMode(TS0,OUTPUT);          //declarar como salida los pines de comunicación con el Arduino
  pinMode(TS1,OUTPUT);          //declarar como salida los pines de comunicación con el Arduino
  pinMode(TS2,OUTPUT);          //declarar como salida los pines de comunicación con el Arduino
  pinMode(TS3,OUTPUT);
  pinMode(pintrigger, OUTPUT);  //pines Ultrasónico

  //Color
  pinMode(S0, OUTPUT);          // pin S0 como salida
  pinMode(S1, OUTPUT);          // pin S1 como salida
  pinMode(S2, OUTPUT);          // pin S2 como salida
  pinMode(S3, OUTPUT);          // pin S3 como salida
  pinMode(salidaTCS, INPUT);    // pin OUT como salida
  digitalWrite(S0,HIGH);        // establece frecuencia de salida
  digitalWrite(S1,LOW);         // del modulo al 20 por ciento
  
  accessPointInit();            //Inicializa el punto de acceso del ESP32
  //Serial.println("Término del setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("LOOP");
  lecSens();
  /*if( Nav && (limC || obs ==3)){                                  //Si se encuentra un obstáculo enfrente, o se determina el límite de cancha mientras navega
    goto Obstaculos;
  }
  if( evObs && (limC || obs != 0)){                               //Si se encuentra evadiendo Obtáculos
    goto Obstaculos;
  }/*/
  if(limC || obs!=0){
    goto Obstaculos;
  }
  if(limC == 0 && obs == 0){
    evObs == false;                                               //Bandera de evación de obstáculos desactivada
  }
  //Modo Búsqueda
  Busqueda:
  Serial.println("Modo búsqueda");
    Nav = false;             //bandera navegacion apagada
    if(objP == 3){              //si no se detectan pelotas
      Serial.println("Espiral");
      digitalWrite(TS0,LOW);      //girar espiral 1 0 0
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,HIGH);
      return;                //leer sensores
      }
    else{                         //si se detectan pelotas
      /*digitalWrite(TS0,LOW);      //detener 0 0 0
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);*/
      Nav = true;                 //bandera de navegacion activa
      goto Navegacion;            //Modo Navegacion
    }

  //Módo navegación
  Navegacion:
    Serial.print("Modo Navegación");
    if(objP == 1){                  //Pelota a la derecha
      Serial.println("Derecha");
      digitalWrite(TS0,HIGH);       //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      digitalWrite(TS3,HIGH);
      return;    
    }
    if(objP == 2){                //Pelota a la izquierda
      Serial.println("Izquierda");
      digitalWrite(TS0,LOW);      //girar izquierda 0 1 0
      digitalWrite(TS1,HIGH);
      digitalWrite(TS2,LOW);
      digitalWrite(TS3,HIGH);
      return;               //leer sensores 
    }
    if(objP == 0){                //Pelota centrada
      Serial.println("Frente");
      digitalWrite(TS0,HIGH);      //adelante 0 1 1
      digitalWrite(TS1,HIGH);
      digitalWrite(TS2,LOW);
      digitalWrite(TS3,HIGH);
      return;               //leer sensores
    }
  //Modo Obstáculos
  Obstaculos:
    Serial.println("Modo Obstáculos");
    evObs = true;                 //bandera de evacion de obstaculos activa
    digitalWrite(TS0,LOW);          //Detener motores 0 0 0
    digitalWrite(TS1,LOW);
    digitalWrite(TS2,LOW);
    digitalWrite(TS3,HIGH);
    delay(2000);
    digitalWrite(TS0,HIGH);          //atras 1 1 1
    digitalWrite(TS1,HIGH);
    digitalWrite(TS2,HIGH);
    digitalWrite(TS3,HIGH);
    delay(2000);
    if(obs == 1){                 //Obstáculo a la derecha
      digitalWrite(TS0,LOW);      //girar izquierda 0 1 0
      digitalWrite(TS1,HIGH);
      digitalWrite(TS2,LOW);
      digitalWrite(TS3,HIGH);
      delay(3000);
      return;
    }
    if(obs == 2){                 //Obstáculo a la izquierda
      digitalWrite(TS0,HIGH);       //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      digitalWrite(TS3,HIGH);
      delay(3000);
      return;
    }
    if(obs == 3){                 //Obstáculo al frente
      digitalWrite(TS0,HIGH);     //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      digitalWrite(TS3,HIGH);
      delay(3000);
      return;
    }
    /*if(obs == 4){                 //Obstáculo atrás
      digitalWrite(TS0,HIGH);       //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      lecSens();                 //leer sensores
      return;
    }*/
    if(limC){                     //Detección del límite de cancha
      digitalWrite(TS0,LOW);      //girar izquierda 0 1 0
      digitalWrite(TS1,HIGH);
      digitalWrite(TS2,LOW);
      digitalWrite(TS3,HIGH);
      delay(3000);
      return;
    } 
}

void lecSens(){
    //Serial.println("Leer Sensores");
    //Sección para asignar valores de los sensores y cuantizarlos
    digitalWrite(TS3,LOW);
    lecObsMike();
    objP = visionPelotas();                                                //Busca centro de pelota, 0-centro,1-derecha,2-izquierda, 3-sin objetivo
    //lecObs();                                           //
    lecturaColor();
    if((rva[0] <= (rvaExterior[0] + 25) && rva[0] >= (rvaExterior[0] - 25)) && (rva[1] <= (rvaExterior[1] + 25) && rva[1] >= (rvaExterior[1] - 25)) && (rva[2] <= (rvaExterior[2] + 25) && rva[2] >= (rvaExterior[2] - 25))){
      limC = true;
    }
    else{
      limC = false;
    }
    
    Serial.print("Obs: ");
    Serial.print(obs);
    Serial.print(" ObjP:");
    Serial.print(objP);
    Serial.print(" limC:" );
    Serial.print(limC);
    Serial.print(" Nav: ");
    Serial.print(Nav);
    Serial.print(" evObs: ");
    Serial.println(evObs);
    Serial.print("TS3: ");
    Serial.print(digitalRead(TS3));
    Serial.print(" TS2: ");
    Serial.print(digitalRead(TS2));
    Serial.print(" TS1: ");
    Serial.print(digitalRead(TS1));
    Serial.print(" TS0: ");
    Serial.println(digitalRead(TS0));
}

void accessPointInit(){
  WiFi.softAP(ssid, password);
  Serial.print("AccessPoint IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

int visionPelotas(){
  //Serial.println("Buscando Pelotas");
  int ball;
  Conexion:
  WiFiClient client = server.available();     // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
     
            if (header.indexOf("right") >= 0){ 
              Serial.println("Pelota a la derecha");
              ball=1;
            }
            if (header.indexOf("left") >= 0){ 
              Serial.println("Pelota a la izquierda");
              ball=2;
            }
            if (header.indexOf("null") >= 0) {
              Serial.println("NO se detecta pelota");
              ball=3;
            }
            if (header.indexOf("center") >= 0){
              Serial.println("Pelota en el centro");
              ball=0;
            }
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  else{
    Serial.println("No hay coneccion python");
    digitalWrite(TS0,LOW);          //Detener motores 0 0 0
    digitalWrite(TS1,LOW);
    digitalWrite(TS2,LOW);
    digitalWrite(TS3,HIGH);
    goto Conexion;  
    ball = 3;                                           //
  }
  return ball;
} 

void lecObs(){
  //Serial.println("Buscando Obstáculos");
  int pose;
  // ENVIAR PULSO DE DISPARO EN EL PIN "TRIGGER"
  digitalWrite(pintrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pintrigger, HIGH);
  // EL PULSO DURA AL MENOS 10 uS EN ESTADO ALTO
  delayMicroseconds(10);
  digitalWrite(pintrigger, LOW);

  unsigned int y1 = 0;
  unsigned int y2 = 0;
  unsigned int y3 = 0;

  
  int espera = 0;

  Serial.println("Buscando Obstáculos b00");
  b00:
  if(PE1 == LOW && PE2 == LOW && PE3==LOW){
    delayMicroseconds(1);
    //Serial.print(".");
    espera = espera+1;
    if(espera > 1000){
      return;
    }
    
    goto b00;
  }

  Serial.println("ESPERA");

  bi:
  //Serial.println("Buscando Obstáculos bi");
    if(PE1 == HIGH){
      y1++;
    }
    if(PE2 == HIGH){
      y2++;
    }
    if(PE3 == HIGH){
      y3++;
    }
    
    /*
    if(PE4 == HIGH){
      y4++;
    }*/

    if(PE1 == LOW && PE2 == LOW && PE3 == LOW){
    //Descomentar para usar cuatro sensores
    //if(PE1 == LOW && PE2 == LOW && PE3 == LOW && PE4 == LOW){ 
      goto bf;
    }
  
  delayMicroseconds(1);
  goto bi;
  bf:
  //Serial.println("Buscando Obstáculos bf");

  //Por calibraciÃ³n, 0.5 fue el mejor valor, al menos de manera preliminar... 
  distancia1 = y1 / 300;
  distancia2 = y2 / 300;
  distancia3 = y3 / 300; 
 
  //Serial
  Serial.print(distancia1);
  Serial.print(" --cm[1] ");
  Serial.print(distancia2);
  Serial.print(" --cm[2] ");
  Serial.print(distancia3);
  Serial.println(" --cm[3] ");
  delay(3);
  
  if(distancia1<30&&distancia1>0){
    obs=2;
    return;
  }
  if(distancia3<30&&distancia3>0){
    obs=1;
    return;
  }
  if(distancia2<70&&distancia2>0){
    obs=3;
    return;
  }
  else{
    obs=0;
  }
}

void lecObsMike(){

  long t; //timepo que demora en llegar el eco
  long d1,d2,d3; //distancia en centimetros

  digitalWrite(pintrigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(pintrigger, LOW);
  
  t = pulseIn(pe1, HIGH); //obtenemos el ancho del pulso
  d1 = t/59;             //escalamos el tiempo a una distancia en cm
  //if(d<20||d>100){
  Serial.print("Distancia1: ");
  Serial.print(d1);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();

  digitalWrite(pintrigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(pintrigger, LOW);
  t = pulseIn(pe2, HIGH); //obtenemos el ancho del pulso
  d2 = t/59;             //escalamos el tiempo a una distancia en cm
  //if(d<20||d>100){
  Serial.print("Distancia2: ");
  Serial.print(d2);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();

  digitalWrite(pintrigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(pintrigger, LOW);
  t = pulseIn(pe3, HIGH); //obtenemos el ancho del pulso
  d3 = t/59;             //escalamos el tiempo a una distancia en cm
  //if(d<20||d>100){
  Serial.print("Distancia3: ");
  Serial.print(d3);      //Enviamos serialmente el valor de la distancia
  Serial.print("cm");
  Serial.println();
  //delay(100);
  if(d1<30&&d1>0){
    obs=2;
    return;
  }
  if(d3<30&&d3>0){
    obs=1;
    return;
  }
  if(d2<20&&d2>0){
    obs=3;
    return;
  }
  else{
    obs=0;
  }
  //}          //Hacemos una pausa de 100ms
}

void lecturaColor(){
   //Serial.println("Leyendo Cancha");
   //rojo
      digitalWrite(S2,LOW);     // establece fotodiodos
      digitalWrite(S3,LOW);     // con filtro rojo
      rva[0] = pulseIn(salidaTCS, LOW);// obtiene duracion de pulso de salida del sensor
  //verde
      digitalWrite(S2,HIGH);     // establece fotodiodos
      digitalWrite(S3,HIGH);     // con filtro verde
      rva[1] = pulseIn(salidaTCS, LOW);// obtiene duracion de pulso de salida del sensor
   //azul
      digitalWrite(S2,LOW);     // establece fotodiodos
      digitalWrite(S3,HIGH);     // con filtro azul
      rva[2] = pulseIn(salidaTCS, LOW);// obtiene duracion de pulso de salida del sensor
    Serial.print("R: ");
    Serial.print(rva[0]);
    Serial.print(" V: ");
    Serial.print(rva[1]);
    Serial.print(" A: ");
    Serial.println(rva[2]);
}
