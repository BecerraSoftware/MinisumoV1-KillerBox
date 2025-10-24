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

/*
  PINES QUE NO ESTAN FUNCANDO
  A4, A6, A7

  PINES EN USO
  S1=>A0
  S2=>A1
  S3=>A2
  S4=>A3
  S8=>A7
*/

//Cuenta con 3 Sensores al frente para detectar 3 posiciones distintas
#define delLeft A2
#define delRigh A6
#define delForward 12 //

//Cuenta con 2 Sensores a los costados para detectar si esta de lado o una orientacion
#define sensorRigh A1 //NOSE
#define sensorLeft A1

//Cuenta con dos sensores de piso que detectan el color blanco para no salir del ring
#define floorLeft A7
#define floorRigh A0

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

//BANDERA
//#define pinServo A4

//Servo bandera;   // crea el objeto para el servo

/*
Variables del sistema del robot
BLANCO==> Del sensor de piso que tan intennso detecta el blanco para determinar que movimiento
se efectua
*/

//sensores de piso
#define BLANCO 400
const int velocidad=255;


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

  // Configura un tiempo máximo de respuesta (solo para detectar errores)

 
  
  //SENSORES
  pinMode(delForward,INPUT);
  pinMode(delRigh,INPUT);
  pinMode(delLeft,INPUT);
  pinMode(sensorRigh,INPUT);
  pinMode(sensorLeft,INPUT);


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

  //dipswith
  pinMode(DipSwith1,INPUT);
  pinMode(DipSwith2,INPUT);


  //leds de prueba
  pinMode(11,OUTPUT);

  //Avanzar hasta que detecte la linea despues gira y ataca 
  /*if(ButonPush(DipSwith1)&&!ButonPush(DipSwith2)){ 
    while(!DetectaBlanco(floorRigh) || !DetectaBlanco(floorLeft)){
    Avanzar(120,0);
  }
  Avanzar(200,4);
  }
  stop();*/

  Serial.begin(9600); // Monitor serial
  delay(200);//quitar esto si es necesario
}

bool DetectaBlanco(int pin){return analogRead(pin)<BLANCO;}
int DetectarPiso(int pin){return analogRead(pin);}
bool DetectarObstaculo(int pin){return analogRead(pin)>500?1:0;}
bool ButonPush(int buton){return digitalRead(buton)==1;}

void loopSensor(){
   // Lee el valor más reciente sin bloquear el programa
  uint16_t distancia = sensor.readRangeContinuousMillimeters();

  Serial.print("Distancia: ");
  Serial.print(distancia);
  Serial.print(" mm");
  Serial.print("  ");
  Serial.print(DetectarObstaculo(delRigh));
  Serial.print("  ");
  Serial.println(DetectarObstaculo(delLeft));
}
void loopxd(){

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
}
void Evadir(float velocidad,int opcion){
  /*
 Función Avanzar:
   - velocidad: valor PWM (0 a 255)
   - opcion:
       0 -> ambos motores hacia adelante (avance)
       1 -> giro a la derecha: motor derecho en reversa
       2 -> giro a la izquierda: motor izquierdo hacia atras, motor derecho en reversa
       3 -> giro en el sitio: motores en direcciones opuestas a velocidad moderada
  */
    switch(opcion) {
      case 0: // Avanzar recto
         digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);
      //DERECHA ADELANTE
        digitalWrite(MPos_Righ, HIGH);
        digitalWrite(MNeg_Righ, LOW);
        analogWrite(PWM_RIGH, velocidad);
        break;
      
      case 1:
       //motorIzquierdoAdelante
       digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);
      //motor derecho atras
       digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);


        break;
      
      case 2: 
       //motor Izquierdo Atras
        digitalWrite(MPos_Left, LOW);
        digitalWrite(MNeg_Left, HIGH);
        analogWrite(PWM_LEFT,velocidad );
      
        //motor derecha adelante
        digitalWrite(MPos_Righ, HIGH);
        digitalWrite(MNeg_Righ, LOW);
        analogWrite(PWM_RIGH, velocidad);
        
        break;
      
      case 3: // Giro en el sitio (por ejemplo, giro a la derecha)
      digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);

        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);
        break;
      case 4: //rutina cuando vea la linea

      Atras(140,500);
      digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);

        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);

      delay(300);



     
  }
  delay(20);  // Pequeño retardo para estabilidad
}
void loopMotores(){
  //izquierda adelante-
       digitalWrite(MPos_Left, LOW);
        digitalWrite(MNeg_Left, HIGH);
        analogWrite(PWM_LEFT, velocidad);
        delay(2000);

        stop();
      //DERECHA ADELANTE
        digitalWrite(MPos_Righ, HIGH);
        digitalWrite(MNeg_Righ, LOW);
        analogWrite(PWM_RIGH, velocidad);
        delay(2000);

        stop();
  //IZQ ATRAS
        digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);
        delay(2000);

        stop();
      
      //DERCHA ATRAS
        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);
        delay(2000);

        stop();
        delay(5000);

}

//loopPrincipal
void loop(){
  uint16_t distancia = sensor.readRangeContinuousMillimeters();
   if(DetectaBlanco(floorRigh) || DetectaBlanco(floorLeft)) {
     Avanzar(80,4);
  } else {
    
  if (distancia<100) {
    Avanzar(255, 0);   // Opción 0: avanzar recto
    Serial.println("avanzar");
  }
  // Si no, si el sensor izquierdo detecta, gira a la derecha (para buscar que el frontal se alinee)
  else if (DetectarObstaculo(delLeft)&& !DetectarObstaculo(delForward)) {
    Avanzar(80, 2);   // Opción 2: giro a la derecha
    Serial.println("activar izquierda");
  }
  // Si no, si el sensor derecho detecta, gira a la izquierda
  else if (DetectarObstaculo(delRigh)&&!DetectarObstaculo(delForward)) {
    Avanzar(80, 1);   // Opción 1: giro a la izquierda
    Serial.println("giro derecha");
  }
  else if(DetectarObstaculo(delLeft)&&DetectarObstaculo(delForward)||DetectarObstaculo(delRigh)&&DetectarObstaculo(delForward)){
     Avanzar(80, 0);   // Opción 0: avanzar recto
    Serial.println("avanzar");
  }
  // Si ningún sensor detecta, gira en el sitio lentamente (buscando la señal del frontal)
  else {
    if(ButonPush(DipSwith2)){
      Avanzar(80,0);
    }
    else{
    Avanzar(80, 3);   // Opción 3: giro en el sitio
    Serial.println("girar sitio");}
  }
  }
  delay(10);
  
}
void Atras(float velocidad,int tiempo){
              digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);      
      //DERCHA ATRAS
        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);
        delay(tiempo);
}
void stop(){
  digitalWrite(MPos_Left,LOW);
  digitalWrite(MNeg_Left,LOW);
  analogWrite(PWM_LEFT,0);

  digitalWrite(MPos_Righ,LOW);
  digitalWrite(MPos_Righ,LOW);
  analogWrite(PWM_RIGH,0);
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
  */
    switch(opcion) {
      case 0:  //avance
      //Izquierda adelante
        digitalWrite(MPos_Left, LOW);
        digitalWrite(MNeg_Left, HIGH);
        analogWrite(PWM_LEFT, velocidad);
      //DERECHA ADELANTE
        digitalWrite(MPos_Righ, HIGH);
        digitalWrite(MNeg_Righ, LOW);
        analogWrite(PWM_RIGH, velocidad);
        break;
      
      case 1: // Giro a la izquierda
       
        //DERECHA ADELANTE
        digitalWrite(MPos_Righ, HIGH);
        digitalWrite(MNeg_Righ, LOW);
        analogWrite(PWM_RIGH, velocidad);

         //IZQ ATRAS
        digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);

        break;
      
      case 2: // Giro a la derecha
        //izquierda adelante-
   //izquierda adelante-
       digitalWrite(MPos_Left, LOW);
        digitalWrite(MNeg_Left, HIGH);//LOW
        analogWrite(PWM_LEFT, 20);
      //derecha atras
        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);
        
        break;
      
      case 3: // Giro en el sitio (por ejemplo, giro a la derecha)
      digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);

        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);
        break;
      case 4: //rutina cuando vea la linea

      Atras(140,1000);
        digitalWrite(MPos_Left, HIGH);
        digitalWrite(MNeg_Left, LOW);
        analogWrite(PWM_LEFT, velocidad);

        digitalWrite(MPos_Righ, LOW);
        digitalWrite(MNeg_Righ, HIGH);
        analogWrite(PWM_RIGH, velocidad);

      delay(100);



     
  }
  delay(20);  // Pequeño retardo para estabilidad
}