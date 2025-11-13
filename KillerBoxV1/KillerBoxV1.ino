#include <Wire.h>

/*
  Código para el robot mini sumo-- KILLERBOX
  Robot capaz de detectar obstáculos y reaccionar de acuerdo a la situación
  con Sensores de piso para detectar el borde de la arena
  y estrategias para empujar al oponente fuera del ring

  creado por: Victor Becerra--- github: BecerraSoftware
*/

#define delLeft A2
#define delRigh A6
#define delForward A1

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
  
}

bool DetectaBlanco(int pin){return analogRead(pin)<BLANCO;}
int DetectarPiso(int pin){return analogRead(pin);}
bool DetectarObstaculo(int pin){return analogRead(pin)>500?1:0;}

void loop(){
  Serial.println(DetectarObstaculo());
}

void loopMAIN() {
  if (digitalRead(4) == 1) {  // Botón de inicio

    bool obstAde = DetectarObstaculo(delForward);
    bool pisoIzq = DetectaBlanco(floorLeft);
    bool pisoDer = DetectaBlanco(floorRigh);
    bool obstIzq = DetectarObstaculo(delLeft);
    bool obstDer = DetectarObstaculo(delRigh);

    // 🔹 1. EVITAR SALIR DEL DOYO
    if (pisoIzq || pisoDer) {
      Atras(220, 300);
      
      unsigned long startTime = millis();
      if (random(0, 2) == 0) { // Gira izquierda o derecha aleatoriamente
        while (millis() - startTime < 300 && !DetectarObstaculo(delForward)) {
          Avanzar(200, 1); // Giro derecha
          if (DetectaBlanco(floorLeft) || DetectaBlanco(floorRigh)) break;
        }
      } else {
        while (millis() - startTime < 300 && !DetectarObstaculo(delForward)) {
          Avanzar(200, 2); // Giro izquierda
          if (DetectaBlanco(floorLeft) || DetectaBlanco(floorRigh)) break;
        }
      }
      stop();
      return;
    }
  
    // 🔹 2. LÓGICA DE PERSECUCIÓN Y ATAQUE
    if (obstAde) {
      Avanzar(255, 0);
      delay(30);
      return;
    }

    if (obstIzq && obstDer) {
      Avanzar(230, 0);
      delay(30);
      return;
    }

    // 🔹 Giro por detección lateral izquierda
    if (obstIzq && !obstDer) {
      unsigned long startTime = millis();
      while (millis() - startTime < 1000) {
        Avanzar(80, 2);
        if (DetectarObstaculo(delForward)) {
          Avanzar(255, 0);
          delay(50);
          break;
        }
        if (DetectaBlanco(floorLeft) || DetectaBlanco(floorRigh)) break;
      }
      stop();
      return;
    }

    // 🔹 Giro por detección lateral derecha
    if (obstDer && !obstIzq) {
      unsigned long startTime = millis();
      while (millis() - startTime < 1000) {
        Avanzar(80, 1);
        if (DetectarObstaculo(delForward)) {
          Avanzar(255, 0);
          delay(50);
          break;
        }
        if (DetectaBlanco(floorLeft) || DetectaBlanco(floorRigh)) break;
      }
      stop();
      return;
    }

    // 🔹 3. BÚSQUEDA (gira lentamente hasta encontrar algo)
    unsigned long startTime = millis();
    while (millis() - startTime < 200) {
      if(random(0, 2) == 0){
        Avanzar(60, 1); // Gira lentamente a la derecha
      }
      else{
        Avanzar(60,2);
      }
      if (DetectarObstaculo(delForward) || DetectarObstaculo(delLeft) || DetectarObstaculo(delRigh))
        break;
    }
    stop();
  } 
  else {
    stop();
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