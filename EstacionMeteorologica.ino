#include <WiFi.h>
#include "ThingSpeak.h"
//+++++++++Pluviomentro ++++++++++++++++++++++++++++++++++++++++++

#define PIN_LLUVIA 4          // Pin de interrupcion del Pluviometro

//Se comparte entre Pluviomentro y Anemometro  //////////////////////////////////////////////

#define INTERVALO 1000  // intervalo de 1seg por el que se obtendran medidas
#define ANTIRREBOTE 15    //referenciado en ISR / multiplique por 1000 si usa un interruptor de láminas

//////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////  Wifi //////////////////////////////////////////////////////////////////////////////////////////////////

const char* ssid = "WiFi-Arnet-69h7-2.4G";
const char* password = "pulpojulio75";

WiFiClient cliente;
////////////// ThingSpeak //////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long chanelID = 1994490;
const char* WriteAPIKey = "523Q5MWM3MTMI18V";

unsigned long tiempo3;
unsigned long tiempo4=0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned long nextCalc;
unsigned long timer;
float milimetrosLluvia = 0;

volatile unsigned int triggerLluvia = 0;
volatile unsigned long last_micros_rg;


//+++++++++++++++++++++++++++++++++++++ Anemometro  ++++++++++++++++++++++++++++++++++++++++++++++++

#define PIN_ANEMOMETRO 16 // Pin de interupcion para el anemometro

unsigned long _nextCalc;
unsigned long _timer;

volatile int _anemometerCounter;
volatile unsigned long last_micros_an;

float _windSpd;

//+++++++++++++++++++++++++++++++++++ Veleta  +++++++++++++++++++++++++++++++++++++++++++++++++++
const int windDir = 34; // pin tied to Vout for voltage divider circuit

//Estas arrays son específicos para la veleta ADS con una resistencia fija de 10kOhm en el divisor de tensin

int sensorExp[] = {226,575,994,1680,2359,2978,3451,3826};
float dirDeg[] = {90,135,180,45,225,0,315,270};
char* dirCard[] = {"E","SE","S","NE","SO","N","NO","O"};

int sensorMin[] = {162,525,949,1600,2259,2915,3321,3763};
int sensorMax[] = {290,625,1039,1760,2459,3041,3581,3889};

int analogico = 0;
float angulo = 0;
char* dir = "temp";
unsigned long tiempo1;
unsigned long tiempo2=0;

/////////////////////////////////////////////////////////////////////////


void setup() {
  //++++++  Pluviometro+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  Serial.begin(115200); 
  attachInterrupt(digitalPinToInterrupt(PIN_LLUVIA), contarLluvia, RISING); 
  
  pinMode(PIN_LLUVIA, INPUT);
  nextCalc = millis() + INTERVALO;

  //+++++ Anemometro ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  _nextCalc = millis() + INTERVALO + 10;
  pinMode(PIN_ANEMOMETRO, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_ANEMOMETRO), countAnemometer, RISING); 


  /// +++++++++++++ Wifi ++++/++++++++++++++++++++++++++++++++
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi conectado!!");  

  ThingSpeak.begin(cliente); //conectar a ThingSpeak


}



void loop() {
  //++++   Pluviometro  +++++++++++++++++++
  timer = millis();

  if(timer > nextCalc) {
    nextCalc = timer + INTERVALO;
    milimetrosLluvia = triggerLluvia * 0.2794; //calculo de lluvia en mm, cada multiplica al factor 0.2784mm
    ThingSpeak.setField(3, milimetrosLluvia);
    Serial.print("Total Tips: ");
    Serial.println(triggerLluvia);
    Serial.print("Lluvia en mm: ");
    Serial.println(milimetrosLluvia);
  }

  // Anemometro 
  _timer = millis();
  
 if(_timer > _nextCalc) {
    _nextCalc = _timer + INTERVALO;
   //actualizar y subir valor del Anemometro
    _windSpd = velocidadViento();
    Serial.println(_windSpd);
    ThingSpeak.setField(1, _windSpd);
    //Serial.print(":");    
 }
/////////////////// Veleta ////////////////////////////
 
 tiempo1=millis();
if((tiempo1 - tiempo2) >= 1000){
  tiempo2=tiempo1;
  analogico = analogRead(windDir);
  for(int i=0; i<=7; i++) {
    if(analogico >= sensorMin[i] && analogico <= sensorMax[i]) {
      dir = dirCard[i];
      break;
   } 
  }
  Serial.println(dir);
  ThingSpeak.setField(2, dir);
  
  
}



//+++++++++ ThinSpeak   +++++++++++++++++++++++++++
tiempo3=millis();
if ((tiempo3-tiempo4) >= 14000){
  tiempo4=tiempo3;
  ThingSpeak.writeFields(chanelID, WriteAPIKey); //escribir en los campos y el canal de ThingSpeak
  Serial.println("Datos enviados a ThingSpeak");
}

  
  
}  ////// Fin del loop  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++





//++++++++++++++++  Interrupcion externa Pluviometro   ++++++++++++++++++++++++++++++++++++++++++++++++

void contarLluvia() {
  // Se verifica si el tiempo transcurrido desde la última llamada de interrupción es mayor que el tiempo de rebote.  
  if((long)(micros() - last_micros_rg) >= ANTIRREBOTE*1000) {   // tiempo de antirebote
   triggerLluvia += 1;
   last_micros_rg = micros();
  }  
}


//Interrupcion externa y funcion anemometro
//devuelve la velocidad del viento desde el último calculo de intervalo.
float velocidadViento() {
 unsigned char i;  
 //long spd = 14920; // una revolucion = 1,492 millas por hora; descomentar según sea necesario para su instrumento
 float spd = 24000; // una revolucion = 2,4 km por hora; descomentar según sea necesario para su instrumento
 spd *= _anemometerCounter;
 spd /= 10000;
 _anemometerCounter = 0;
 return spd;
}

void countAnemometer() {
 if((long)(micros() - last_micros_an) >= ANTIRREBOTE * 1000) {// tiempo de antirebote
    _anemometerCounter++;
    last_micros_an = micros();
 }
}
