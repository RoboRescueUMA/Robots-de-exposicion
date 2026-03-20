#include <MeMCore.h>

// ---------- MOTORES ----------
MeDCMotor motorIzq(M1);
MeDCMotor motorDer(M2);

int velocidad = 50;

// ---------- MODOS ----------
enum Modo {
  AUTOMATICO,
  MANUAL
};

Modo modoActual = MANUAL; // Arranca en manual

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  parar(); // seguridad: empieza detenido
  Serial.println("Sistema iniciado");
  Serial.println("Modo por defecto: MANUAL");
}

// ---------- LOOP ----------
void loop() {

  // ---- LECTURA SERIE ----
  if (Serial.available()) {
    char c = Serial.read();

    // Debug: mostrar lo recibido
    Serial.print("Recibido: ");
    Serial.println(c);

    // ---- CAMBIO DE MODO ----
    if (c == 'M') {
      modoActual = MANUAL;
      parar();
      Serial.println("Modo MANUAL");
    } 
    else if (c == 'A') {
      modoActual = AUTOMATICO;
      Serial.println("Modo AUTOMATICO");
    } 
    // ---- CONTROL MANUAL ----
    else if (modoActual == MANUAL) {
      controlManual(c);
    }
  }

  // ---- MODO AUTOMÁTICO ----
  if (modoActual == AUTOMATICO) {
    modoAutomatico();
  }
}

// ---------- MODO AUTOMÁTICO ----------
void modoAutomatico() {
  avanzar();
  delay(2000);

  girarDerecha();
  delay(800);
}

// ---------- MODO MANUAL ----------
void controlManual(char c) {
  switch (c) {
    case 'w':
      avanzar();
      Serial.println("Avanzar");
      break;
    case 's':
      retroceder();
      Serial.println("Retroceder");
      break;
    case 'a':
      girarIzquierda();
      Serial.println("Izquierda");
      break;
    case 'd':
      girarDerecha();
      Serial.println("Derecha");
      break;
    case 'x':
      parar();
      Serial.println("Parar");
      break;
    default:
      Serial.println("Comando no reconocido");
      break;
  }
}

// ---------- MOVIMIENTO ----------
void avanzar() {
  motorIzq.run(-velocidad);
  motorDer.run(velocidad);
}

void retroceder() {
  motorIzq.run(velocidad);
  motorDer.run(-velocidad);
}

void girarDerecha() {
  motorIzq.run(-velocidad);
  motorDer.run(-velocidad);
}

void girarIzquierda() {
  motorIzq.run(velocidad);
  motorDer.run(velocidad);
}

void parar() {
  motorIzq.run(0);
  motorDer.run(0);
}