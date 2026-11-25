// ============================================================================
// Trabajo Práctico N.º 8 - Programación Orientada a Objetos (POO)
// Ejercicio 1 + Ejercicio 2: Sensores y Control Independiente de LEDs
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
// 2.1 EJERCICIO 2: Clase Led (encender, apagar, parpadear independiente)
// ----------------------------------------------------------------------------
class Led {
  private:
    int pin;
    bool estadoEncendido;
    bool modoParpadeo;
    unsigned long intervaloParpadeo;
    unsigned long ultimoCambio;

  public:
    // Constructor: configura el pin como salida y arranca apagado
    Led(int _pin) {
      pin = _pin;
      estadoEncendido = false;
      modoParpadeo = false;
      intervaloParpadeo = 500;
      ultimoCambio = 0;
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
    }

    // Enciende el LED de forma continua (cancela parpadeo)
    void encender() {
      modoParpadeo = false;
      estadoEncendido = true;
      digitalWrite(pin, HIGH);
    }

    // Apaga el LED de forma continua (cancela parpadeo)
    void apagar() {
      modoParpadeo = false;
      estadoEncendido = false;
      digitalWrite(pin, LOW);
    }

    // Activa el parpadeo periódico a la velocidad indicada (en milisegundos)
    void parpadear(unsigned long intervaloMs) {
      modoParpadeo = true;
      intervaloParpadeo = intervaloMs;
    }

    // Debe llamarse en cada vuelta del loop() para gestionar el parpadeo sin bloquear
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

    // Devuelve una descripción legible del estado actual para mostrar en consola
    String obtenerEstadoTexto() {
      if (modoParpadeo) {
        return "PARPADEANDO (" + String(intervaloParpadeo) + " ms)";
      }
      return estadoEncendido ? "ENCENDIDO" : "APAGADO";
    }
};

// ============================================================================
// Instanciación de Objetos (Sensores del Ej. 1 + 2.2 Múltiples LEDs)
// ============================================================================
SensorTemperatura     tempSim("Temp_Ambiente (Sim)", 15.0, 38.0);
SensorLuz             luzSim("Luz_Exterior (Sim)", 1000);
SensorUltrasonido     distSim("Distancia_Frente (Sim)", 2.0, 200.0);
SensorTemperaturaReal tempReal("Temp_TMP36 (Real)", A0);

// 2.2 Tres objetos Led independientes en distintos pines
Led ledTemp(13);  // LED Rojo: indicador térmico
Led ledLuz(12);   // LED Amarillo: indicador de baja iluminación
Led ledDist(11);  // LED Verde: indicador de proximidad

// Temporizador no bloqueante para muestrear sensores cada 2.5 segundos
unsigned long ultimaLectura = 0;
const unsigned long INTERVALO_MUESTREO = 2500;

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A5));

  Serial.println("=== INICIO DE SISTEMA: SENSORES + CONTROL DE LEDS (POO) ===");
  Serial.println();
}

void loop() {
  unsigned long ahora = millis();

  // Muestreo periódico de sensores (sin usar delay() para no frenar los LEDs)
  if (ahora - ultimaLectura >= INTERVALO_MUESTREO) {
    ultimaLectura = ahora;

    // 1. Actualizar lecturas de sensores
    tempSim.generarMedicion();
    luzSim.generarMedicion();
    distSim.generarMedicion();
    tempReal.generarMedicion();

    // 2.3 Hacer reaccionar cada LED según la lectura de su sensor simulado
    // --- Reacción a Temperatura ---
    if (tempSim.esTemperaturaAlta(30.0)) {
      ledTemp.encender();            // Alerta alta: fijo encendido
    } else if (tempSim.leerMedicion() >= 22.0) {
      ledTemp.parpadear(600);        // Temperatura media: parpadeo lento
    } else {
      ledTemp.apagar();              // Temperatura baja: apagado
    }

    // --- Reacción a Luz ---
    if (luzSim.obtenerPorcentajeLuz() < 30) {
      ledLuz.encender();             // Poca luz (<30%): enciende luz auxiliar
    } else {
      ledLuz.apagar();               // Buena iluminación: apagado
    }

    // --- Reacción a Distancia ---
    if (distSim.hayObstaculo(40.0)) {
      ledDist.parpadear(150);        // Obstáculo cerca (<=40 cm): parpadeo rápido
    } else {
      ledDist.apagar();              // Despejado: apagado
    }

    // 3. Imprimir reporte en Monitor Serie
    tempSim.mostrar();
    luzSim.mostrar();
    distSim.mostrar();
    tempReal.mostrar();

    Serial.println("  [Estado de Actuadores]");
    Serial.println("  -> LED Temp (Pin 13): " + ledTemp.obtenerEstadoTexto());
    Serial.println("  -> LED Luz  (Pin 12): " + ledLuz.obtenerEstadoTexto());
    Serial.println("  -> LED Dist (Pin 11): " + ledDist.obtenerEstadoTexto());
    Serial.println("-----------------------------------------------------------");
  }

  // 4. Actualizar continuamente el estado de parpadeo de cada LED independiente
  ledTemp.actualizar();
  ledLuz.actualizar();
  ledDist.actualizar();
}