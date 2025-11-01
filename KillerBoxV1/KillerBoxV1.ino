#include <Wire.h>
#include <VL53L0X.h>
VL53L0X sensor;

/*
  Código para el robot mini sumo-- KILLERBOX
  Robot capaz de detectar obstáculos y reaccionar de acuerdo a la situación
  con Sensores de piso para detectar el borde de la arena
  y estrategias para empujar al oponente fuera del ring

  creado por: Victor Becerra--- github: BecerraSoftware
*/

#define delLeft A2
#define delRigh A6

//Cuenta con dos sensores de piso que detectan el color blanco para no salir del ring
#define floorLeft A0
#define floorRigh A7

//Pines de motores
#define MPos_Left 9
#define MNeg_Left 8
#define PWM_LEFT 10

#define MPos_Righ 6
#define MNeg_Righ 7
#define PWM_RIGH 5

//variables de dipswich
const int DipSwith1 =2;
const int DipSwith2 =3;

/*
Variables del sistema del robot
BLANCO==> Del sensor de piso que tan intennso detecta el blanco para determinar que movimiento
se efectua
*/

//sensores de piso
#define BLANCO 400


void setup() {
    //Sensor para vl53x
    Wire.begin();
    if (!sensor.init()) {
    Serial.println("Error al iniciar VL53L0X");
    while (1);
  }
  Serial.println("Se inicializo");
  sensor.setTimeout(500);

  sensor.setMeasurementTimingBudget(90000);
  sensor.startContinuous(0);
  Serial.println("Sensor VL53L0X iniciado en modo continuo");
  delay(2000);

 
  
  //SENSORES
  pinMode(delRigh,INPUT);
  pinMode(delLeft,INPUT);
  //Sensores de piso
  pinMode(floorRigh,INPUT);
  pinMode(floorLeft,INPUT);

  //MOTORES
  pinMode(MPos_Righ, OUTPUT);
  pinMode(MNeg_Righ, OUTPUT);
  pinMode(PWM_RIGH, OUTPUT);

  pinMode(MPos_Left,OUTPUT);
  pinMode(MNeg_Left,OUTPUT);
  pinMode(PWM_LEFT,OUTPUT);
  pinMode(4,INPUT);
  
  stop();

  //dipswith
  pinMode(DipSwith1,INPUT);
  pinMode(DipSwith2,INPUT);
  
  Serial.begin(9600); // Monitor serial
}

bool DetectaBlanco(int pin){return analogRead(pin)<BLANCO;}
int DetectarPiso(int pin){return analogRead(pin);}
bool DetectarObstaculo(int pin){return analogRead(pin)>500?1:0;}
bool Vl53x(int distancia){return distancia<500?true:false;}
const int DISTANCIA_MAX= 200;

void loop() {
  if (digitalRead(4) == 1) {
     // Botón de inicio
    int distancia = sensor.readRangeContinuousMillimeters();
    bool pisoIzq = DetectaBlanco(floorLeft);
    bool pisoDer = DetectaBlanco(floorRigh);
    bool obstIzq = DetectarObstaculo(delLeft);
    bool obstDer = DetectarObstaculo(delRigh);

    /* 🔹 1. EVITAR SALIR DEL DOYO
    if (pisoIzq && pisoDer) {
      Serial.println("¡Borde detectado! Retrocede y gira aleatoriamente");
      Atras(220, 300);
      (random(0, 2) == 0) ? Avanzar(200, 1) : Avanzar(200, 2);
      delay(300);
      stop();
      return;
    }
    else if (pisoIzq) {
      Serial.println("Borde izquierdo detectado → retrocede y gira derecha");
      Atras(220, 250);
      Avanzar(200, 2);
      delay(300);
      stop();
      return;
    }
    else if (pisoDer) {
      Serial.println("Borde derecho detectado → retrocede y gira izquierda");
      Atras(220, 250);
      Avanzar(200, 1);
      delay(300);
      stop();
      return;
    }*/
/*
    // 🔹 2. LÓGICA DE PERSECUCIÓN Y ATAQUE
    if (Vl53x(distancia)) {
      // Si ya ve al enemigo con el VL53X, avanza directo
      Serial.println("VL53X detecta al enemigo → ataque directo");
      Avanzar(255, 0);
      delay(30);
      return;
      }

    // Si detecta ambos sensores frontales → enemigo al frente
    if (obstIzq && obstDer) {
      Serial.println("Enemigo al frente (ambos sensores) → avanzar directo");
      Avanzar(230, 0);
      delay(30);
      return;
      }

    // Si detecta solo el sensor izquierdo → girar hasta verlo con el VL53X
    if (obstIzq && !obstDer) {
      Serial.println("Oponente detectado por izquierda → girando hasta verlo con VL53X");
      unsigned long startTime = millis();
      while (millis() - startTime < 1000) { // evita que se quede trabado
        Avanzar(80, 2);  // Gira a la izquierda
        int distanciaTemp = sensor.readRangeContinuousMillimeters();
        if (Vl53x(distanciaTemp)) {
          Serial.println("VL53X lo detectó → atacar");
          Avanzar(255, 0);
          delay(50);
          break;
        }
        if (DetectaBlanco(floorLeft) || DetectaBlanco(floorRigh)) break; // seguridad
      }
      stop();
      return;
    }

  // Si detecta solo el sensor derecho → girar hasta verlo con el VL53X
    if (obstDer && !obstIzq) {
      Serial.println("Oponente detectado por derecha → girando hasta verlo con VL53X");
      unsigned long startTime = millis();
      while (millis() - startTime < 1000) {
        Avanzar(80, 1);  // Gira a la derecha
        int distanciaTemp = sensor.readRangeContinuousMillimeters();
        if (Vl53x(distanciaTemp)) {
          Serial.println("VL53X lo detectó → atacar");
          Avanzar(255, 0);
          delay(50);
          break;
        }
        if (DetectaBlanco(floorLeft) || DetectaBlanco(floorRigh)) break;
      }
      stop();
      return;
  } 

  // 🔹 3. Si no detecta nada → buscar
  Serial.println("Buscando enemigo...");
  Avanzar(50, 0);
  delay(80);*/
  }
  else{
  stop();
  delay(30);
  }
}



  
void Avanzar(float velocidad, int opcion){    
  /*
 Función Avanzar:
   - velocidad: valor PWM (0 a 255)
   - opcion:
       0 -> ambos motores hacia adelante (avance)
       1 -> giro a la izquierda: motor izquierdo en reversa, motor derecho hacia adelante
       2 -> giro a la derecha: motor izquierdo hacia adelante, motor derecho en reversa
       3 -> giro en el sitio: motores en direcciones opuestas a velocidad moderada
       4 -> rutina cuando vea la linea.
  */
    switch(opcion) {
      case 0:
        //derecha adelante
        digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);

        //izquierda adelante
        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);
        break;
      
      case 1: // Giro a la izquierda
       
        //DERECHA ADELANTE
        digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);

         //IZQUIERDA ATRAS
         digitalWrite(MPos_Righ, HIGH);
         digitalWrite(MNeg_Righ, LOW);
         analogWrite(PWM_RIGH, velocidad);
        break;
      
      case 2: 
        //izquierda adelante
        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);

        //derecha atras
         digitalWrite(MPos_Left, LOW);
         digitalWrite(MNeg_Left, HIGH);
         analogWrite(PWM_LEFT, velocidad);
        
        break;
      
      case 3:
        //izquierda adelante
        digitalWrite(MPos_Left, LOW);
        digitalWrite(MNeg_Left, HIGH);//LOW
        analogWrite(PWM_LEFT, velocidad);

        //derecha atras
        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);
        break;
      case 4: {
        Atras(220, 350);
        stop();
        delay(40);
        digitalWrite(MPos_Left, LOW);
        digitalWrite(MNeg_Left, HIGH);//LOW
        analogWrite(PWM_LEFT, velocidad);

        //derecha atras
        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);  
        return;
      }
  }
  delay(20);  // Pequeño retardo para estabilidad
}
void Atras(float velocidad,int tiempo){
              digitalWrite(MPos_Left, LOW);
        digitalWrite(MNeg_Left, HIGH);
        analogWrite(PWM_LEFT, velocidad);      
      //DERCHA ATRAS
        digitalWrite(MPos_Righ, HIGH);
        digitalWrite(MNeg_Righ, LOW);
        analogWrite(PWM_RIGH, velocidad);
        delay(tiempo);
        stop();
}
void stop(){
  digitalWrite(MPos_Left,LOW);
  digitalWrite(MNeg_Left,LOW);
  analogWrite(PWM_LEFT,0);

  digitalWrite(MPos_Righ,LOW);
  digitalWrite(MPos_Righ,LOW);
  analogWrite(PWM_RIGH,0);
}
/*void loopxd(){

  while(!DetectarObstaculo(delForward)){
    while(DetectarObstaculo(delLeft)){
      Evadir(100,2);
      if(!DetectarObstaculo(delLeft)){break;}
    }
    while(DetectarObstaculo(delRigh)){
      Evadir(100,1);
      if(!DetectarObstaculo(delRigh)){break;}
  }
  stop();
    
  

  }
}*/