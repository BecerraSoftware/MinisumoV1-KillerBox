  /*
  Robot Mini Sumo: KILLERBOX (Versión Los legendarios Ultrasonidos💘)
  Autor: Victor Becerra (github: BecerraSoftware)
*/

#define DEBUG 1

// Sensores ultrasónicos (HC-SR04)
#define US_FRONT_TRIG 12
#define US_FRONT_ECHO 13

#define US_LEFT_TRIG 4
#define US_LEFT_ECHO 3

#define US_RIGHT_TRIG 2
#define US_RIGHT_ECHO 11

#define US_THRESHOLD_CM 30
#define US_TIMEOUT_US 30000
#define US_SAMPLES 3

// Sensores de piso
#define floorLeft A7
#define floorRight A0
#define BLANCO 400

// Motores
#define MPos_Left 9
#define MNeg_Left 8
#define PWM_LEFT 10

#define MPos_Right 6
#define MNeg_Right 7
#define PWM_RIGHT 5

// Velocidades
#define VEL_MAX 255
#define VEL_GIRO 150
#define VEL_RECUA 180

// Dipswitches
#define DipSwitch1 A2
#define DipSwitch2 A3

// ====================== SETUP ======================
void setup() {
  Serial.begin(9600);

  // Pines ultrasónicos
  pinMode(US_FRONT_TRIG, OUTPUT);
  pinMode(US_FRONT_ECHO, INPUT);

  pinMode(US_LEFT_TRIG, OUTPUT);
  pinMode(US_LEFT_ECHO, INPUT);

  pinMode(US_RIGHT_TRIG, OUTPUT);
  pinMode(US_RIGHT_ECHO, INPUT);

  // Sensores de piso
  pinMode(floorLeft, INPUT);
  pinMode(floorRight, INPUT);

  // Motores
  pinMode(MPos_Left, OUTPUT);
  pinMode(MNeg_Left, OUTPUT);
  pinMode(PWM_LEFT, OUTPUT);

  pinMode(MPos_Right, OUTPUT);
  pinMode(MNeg_Right, OUTPUT);
  pinMode(PWM_RIGHT, OUTPUT);

  pinMode(DipSwitch1, INPUT);
  pinMode(DipSwitch2, INPUT);
  stop();
  Serial.println("yuhaaaaniiiii");
  delay(500); //Aqui cambiar para lo del SECIHTI 
  
  
}

// ====================== FUNCIONES DE SENSORES ======================

// Lectura ultrasónica individual
long readUltrasonicCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  unsigned long duration = pulseIn(echoPin, HIGH, US_TIMEOUT_US);
  if (duration == 0) return -1;
  return duration / 58;
}

// Promedio de lecturas
long readUltrasonicAvg(int trigPin, int echoPin, int samples = US_SAMPLES) {
  long sum = 0; int valid = 0;
  for (int i = 0; i < samples; i++) {
    long d = readUltrasonicCM(trigPin, echoPin);
    if (d > 0) { sum += d; valid++; }
    delay(5);
  }
  return (valid == 0) ? -1 : sum / valid;
}

bool DetectaBlanco(int pin) { return analogRead(pin) < BLANCO; }
bool ButonPush(int pin) { return digitalRead(pin) == HIGH; }

// ====================== FUNCIONES DE MOVIMIENTO ======================

void stop() {
  digitalWrite(MPos_Left, LOW);
  digitalWrite(MNeg_Left, LOW);
  analogWrite(PWM_LEFT, 0);
  digitalWrite(MPos_Right, LOW);
  digitalWrite(MNeg_Right, LOW);
  analogWrite(PWM_RIGHT, 0);
}

void Avanzar(int vel) {
  digitalWrite(MPos_Left, LOW);
  digitalWrite(MNeg_Left, HIGH);
  analogWrite(PWM_LEFT, vel);
  digitalWrite(MPos_Right, LOW);
  digitalWrite(MNeg_Right, HIGH);
  analogWrite(PWM_RIGHT, vel);
}

void Atras(int vel) {
  digitalWrite(MPos_Left, HIGH);
  digitalWrite(MNeg_Left, LOW);
  analogWrite(PWM_LEFT, vel);
  digitalWrite(MPos_Right, HIGH);
  digitalWrite(MNeg_Right, LOW);
  analogWrite(PWM_RIGHT, vel);
}

void GirarDerecha(int vel) {
  digitalWrite(MPos_Left, LOW);
  digitalWrite(MNeg_Left, HIGH);
  analogWrite(PWM_LEFT, vel);
  digitalWrite(MPos_Right, HIGH);
  digitalWrite(MNeg_Right, LOW);
  analogWrite(PWM_RIGHT, vel);
}

void GirarIzquierda(int vel) {
  digitalWrite(MPos_Left, HIGH);
  digitalWrite(MNeg_Left, LOW);
  analogWrite(PWM_LEFT, vel);
  digitalWrite(MPos_Right, LOW);
  digitalWrite(MNeg_Right, HIGH);
  analogWrite(PWM_RIGHT, vel);
}

// ====================== LÓGICA PRINCIPAL ======================


void loop() {
  long front = readUltrasonicAvg(US_FRONT_TRIG, US_FRONT_ECHO);
  long left = readUltrasonicAvg(US_LEFT_TRIG, US_LEFT_ECHO);
  long right = readUltrasonicAvg(US_RIGHT_TRIG, US_RIGHT_ECHO);

  if (DEBUG) {
    Serial.print("F:"); Serial.print(front);
    Serial.print(" L:"); Serial.print(left);
    Serial.print(" R:"); Serial.println(right);
  }

  // Seguridad de línea blanca (bordes)
if (DetectaBlanco(floorLeft) || DetectaBlanco(floorRight)) {
  unsigned long startTime = millis();
  unsigned long phase = 0; // 0 = retroceso, 1 = giro

  while (true) {
    // Leer sensores ultrasónicos durante la maniobra
    long front = readUltrasonicAvg(US_FRONT_TRIG, US_FRONT_ECHO);
    long left  = readUltrasonicAvg(US_LEFT_TRIG, US_LEFT_ECHO);
    long right = readUltrasonicAvg(US_RIGHT_TRIG, US_RIGHT_ECHO);

    // --- Fase 0: retroceso durante 500 ms ---
    if (phase == 0) {
      Atras(VEL_RECUA);
      if (millis() - startTime >= 500) {
        startTime = millis(); // reinicia temporizador
        phase = 1;
      }
    }
    // --- Fase 1: giro durante 400 ms ---
    else if (phase == 1) {
      GirarDerecha(VEL_GIRO);
      if (millis() - startTime >= 400) {
        stop();
        break;
      }
    }

    // Si detecta enemigo mientras retrocede o gira, cancelar evasión y atacar
    if ((front > 0 && front < US_THRESHOLD_CM) ||
        (left > 0 && left < US_THRESHOLD_CM) ||
        (right > 0 && right < US_THRESHOLD_CM)) {
      Serial.println("⚔️ Enemigo detectado durante evasión!");
      Avanzar(VEL_MAX);
      break;
    }

    // Evitar bloqueo
    delay(10);
  }
  return;
}


  // Estrategia de detección
  if (front > 0 && front < US_THRESHOLD_CM) {
    // Enemigo al frente → atacar
    Avanzar(VEL_MAX);
  } 
  else if (left > 0 && left < US_THRESHOLD_CM) {
    // Enemigo al lado izquierdo → girar hacia él
    GirarIzquierda(VEL_GIRO);
  } 
  else if (right > 0 && right < US_THRESHOLD_CM) {
    // Enemigo al lado derecho → girar hacia él
    GirarDerecha(VEL_GIRO);
  } 
  else {
    if(digitalRead(DipSwitch1) == HIGH){
      // No hay enemigo → buscar
      GirarIzquierda(VEL_GIRO / 2);
    }
    else
    // No hay enemigo → buscar
    GirarDerecha(VEL_GIRO / 2);
  }

  delay(20);
}
