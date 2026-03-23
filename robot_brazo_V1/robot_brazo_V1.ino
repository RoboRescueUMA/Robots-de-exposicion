#include <MeOrion.h>

// ---------- MOTORES ----------
MeDCMotor motorIzq(M1);
MeDCMotor motorDer(M2);
MeUltrasonicSensor ultra(PORT_3);
MeDCMotor motorPinza(PORT_1);
MeDCMotor motorBrazo(PORT_2);

int velocidad = 50;

// ---------- ESTADO AUTOMÁTICO ----------
enum Paso {
  AVANZAR,
  GIRAR_DER,
  PAUSAR,
  GIRAR_IZQ,
  SUBIR_BRAZO,
  BAJAR_BRAZO,
  ABRIR_PINZA,
  RETROCEDER,
  FIN_CICLO
};

Paso pasoActual       = AVANZAR;
unsigned long t_paso  = 0;
bool agarroObjeto     = false;
bool pinzaCerrada     = false;

// Duración de cada paso (ms)
const unsigned long DUR_AVANZAR    = 2000;
const unsigned long DUR_GIRAR_DER  = 800;
const unsigned long DUR_PAUSAR     = 600;
const unsigned long DUR_GIRAR_IZQ  = 800;
const unsigned long DUR_BRAZO      = 1200;
const unsigned long DUR_PINZA      = 1700;
const unsigned long DUR_RETROCEDER = 2000;

// ---------- MODOS ----------
enum Modo { AUTOMATICO, MANUAL };
Modo modoActual = MANUAL;

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  parar();
  Serial.println("Sistema iniciado. Modo: MANUAL");
}

// ---------- LOOP ----------
void loop() {
  double distancia = ultra.distanceCm();

  if (Serial.available()) {
    char c = Serial.read();
    Serial.print("Recibido: "); Serial.println(c);

    if (c == 'M') {
      modoActual = MANUAL;
      parar(); detenerBrazo(); detenerPinza();
      Serial.println("Modo MANUAL");
    }
    else if (c == 'A') {
      modoActual   = AUTOMATICO;
      pasoActual   = AVANZAR;
      t_paso       = millis();
      agarroObjeto = false;
      pinzaCerrada = false;
      Serial.println("Modo AUTOMATICO");
    }
    else if (modoActual == MANUAL) {
      controlManual(c);
    }
  }

  if (modoActual == AUTOMATICO) {
    modoAutomatico(distancia);
  }
}

// ---------- AVANZA AL SIGUIENTE PASO ----------
void siguientePaso() {
  parar(); detenerBrazo(); detenerPinza();
  pasoActual = (Paso)(pasoActual + 1);
  t_paso = millis();
  Serial.print("Paso: "); Serial.println(pasoActual);
}

// ---------- MODO AUTOMÁTICO ----------
void modoAutomatico(double distancia) {
  unsigned long elapsed = millis() - t_paso;

  // Sensor: cerrar pinza si detecta objeto
  if (distancia > 0 && distancia <= 9 && !pinzaCerrada) {
    parar();
    detenerBrazo();
    cerrarPinza();
    delay(1000);
    detenerPinza();
    pinzaCerrada = true;
    agarroObjeto = true;
    t_paso = millis();
    Serial.println("Objeto detectado - pinza cerrada");
    return;
  }

  switch (pasoActual) {

    case AVANZAR:
      avanzar();
      if (elapsed >= DUR_AVANZAR) siguientePaso();
      break;

    case GIRAR_DER:
      girarDerecha();
      if (elapsed >= DUR_GIRAR_DER) siguientePaso();
      break;

    case PAUSAR:
      parar();
      if (elapsed >= DUR_PAUSAR) siguientePaso();
      break;

    case GIRAR_IZQ:
      girarIzquierda();
      if (elapsed >= DUR_GIRAR_IZQ) siguientePaso();
      break;

    case SUBIR_BRAZO:
      subirBrazo();
      if (elapsed >= DUR_BRAZO) siguientePaso();
      break;

    case BAJAR_BRAZO:
      bajarBrazo();
      if (elapsed >= DUR_BRAZO) siguientePaso();
      break;

    case ABRIR_PINZA:
      if (!agarroObjeto) {
        siguientePaso();
        break;
      }
      abrirPinza();
      if (elapsed >= DUR_PINZA) {
        detenerPinza();
        pinzaCerrada = false;
        siguientePaso();
      }
      break;

    case RETROCEDER:
      retroceder();
      if (elapsed >= DUR_RETROCEDER) siguientePaso();
      break;

    case FIN_CICLO:
      pasoActual   = AVANZAR;
      t_paso       = millis();
      agarroObjeto = false;
      Serial.println("-- Ciclo reiniciado --");
      break;
  }
}

// ---------- MODO MANUAL ----------
void controlManual(char c) {
  switch (c) {
    case 'w': avanzar();        Serial.println("Avanzar");      break;
    case 's': retroceder();     Serial.println("Retroceder");   break;
    case 'a': girarIzquierda(); Serial.println("Izquierda");    break;
    case 'd': girarDerecha();   Serial.println("Derecha");      break;
    case 'x': parar();          Serial.println("Parar");        break;
    case 'i': subirBrazo();     Serial.println("Subir brazo");  break;
    case 'k': bajarBrazo();     Serial.println("Bajar brazo");  break;
    case 'j': abrirPinza();     Serial.println("Abrir pinza");  break;
    case 'l': cerrarPinza();    Serial.println("Cerrar pinza"); break;
    case 'b': detenerBrazo();   Serial.println("Stop brazo");   break;
    case 'p': detenerPinza();   Serial.println("Stop pinza");   break;
    default:  Serial.println("Comando no reconocido");          break;
  }
}

// ---------- MOVIMIENTO ----------
void avanzar()        { motorIzq.run(-velocidad);    motorDer.run(velocidad);   }
void retroceder()     { motorIzq.run(velocidad);     motorDer.run(-velocidad);  }
void girarDerecha()   { motorIzq.run(-velocidad*3);  motorDer.run(-velocidad*3);}
void girarIzquierda() { motorIzq.run(velocidad*3);   motorDer.run(velocidad*3); }
void parar()          { motorIzq.run(0);             motorDer.run(0);           }
void subirBrazo()     { motorBrazo.run(velocidad*3); }
void bajarBrazo()     { motorBrazo.run(-velocidad*2);}
void abrirPinza()     { motorPinza.run(velocidad*3); }
void cerrarPinza()    { motorPinza.run(-velocidad*3);}
void detenerBrazo()   { motorBrazo.run(0);           }
void detenerPinza()   { motorPinza.run(0);           }
