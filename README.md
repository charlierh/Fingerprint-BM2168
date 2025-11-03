# Control-LED-Fingerprint-BM2168

## Descripción
Librería Arduino para controlar los LEDs RGB del sensor de huella BM2168 mediante comandos UART.

**Descubrimiento:** Esta documentación es el resultado de reverse engineering del protocolo del sensor BM2168, ya que la documentación oficial no está disponible públicamente. Pra ello se usó un ESP8266 D1 Mini.

## 🚀 Características

- ✅ Control completo de LEDs RGB
- ✅ Múltiples efectos: respiración, parpadeo, fijo, gradual
- ✅ Parpadeo rápido controlado por software
- ✅ Sintaxis compatible con Adafruit Fingerprint
- ✅ Soporte para ESP8266, ESP32, Arduino
- ✅ Protocolo UART a 57600 bps
- ✅ Fácil de usar con ejemplos incluidos

## Esquema de Conexiones con ESP8266
- BM2168 Pin Out

<img width="618" height="494" alt="Anotación 2025-11-03 081255" src="https://github.com/user-attachments/assets/13db6570-a484-440e-a89d-7fe1793533a8" />

- Wiring Diagram
<img width="1083" height="476" alt="BM2168 wiring to ESP" src="https://github.com/user-attachments/assets/a5554946-2233-49c5-b333-9403df23e620" />


## Instalación
### Método 1: Gestor de Librerías Arduino (Recomendado)
1. Abre Arduino IDE
2. Ve a `Sketch > Include Library > Manage Libraries`
3. Busca "BM2168 LED Control"
4. Haz clic en "Install"

### Método 2: Instalación Manual
1. Descarga [la última release](https://github.com/charlierh/BM2168-LED-Control/releases)
2. En Arduino IDE: `Sketch > Include Library > Add .ZIP Library`
3. Selecciona el archivo descargado
## Función Principal de Control LED
```cpp
void setLED(uint8_t mode, uint8_t startColor, uint8_t endColor, uint8_t cycles) {
  uint8_t ledPacket[] = {
    0xEF, 0x01,                         // Header
    0xFF, 0xFF, 0xFF, 0xFF,             // Address por defecto
    0x01,                               // Packet ID
    0x00, 0x07,                         // Length
    0x3C,                               // Command PS_ControlBLN
    mode, startColor, endColor, cycles, // Parameters
    0x00, 0x00                          // Checksum
  };
  
  // Calcular checksum
  uint16_t checksum = 0;
  for(int i = 6; i <= 13; i++) {
    checksum += ledPacket[i];
  }
  ledPacket[14] = (checksum >> 8) & 0xFF;
  ledPacket[15] = checksum & 0xFF;
  
  fingerPrint.write(ledPacket, 16);
}
```
## Especificación del Protocolo
 Comando LED: PS_ControlBLN (0x3C)
###
Estructura del Paquete:
###
| Header | Address | PID | Length | Cmd | Params | Checksum |
|:-------|:---------|:----|:--------|:----|:---------|:-----------|
| 0xEF 0x01 | 0xFF 0xFF | 0x01 | 0x00 0x07 | 0x3C | [4 bytes] | 0x00 0xXX |
|           | 0xFF 0xFF |      |           |      |           |           |

### Parámetros
- mode: Modo de operación (1 byte)
- startColor: Color inicial (1 byte)
- endColor: Color final (1 byte)
- cycles: Número de ciclos (1 byte)

### Modos de Operación

| Modo      | Valor | Descripción                         |
|:-----------|:------|:------------------------------------|
| Breathing  | 0x01  | Efecto de respiración entre colores |
| Flash      | 0x02  | Parpadeo                            |
| On         | 0x03  | Encendido fijo                      |
| Off        | 0x04  | Apagado                             |
| Fade In    | 0x05  | Encendido gradual                   |
| Fade Out   | 0x06  | Apagado gradual                     |

### Colores RGB

| Color   | Valor | Combinación              |
|:---------|:------|:-------------------------|
| Off      | 0x00  | Apagado                 |
| Blue     | 0x01  | LED Azul                |
| Green    | 0x02  | LED Verde               |
| Red      | 0x04  | LED Rojo                |
| Yellow   | 0x06  | Rojo + Verde            |
| Cyan     | 0x03  | Verde + Azul            |
| Magenta  | 0x05  | Rojo + Azul             |
| White    | 0x07  | Rojo + Verde + Azul     |

## Ejemplos de Uso
Ejemplo 1: LED Rojo Fijo
```
setLED(0x03, 0x04, 0x04, 0x00);
```
Ejemplo 2: Respiración Azul (3 ciclos)
```
setLED(0x01, 0x00, 0x01, 0x03);
```
Ejemplo 3: Parpadeo Verde (5 ciclos)
```
setLED(0x02, 0x02, 0x02, 0x05);
```
Ejemplo 4: Secuencia de Colores
```
void rainbowSequence() {
  uint8_t colors[] = {0x04, 0x02, 0x01, 0x06, 0x03, 0x05, 0x07};
  
  for(int i = 0; i < 7; i++) {
    setLED(0x03, colors[i], colors[i], 0);
    delay(1000);
  }
}
```
Código Completo de Ejemplo
```
#include <SoftwareSerial.h>

#define FINGERPRINT_RX D6 // TX del sensor
#define FINGERPRINT_TX D5 // RX del sensor

SoftwareSerial fingerPrint(FINGERPRINT_RX, FINGERPRINT_TX);

void setup() {
  Serial.begin(115200);
  fingerPrint.begin(57600);
  
  Serial.println("BM2168 LED Control Ready");
  
  // Ejemplo: secuencia de demostración
  runDemoSequence();
}

void loop() {
  // Tu código principal aquí
}

void runDemoSequence() {
  // Rojo fijo
  setLED(0x03, 0x04, 0x04, 0x00);
  delay(2000);
  
  // Respiración azul
  setLED(0x01, 0x00, 0x01, 3);
  delay(5000);
  
  // Parpadeo verde
  setLED(0x02, 0x02, 0x02, 5);
  delay(4000);
  
  // Apagar
  setLED(0x04, 0x00, 0x00, 0x00);
}

void setLED(uint8_t mode, uint8_t startColor, uint8_t endColor, uint8_t cycles) {
  uint8_t ledPacket[16] = {
    0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00,
    0x07, 0x3C, mode, startColor, endColor, cycles, 0x00, 0x00
  };
  
  uint16_t checksum = 0;
  for(int i = 6; i <= 13; i++) checksum += ledPacket[i];
  ledPacket[14] = (checksum >> 8) & 0xFF;
  ledPacket[15] = checksum & 0xFF;
  
  fingerPrint.write(ledPacket, 16);
}
```
### Sintaxis Nativa

```
#include <BM2168_LED.h>

BM2168_LED fingerLED(D6, D5);  // RX, TX

void setup() {
  fingerLED.begin();
  fingerLED.setColor(COLOR_RED);     // Rojo fijo
  fingerLED.fastFlash(COLOR_BLUE, 50, 50, 10); // Parpadeo rápido
}

void loop() {
  fingerLED.update(); // Para efectos no bloqueantes
}
```
### Sintaxis Adafruit-Compatible
```
#include <BM2168_LED.h>

BM2168_LED finger(D6, D5);

void setup() {
  finger.begin();
  // Sintaxis estilo Adafruit
  finger.ledControl(FINGERPRINT_LED_FLASHING, 50, FINGERPRINT_LED_RED, 5);
}
```
## Examples
La librería incluye ejemplos para empezar:
- BasicDemo: Control básico de colores y efectos
- AdvancedEffects: Efectos avanzados y secuencias
- CommandTester: Utilidad para probar comandos
- ManualControl: Control por monitor serie
- FastFlashDemo: Parpadeos rápidos controlados
- AdafruitCompatible: Sintaxis estilo Adafruit

## 🔄 Compatibilidad con Sintaxis Adafruit
### La librería ahora soporta sintaxis estilo Adafruit Fingerprint:
```
// Sintaxis Adafruit-style
finger.ledControl(FINGERPRINT_LED_FLASHING, 50, FINGERPRINT_LED_RED, 3);

// Constantes disponibles:
// FINGERPRINT_LED_FLASHING, FINGERPRINT_LED_BREATHING
// FINGERPRINT_LED_ON, FINGERPRINT_LED_OFF
// FINGERPRINT_LED_RED, GREEN, BLUE, WHITE
```

## Solución de Problemas
### El senor no responde

1. Verificar conexiones VCC (3.3V), GND, RX, TX, IRQ, VIN (3.3V)
2. Confirmar baud rate (57600)
3. Revisar cálculo de checksum

### Parpadeo demasiado lento
- Usa fastFlash() o ledControl() con parámetro de velocidad
- Velocidad mínima recomendada: 25ms

### Checksum Incorrecto
El checksum es la suma de bytes desde Packet ID (0x01) hasta el último parámetro.

### Colores No Esperados
Verificar valores de color según la tabla de combinaciones RGB.

### Modelos Compatibles
- ✅ BM2168 (verificado)
- ⚠️ BM2166 (probable, no verificado)
- ⚠️ ZW101/ZW111 (probable, no verificado)

## Descubrimientos Importantes
1. Comando: PS_ControlBLN (0x3C) - No documentado oficialmente
2. Protocolo: UART 57600 bps, 8N1
3. Checksum: Suma simple de bytes específicos
4. RGB: Control individual de LEDs mediante bits

## Referencias

**Esta librería está desarrollada específicamente para el sensor BM2168**
y utiliza comandos del **protocolo extendido de Hi-Link**.

### Manual de Referencia
- **`Fingerprint module_V1.1 (communication protocol).pdf`** - Shenzhen Hi-Link Electronic Co., Ltd.
  - Páginas 72-73: Comando `PS_ControlBLN (0x3C)` y parámetros LED RGB
  - Páginas 21-24: Estructura de paquetes de comunicación UART
  - Página 11: Configuración UART (57600 bps, 8N1)

## Contribuciones
¡Las contribuciones son bienvenidas! Si encuentras:
- Nuevos modelos compatibles
- Efectos adicionales
- Mejoras en el protocolo
- Por favor, abre un Issue o Pull Request.

Licencia
MIT License - Ver archivo LICENSE para detalles.
