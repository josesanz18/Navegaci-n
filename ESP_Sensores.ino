//********************************Variables para Configuración AccessPoint*********************
#include <WiFi.h>
const char* ssid     = "ESP32-Access-Point-2";
const char* password = "123456789";
WiFiServer server(80);
String header;

//Entradas
#define PE1 digitalRead(pe1)
#define PE2 digitalRead(pe2)
#define PE3 digitalRead(pe3)


//Salidas
#define   TS0   0          //Pines para enviar al Arduino Uno
#define   TS1   2          //Pines para enviar al Arduino Uno
#define   TS2   15           //Pines para enviar al Arduino Uno

//Detección de color
#define S0 36         // S0 a pin 4
#define S1 39         // S1 a pin 5
#define S2 34         // S2 a pin 6
#define S3 35         // S3 a pin 7
#define salidaTCS 32  // salidaTCS a pin 8

// DECLARACION DE VARIABLES PARA PINES
const int pe1 = 19;
const int pe2 = 18;
const int pe3 = 5;
const int pintrigger = 4;
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
int rvaExterior[3] = {0,0,0};

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
  Serial.println("Término del setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("LOOP");
  //Modo Búsqueda
  Busqueda:
  Serial.println("Modo búsqueda");
    Nav = false;                //bandera navegacion apagada
    goto lecSens;               //leer sensores
    if(objP == 3){              //si no se detectan pelotas
      Serial.println("Espiral");
      digitalWrite(TS0,LOW);      //girar espiral 1 0 0
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,HIGH);
      goto Busqueda;                //leer sensores
      }
    else{                         //si se detectan pelotas
      digitalWrite(TS0,LOW);      //detener 0 0 0
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      Nav = true;                 //bandera de navegacion activa
      goto Navegacion;            //Modo Navegacion
    }
    

  //Módo navegación
  Navegacion:
    Serial.println("Modo Navegación");
    if(objP == 1){                  //Pelota a la derecha
      Serial.print("Derecha");
      digitalWrite(TS0,HIGH);       //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      goto lecSens;                 //leer sensores     
    }
    if(objP == 2){                //Pelota a la izquierda
      Serial.print("Izquierda");
      digitalWrite(TS0,LOW);      //girar izquierda 0 1 0
      digitalWrite(TS1,HIGH);
      digitalWrite(TS2,LOW);
      goto lecSens;               //leer sensores 
    }
    if(objP == 0){                //Pelota centrada
      Serial.print("Frente");
      digitalWrite(TS0,LOW);      //adelante 0 1 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,HIGH);
      goto lecSens;               //leer sensores
    }

  //Modo Obstáculos
  Obstaculos:
    Serial.println("Modo Obstáculos");
    evObs = true;                 //bandera de evacion de obstaculos activa
    digitalWrite(TS0,LOW);          //Detener motores 0 0 0
    digitalWrite(TS1,LOW);
    digitalWrite(TS2,LOW);
    delay(250);                   //Esperar pequeño tiempo, evita cambio de polaridad brusca en motores
    if(obs == 1){                 //Obstáculo a la derecha
      digitalWrite(TS0,LOW);      //girar izquierda 0 1 0
      digitalWrite(TS1,HIGH);
      digitalWrite(TS2,LOW);
      goto lecSens;               //leer sensores
      return;
    }
    if(obs == 2){                 //Obstáculo a la izquierda
      digitalWrite(TS0,HIGH);       //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      goto lecSens;               //leer sensores
      return;
    }
    if(obs == 3){                 //Obstáculo al frente
      digitalWrite(TS0,HIGH);     //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      goto lecSens;               //leer sensores
      return;
    }
    /*if(obs == 4){                 //Obstáculo atrás
      digitalWrite(TS0,HIGH);       //girar derecha 0 0 1
      digitalWrite(TS1,LOW);
      digitalWrite(TS2,LOW);
      goto lecSens;                 //leer sensores
      return;
    }*/
    if(limC){                     //Detección del límite de cancha
      goto lecSens;               //leer sensores
      digitalWrite(TS0,LOW);      //girar izquierda 0 1 0
      digitalWrite(TS1,HIGH);
      digitalWrite(TS2,LOW);
      delay(1000);                //control de giro Izquierdo
      goto lecSens;               //leer sensores
      return;
    } 

  lecSens:
    Serial.println("Leer Sensores");
    //Sección para asignar valores de los sensores y cuantizarlos
    objP = visionPelotas();                                                //Busca centro de pelota, 0-centro,1-derecha,2-izquierda, 3-sin objetivo
    obs = lecObs();                       //
    rva[0] = lecturaColor('r');
    rva[1] = lecturaColor('v');
    rva[2] = lecturaColor('a');
    Serial.print("Obs: ");
    Serial.print(obs);
    Serial.print(" limC:" );
    Serial.print(limC);
    Serial.print(" Nav: ");
    Serial.print(Nav);
    Serial.print(" evObs: ");
    Serial.println(evObs);
    if((rva[0] == rvaExterior[0]) && (rva[1] == rvaExterior[1]) && (rva[2] == rvaExterior[2])){
      limC = true;
    }
    else{
      limC = false;
    }
    if( Nav && (limC || obs ==3)){                                  //Si se encuentra un obstáculo enfrente, o se determina el límite de cancha mientras navega
      goto Obstaculos;
    }
    if( evObs && (limC || obs != 0)){                               //Si se encuentra evadiendo Obtáculos
      goto Obstaculos;
    }
    if(!limC && obs == 0){
      evObs == false;                                               //Bandera de evación de obstáculos desactivada
    }
    return;
}

void accessPointInit(){
  WiFi.softAP(ssid, password);
  Serial.print("AccessPoint IP address: ");
  Serial.println(WiFi.softAPIP());
  server.begin();
}

int visionPelotas(){
  Serial.println("Buscando Pelotas");
  int ball;
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
    ball = 3;
  }
  return ball;
} 

int lecObs(){
  Serial.println("Buscando Obstáculos");
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

  /*b00:
  Serial.println("Buscando Obstáculos b00");
  if(PE1 == LOW && PE2 == LOW){
    delayMicroseconds(1);
    goto b00;
  }*/

  bi:
  Serial.println("Buscando Obstáculos bi");
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
  Serial.println("Buscando Obstáculos bf");

  //Por calibraciÃ³n, 0.5 fue el mejor valor, al menos de manera preliminar... 
  distancia1 = y1 * 0.5;
  distancia2 = y2 * 0.5;
  distancia3 = y3 * 0.5; 
 
  //Serial
  Serial.print(distancia1);
  Serial.print(" --cm[1] ");
  Serial.print(distancia2);
  Serial.println(" --cm[2] ");
  delay(3);
  return 0;
}

int lecturaColor(char col){
   Serial.println("Leyendo Cancha");
   //rojo
   if(col == 'r'){
      digitalWrite(S2,LOW);     // establece fotodiodos
      digitalWrite(S3,LOW);     // con filtro rojo
   }
   //verde
   if(col == 'v'){
      digitalWrite(S2,HIGH);     // establece fotodiodos
      digitalWrite(S3,HIGH);     // con filtro verde
   }
   //azul
   if(col == 'a'){
      digitalWrite(S2,LOW);     // establece fotodiodos
      digitalWrite(S3,HIGH);     // con filtro azul
   }
 
  int R = pulseIn(salidaTCS, LOW); // obtiene duracion de pulso de salida del sensor
  delay(3);          // demora de 200 mseg 

  return R;
}
