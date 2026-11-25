// ============================================================================
// Trabajo Práctico N.º 8 - Programación Orientada a Objetos (POO)
// Código Completo Integrado: Sensores (Ej. 1) + LEDs (Ej. 2) + Cronómetro (Ej. 3)
// ============================================================================

// ----------------------------------------------------------------------------
// EJERCICIO 1: Clase Base Sensor y Subclases
// ----------------------------------------------------------------------------
class Sensor {
  protected:
    String nombre;
    float valor;
    String magnitud;

  public:
    Sensor(String _nombre, String _magnitud) {
      nombre = _nombre;
      magnitud = _magnitud;
      valor = 0.0;
    }

    void generarMedicion() {
      valor = random(0, 100);
    }

    float leerMedicion() {
      return valor;
    }

    void mostrar() {
      Serial.print(nombre);
      Serial.print(": ");
      Serial.print(valor, 1);
      Serial.println(magnitud);
    }
};

class SensorTemperatura : public Sensor {
  private:
    float tempMin;
    float tempMax;

  public:
    SensorTemperatura(String _nombre, float _min, float _max)
      : Sensor(_nombre, " C") {
      tempMin = _min;
      tempMax = _max;
    }

    void generarMedicion() {
      valor = random((long)(tempMin * 10), (long)(tempMax * 10)) / 10.0;
    }

    bool esTemperaturaAlta(float umbral) {
      return valor > umbral;
    }
};

class SensorLuz : public Sensor {
  private:
    int maxLux;

  public:
    SensorLuz(String _nombre, int _maxLux)
      : Sensor(_nombre, " lux") {
      maxLux = _maxLux;
    }

    void generarMedicion() {
      valor = random(0, maxLux + 1);
    }

    int obtenerPorcentajeLuz() {
      return map((long)valor, 0, maxLux, 0, 100);
    }
};

class SensorUltrasonido : public Sensor {
  private:
    float rangoMinCm;
    float rangoMaxCm;

  public:
    SensorUltrasonido(String _nombre, float _minCm, float _maxCm)
      : Sensor(_nombre, " cm") {
      rangoMinCm = _minCm;
      rangoMaxCm = _maxCm;
    }

    void generarMedicion() {
      valor = random((long)(rangoMinCm * 10), (long)(rangoMaxCm * 10)) / 10.0;
    }

    bool hayObstaculo(float distanciaLimite) {
      return valor <= distanciaLimite;
    }
};

class SensorTemperaturaReal : public Sensor {
  private:
    int pinAnalogico;
    float voltaje;

  public:
    SensorTemperaturaReal(String _nombre, int _pin)
      : Sensor(_nombre, " C") {
      pinAnalogico = _pin;
      voltaje = 0.0;
      pinMode(pinAnalogico, INPUT);
    }

    void generarMedicion() {
      int lecturaCruda = analogRead(pinAnalogico);
      voltaje = lecturaCruda * (5.0 / 1023.0);
      valor = (voltaje - 0.5) * 100.0;
    }

    float leerVoltaje() {
      return voltaje;
    }
};

// ----------------------------------------------------------------------------
// EJERCICIO 2: Clase Led (Control no bloqueante)
// ----------------------------------------------------------------------------
class Led {
  private:
    int pin;
    bool estadoEncendido;
    bool modoParpadeo;
    unsigned long intervaloParpadeo;
    unsigned long ultimoCambio;

  public:
    Led(int _pin) {
      pin = _pin;
      estadoEncendido = false;
      modoParpadeo = false;
      intervaloParpadeo = 500;
      ultimoCambio = 0;
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }

    void encender() {
      modoParpadeo = false;
      estadoEncendido = true;
      digitalWrite(pin, HIGH);
    }

    void apagar() {
      modoParpadeo = false;
      estadoEncendido = false;
      digitalWrite(pin, LOW);
    }

    void parpadear(unsigned long intervaloMs) {
      modoParpadeo = true;
      intervaloParpadeo = intervaloMs;
    }

    void actualizar() {
      if (modoParpadeo) {
        unsigned long ahora = millis();
        if (ahora - ultimoCambio >= intervaloParpadeo) {
          ultimoCambio = ahora;
          estadoEncendido = !estadoEncendido;
          digitalWrite(pin, estadoEncendido ? HIGH : LOW);
        }
      }
    }

    String obtenerEstadoTexto() {
      if (modoParpadeo) {
        return "PARPADEANDO (" + String(intervaloParpadeo) + " ms)";
      }
      return estadoEncendido ? "ENCENDIDO" : "APAGADO";
    }
};

// ----------------------------------------------------------------------------
// 3.1 EJERCICIO 3: Clase Cronometro (usando únicamente millis())
// ----------------------------------------------------------------------------
class Cronometro {
  private:
    bool enMarcha;
    unsigned long tiempoInicio;
    unsigned long tiempoAcumulado;

  public:
    // Constructor: inicia detenido en 0 ms
    Cronometro() {
      enMarcha = false;
      tiempoInicio = 0;
      tiempoAcumulado = 0;
    }

    // Activa o reanuda el conteo ('A')
    void activar() {
      if (!enMarcha) {
        tiempoInicio = millis();
        enMarcha = true;
        Serial.println(">> [CRONOMETRO] ACTIVADO");
      } else {
        Serial.println(">> [CRONOMETRO] Ya estaba en marcha");
      }
    }

    // Detiene/pausa el conteo conservando el tiempo acumulado ('D')
    void detener() {
      if (enMarcha) {
        tiempoAcumulado += (millis() - tiempoInicio);
        enMarcha = false;
        Serial.println(">> [CRONOMETRO] DETENIDO");
      } else {
        Serial.println(">> [CRONOMETRO] Ya estaba detenido");
      }
    }

    // Reinicia el contador a 0 ('R')
    void resetear() {
      tiempoAcumulado = 0;
      if (enMarcha) {
        tiempoInicio = millis();
      }
      Serial.println(">> [CRONOMETRO] RESETEADO A 0.0 s");
    }

    // Devuelve el tiempo total transcurrido en milisegundos
    unsigned long obtenerMilisegundos() {
      if (enMarcha) {
        return tiempoAcumulado + (millis() - tiempoInicio);
      }
      return tiempoAcumulado;
    }

    // Imprime el estado y el tiempo transcurrido en segundos
    void mostrar() {
      float segundos = obtenerMilisegundos() / 1000.0;
      Serial.print("Cronometro [");
      Serial.print(enMarcha ? "CORRIENDO" : "DETENIDO");
      Serial.print("]: ");
      Serial.print(segundos, 2);
      Serial.println(" s");
    }
};

// ============================================================================
// Instanciación de Objetos
// ============================================================================
SensorTemperatura     tempSim("Temp_Ambiente (Sim)", 15.0, 38.0);
SensorLuz             luzSim("Luz_Exterior (Sim)", 1000);
SensorUltrasonido     distSim("Distancia_Frente (Sim)", 2.0, 200.0);
SensorTemperaturaReal tempReal("Temp_TMP36 (Real)", A0);

Led ledTemp(13);
Led ledLuz(12);
Led ledDist(11);

Cronometro crono;  // Objeto del Ejercicio 3

unsigned long ultimaLectura = 0;
const unsigned long INTERVALO_MUESTREO = 3000;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A5));

  Serial.println("=== TP8 COMPLETO: SENSORES + LEDS + CRONOMETRO ===");
  Serial.println("Comandos Serie -> [A]: Activar | [D]: Detener | [R]: Resetear");
  Serial.println("------------------------------------------------------------");
}

void loop() {
  // 1. Lectura de comandos por puerto serie para controlar el Cronómetro (Ej. 3)
  if (Serial.available() > 0) {
    char comando = Serial.read();

    // Ignoramos saltos de línea o retornos de carro al presionar Enter
    if (comando != '\n' && comando != '\r') {
      comando = toupper(comando);  // Acepta tanto mayúsculas como minúsculas

      if (comando == 'A') {
        crono.activar();
      } else if (comando == 'D') {
        crono.detener();
      } else if (comando == 'R') {
        crono.resetear();
      } else {
        Serial.print(">> Comando no valido: '");
        Serial.print(comando);
        Serial.println("'. Use A, D o R.");
      }
      crono.mostrar();
    }
  }

  // 2. Muestreo periódico de sensores y actualización de LEDs (Ej. 1 y 2)
  unsigned long ahora = millis();
  if (ahora - ultimaLectura >= INTERVALO_MUESTREO) {
    ultimaLectura = ahora;

    tempSim.generarMedicion();
    luzSim.generarMedicion();
    distSim.generarMedicion();
    tempReal.generarMedicion();

    // Reacción de LEDs
    if (tempSim.esTemperaturaAlta(30.0)) {
      ledTemp.encender();
    } else if (tempSim.leerMedicion() >= 22.0) {
      ledTemp.parpadear(600);
    } else {
      ledTemp.apagar();
    }

    if (luzSim.obtenerPorcentajeLuz() < 30) {
      ledLuz.encender();
    } else {
      ledLuz.apagar();
    }

    if (distSim.hayObstaculo(40.0)) {
      ledDist.parpadear(150);
    } else {
      ledDist.apagar();
    }

    // Reporte general en Monitor Serie
    tempSim.mostrar();
    luzSim.mostrar();
    distSim.mostrar();
    tempReal.mostrar();
    crono.mostrar();
    Serial.println("------------------------------------------------------------");
  }

  // 3. Refresco continuo del parpadeo de los LEDs
  ledTemp.actualizar();
  ledLuz.actualizar();
  ledDist.actualizar();
}