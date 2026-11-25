// ============================================================================
// Trabajo Práctico N.º 8 - Programación Orientada a Objetos (POO)
// Ejercicio 1: Simulación de Sensores con Clases y Sensor Real (TMP36)
// ============================================================================

// ----------------------------------------------------------------------------
// 1.1 Clase Base: Sensor
// ----------------------------------------------------------------------------
class Sensor {
  protected:
    String nombre;    // Identificador del sensor
    float valor;      // Último valor medido o simulado
    String magnitud;  // Unidad de medida (" °C", " lux", " cm")

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

    // Método propio de la clase para imprimir su estado por Monitor Serie
    void mostrar() {
      Serial.print(nombre);
      Serial.print(": ");
      Serial.print(valor, 1);
      Serial.println(magnitud);
    }
};

// ----------------------------------------------------------------------------
// 1.2 Clases Derivadas (Herencia) - Sensores Simulados
// ----------------------------------------------------------------------------

// Subclase 1: Sensor de Temperatura Simulado
// Agrega atributos de rango operativo (tempMin, tempMax) y método de alarma térmica.
class SensorTemperatura : public Sensor {
  private:
    float tempMin;
    float tempMax;

  public:
    SensorTemperatura(String _nombre, float _min, float _max)
      : Sensor(_nombre, " °C") {
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

// Subclase 2: Sensor de Luz Simulado
// Agrega atributo de nivel máximo de iluminancia y método de porcentaje.
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

// Subclase 3: Sensor de Ultrasonido Simulado
// Agrega límites de alcance y método de detección de obstáculos.
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

// ----------------------------------------------------------------------------
// 1.4 Clase Derivada - Sensor Real (TMP36 conectado a pin analógico)
// Agrega el pin físico de entrada, el voltaje leído y la conversión real.
// ----------------------------------------------------------------------------
class SensorTemperaturaReal : public Sensor {
  private:
    int pinAnalogico;
    float voltaje;

  public:
    SensorTemperaturaReal(String _nombre, int _pin)
      : Sensor(_nombre, " °C") {
      pinAnalogico = _pin;
      voltaje = 0.0;
      pinMode(pinAnalogico, INPUT);
    }

    void generarMedicion() {
      int lecturaCruda = analogRead(pinAnalogico);
      voltaje = lecturaCruda * (5.0 / 1023.0);
      valor = (voltaje - 0.5) * 100.0;  // Conversión TMP36 (500 mV offset, 10 mV/°C)
    }

    float leerVoltaje() {
      return voltaje;
    }
};

// ============================================================================
// 1.3 Programa Principal: Instanciación y monitoreo por puerto serie
// ============================================================================

SensorTemperatura     tempSim("Temp_Ambiente (Sim)", 15.0, 38.0);
SensorLuz             luzSim("Luz_Exterior (Sim)", 1000);
SensorUltrasonido     distSim("Distancia_Frente (Sim)", 2.0, 200.0);
SensorTemperaturaReal tempReal("Temp_TMP36 (Real)", A0);

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(A5));

  Serial.println("=== INICIO DE MONITOREO DE SENSORES (POO) ===");
  Serial.println();
}

void loop() {
  // 1. Generamos las mediciones en cada objeto
  tempSim.generarMedicion();
  luzSim.generarMedicion();
  distSim.generarMedicion();
  tempReal.generarMedicion();

  // 2. Mostramos las lecturas llamando al método heredado mostrar()
  tempSim.mostrar();
  luzSim.mostrar();
  distSim.mostrar();
  tempReal.mostrar();

  // 3. Ejecutamos los métodos específicos de cada subclase
  if (tempSim.esTemperaturaAlta(30.0)) {
    Serial.println("  -> [ALERTA] Temperatura simulada elevada (> 30 C)");
  }

  Serial.print("  -> Nivel de iluminacion: ");
  Serial.print(luzSim.obtenerPorcentajeLuz());
  Serial.println("%");

  if (distSim.hayObstaculo(25.0)) {
    Serial.println("  -> [AVISO] Obstaculo detectado a menos de 25 cm");
  }

  Serial.print("  -> Tension en pin A0 (TMP36): ");
  Serial.print(tempReal.leerVoltaje(), 2);
  Serial.println(" V");

  Serial.println("---------------------------------------------");
  delay(2000);
}