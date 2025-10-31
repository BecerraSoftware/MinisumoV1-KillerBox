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
  
  stop();

  //dipswith
  pinMode(DipSwith1,INPUT);
  pinMode(DipSwith2,INPUT);
  
  Serial.begin(9600); // Monitor serial



  
  delay(2000);//quitar esto si es necesario
}

bool DetectaBlanco(int pin){return analogRead(pin)<BLANCO;}
int DetectarPiso(int pin){return analogRead(pin);}
bool DetectarObstaculo(int pin){return analogRead(pin)>500?1:0;}
bool Vl53x(int distancia){return distancia<200?true:false;}
//loopPrincipal LO MAS OPTIMIZADO POSIBLE Y RAPIDO PARA GANAR Y QUE SI TIENE EL DIPSWITCH ACTIVADO EL 1 CUANDO EL VL53X DETECTE E UN RANGO ESQUIVE EL ROBOT
void loop() {
  static bool scanDirection = false;
  static bool sidestepLeft = false;
  static unsigned long lastScanFlip = 0;

  int distancia = sensor.readRangeContinuousMillimeters();
  bool timeout = sensor.timeoutOccurred();
  bool frontClose = !timeout && Vl53x(distancia);

  bool leftEdge = DetectaBlanco(floorLeft);
  bool rightEdge = DetectaBlanco(floorRigh);
  bool leftContact = digitalRead(delLeft) == HIGH;
  bool rightContact = digitalRead(delRigh) == HIGH;

  bool evadeMode = digitalRead(DipSwith1) == HIGH;
  bool aggressiveMode = digitalRead(DipSwith2) == HIGH;

  if (leftEdge && rightEdge) {
    Atras(200, 250);
    return;
  }
  if (leftEdge) {
    Avanzar(220, 2);
    return;
  }
  if (rightEdge) {
    Avanzar(220, 1);
    return;
  }

  if (evadeMode && frontClose && !leftContact && !rightContact) {
    sidestepLeft = !sidestepLeft;
    Avanzar(230, sidestepLeft ? 1 : 2);
    return;
  }

  if (leftContact && rightContact) {
    Avanzar(255, 0);
    return;
  }
  if (leftContact) {
    Avanzar(240, 1);
    return;
  }
  if (rightContact) {
    Avanzar(240, 2);
    return;
  }

  if (frontClose) {
    Avanzar(255, 0);
    return;
  }

  unsigned long now = millis();
  if (now - lastScanFlip > 600) {
    scanDirection = !scanDirection;
    lastScanFlip = now;
  }

  Avanzar(aggressiveMode ? 220 : 200, scanDirection ? 1 : 2);
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

        bool pivotLeft = (millis() & 0x01);
        if (pivotLeft) {
          // pivote hacia la izquierda
          digitalWrite(MPos_Left, LOW);
          digitalWrite(MNeg_Left, HIGH);
          analogWrite(PWM_LEFT, 210);
          digitalWrite(MPos_Righ, HIGH);
          digitalWrite(MNeg_Righ, LOW);
          analogWrite(PWM_RIGH, 230);
        } else {
          // pivote hacia la derecha
          digitalWrite(MPos_Left, HIGH);
          digitalWrite(MNeg_Left, LOW);
          analogWrite(PWM_LEFT, 230);
          digitalWrite(MPos_Righ, LOW);
          digitalWrite(MNeg_Righ, HIGH);
          analogWrite(PWM_RIGH, 210);
        }

        delay(260);

        // reentrada agresiva hacia adelante
        digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);
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