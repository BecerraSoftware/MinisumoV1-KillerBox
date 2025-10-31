#include <Wire.h>
#include <VL53L0X.h>

const uint8_t SENSOR_IZQ_PIN = 2;
const uint8_t SENSOR_DER_PIN = 3;
const uint8_t MOTOR_IZQ_A = 5;
const uint8_t MOTOR_IZQ_B = 6;
const uint8_t MOTOR_DER_A = 9;
const uint8_t MOTOR_DER_B = 10;

const uint16_t DISTANCIA_OBJETIVO_MM = 300;
const uint16_t DISTANCIA_MAX_MM = 1200;

VL53L0X tof;

void setup() {
  pinMode(SENSOR_IZQ_PIN, INPUT_PULLUP);
  pinMode(SENSOR_DER_PIN, INPUT_PULLUP);
  pinMode(MOTOR_IZQ_A, OUTPUT);
  pinMode(MOTOR_IZQ_B, OUTPUT);
  pinMode(MOTOR_DER_A, OUTPUT);
  pinMode(MOTOR_DER_B, OUTPUT);

  Wire.begin();
  tof.setTimeout(50);
  if (!tof.init()) {
    while (true) {
      detener();
      delay(100);
    }
  }
  tof.startContinuous();
}

void loop() {
  const bool bordeIzq = digitalRead(SENSOR_IZQ_PIN) == LOW;
  const bool bordeDer = digitalRead(SENSOR_DER_PIN) == LOW;
  const uint16_t distancia = limitarDistancia(tof.readRangeContinuousMillimeters());

  if (tof.timeoutOccurred()) {
    detener();
    return;
  }

  if (bordeIzq && bordeDer) {
    retroceder(200);
  } else if (bordeIzq) {
    girarDerecha(200);
  } else if (bordeDer) {
    girarIzquierda(200);
  } else if (distancia < DISTANCIA_OBJETIVO_MM) {
    empujar();
  } else {
    avanzar();
  }
}

uint16_t limitarDistancia(uint16_t lectura) {
  if (lectura == 0 || lectura > DISTANCIA_MAX_MM) {
    return DISTANCIA_MAX_MM;
  }
  return lectura;
}

void avanzar() {
  motorIzq(255, 0);
  motorDer(255, 0);
}

void empujar() {
  motorIzq(255, 0);
  motorDer(255, 0);
}

void retroceder(uint16_t tiempoMs) {
  motorIzq(0, 255);
  motorDer(0, 255);
  delay(tiempoMs);
}

void girarIzquierda(uint16_t tiempoMs) {
  motorIzq(0, 255);
  motorDer(255, 0);
  delay(tiempoMs);
}

void girarDerecha(uint16_t tiempoMs) {
  motorIzq(255, 0);
  motorDer(0, 255);
  delay(tiempoMs);
}

void detener() {
  motorIzq(0, 0);
  motorDer(0, 0);
}

void motorIzq(uint8_t pwmAdelante, uint8_t pwmAtras) {
  analogWrite(MOTOR_IZQ_A, pwmAdelante);
  analogWrite(MOTOR_IZQ_B, pwmAtras);
}

void motorDer(uint8_t pwmAdelante, uint8_t pwmAtras) {
  analogWrite(MOTOR_DER_A, pwmAdelante);
  analogWrite(MOTOR_DER_B, pwmAtras);
}
