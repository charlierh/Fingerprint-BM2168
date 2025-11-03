#include "BM2168_LED.h"
#include <Arduino.h>

BM2168_LED::BM2168_LED(uint8_t rxPin, uint8_t txPin) {
  _rxPin = rxPin;
  _txPin = txPin;
  _baudRate = 57600;
  _fingerPrint = nullptr;
}

void BM2168_LED::begin(unsigned long baudRate) {
  _baudRate = baudRate;
  if (_fingerPrint != nullptr) {
    delete _fingerPrint;
  }
  _fingerPrint = new SoftwareSerial(_rxPin, _txPin);
  _fingerPrint->begin(_baudRate);
  
  // Pequeño delay para inicialización
  delay(100);
}

void BM2168_LED::setLED(uint8_t mode, uint8_t startColor, uint8_t endColor, uint8_t cycles) {
  sendCommand(mode, startColor, endColor, cycles);
}

void BM2168_LED::setColor(uint8_t color) {
  sendCommand(MODE_ON, color, color, 0);
}

void BM2168_LED::off() {
  sendCommand(MODE_OFF, COLOR_OFF, COLOR_OFF, 0);
}

void BM2168_LED::breathing(uint8_t color, uint8_t cycles) {
  sendCommand(MODE_BREATHING, COLOR_OFF, color, cycles);
}

void BM2168_LED::flash(uint8_t color, uint8_t cycles) {
  sendCommand(MODE_FLASH, color, color, cycles);
}

void BM2168_LED::fadeIn(uint8_t color) {
  sendCommand(MODE_FADE_IN, color, color, 0);
}

void BM2168_LED::fadeOut(uint8_t color) {
  sendCommand(MODE_FADE_OUT, color, color, 0);
}

void BM2168_LED::rainbow(uint16_t duration) {
  uint8_t colors[] = {COLOR_RED, COLOR_GREEN, COLOR_BLUE, COLOR_YELLOW, COLOR_CYAN, COLOR_MAGENTA, COLOR_WHITE};
  cycleColors(colors, 7, duration);
}

void BM2168_LED::cycleColors(uint8_t colors[], uint8_t count, uint16_t duration) {
  for(int i = 0; i < count; i++) {
    setColor(colors[i]);
    delay(duration);
  }
}

// Parpadeo rápido controlado
void BM2168_LED::fastFlash(uint8_t color, uint16_t onTime, uint16_t offTime, uint8_t flashes) {
  for(int i = 0; i < flashes; i++) {
    setColor(color);
    delay(onTime);
    off();
    delay(offTime);
  }
}

void BM2168_LED::startFastFlash(uint8_t color, uint16_t onTime, uint16_t offTime) {
  _fastFlashActive = true;
  _flashColor = color;
  _onTime = onTime;
  _offTime = offTime;
  _lastFlashTime = millis();
  _flashState = true;
  setColor(color);
}

void BM2168_LED::stopFastFlash() {
  _fastFlashActive = false;
  off();
}

void BM2168_LED::update() {
  if (!_fastFlashActive) return;
  
  unsigned long currentTime = millis();
  unsigned long elapsed = currentTime - _lastFlashTime;
  
  if (_flashState && elapsed >= _onTime) {
    // Apagar después del tiempo ON
    off();
    _flashState = false;
    _lastFlashTime = currentTime;
  }
  else if (!_flashState && elapsed >= _offTime) {
    // Encender después del tiempo OFF
    setColor(_flashColor);
    _flashState = true;
    _lastFlashTime = currentTime;
  }
}

// Sintaxis Adafruit-compatible
void BM2168_LED::ledControl(uint8_t mode, uint16_t speed, uint8_t color, uint8_t cycles) {
  // Si speed = 0, usar comando nativo del sensor
  // Si speed > 0, implementar control por software para velocidad personalizada
  
  if (speed == 0) {
    // Comando nativo del sensor (velocidad fija)
    sendCommand(mode, color, color, cycles);
  } else {
    // Control por software con velocidad personalizada
    handleSpeedControl(mode, speed, color, cycles);
  }
}

void BM2168_LED::handleSpeedControl(uint8_t mode, uint16_t speed, uint8_t color, uint8_t cycles) {
  switch(mode) {
    case LED_FLASHING:
      // Parpadeo con velocidad controlada
      if (cycles == 0) {
        // Ciclo infinito - modo no bloqueante
        startFastFlash(color, speed, speed);
      } else {
        // Ciclos específicos - modo bloqueante
        fastFlash(color, speed, speed, cycles);
      }
      break;
      
    case LED_BREATHING:
      {
        // ✅ CORREGIDO: Scope propio para variables locales
        uint16_t breathTime = speed / 2;
        // Respiración con velocidad controlada
        if (cycles == 0) {
          // Usar comando nativo para breathing infinito
          sendCommand(MODE_BREATHING, COLOR_OFF, color, 0);
        } else {
          // Simular breathing con ciclos controlados
          for(int i = 0; i < cycles; i++) {
            sendCommand(MODE_BREATHING, COLOR_OFF, color, 1);
            delay(speed);
          }
        }
      }
      break;
      
    case LED_ON:
      setColor(color);
      break;
      
    case LED_OFF:
      off();
      break;
      
    default:
      // Para otros modos, usar comando nativo
      sendCommand(mode, color, color, cycles);
      break;
  }
}

bool BM2168_LED::isConnected() {
  // Por ahora retorna true asumiendo conexión
  // Se puede mejorar con un comando de handshake real
  return true;
}

void BM2168_LED::sendCommand(uint8_t mode, uint8_t startColor, uint8_t endColor, uint8_t cycles) {
  if (_fingerPrint == nullptr) return;
  
  uint8_t packet[16] = {
    0xEF, 0x01,                         // Header
    0xFF, 0xFF, 0xFF, 0xFF,             // Address
    0x01,                               // Packet ID
    0x00, 0x07,                         // Length
    0x3C,                               // Command
    mode, startColor, endColor, cycles, // Parameters
    0x00, 0x00                          // Checksum
  };
  
  // Calcular checksum
  uint16_t checksum = calculateChecksum(packet, 6, 13);
  packet[14] = (checksum >> 8) & 0xFF;
  packet[15] = checksum & 0xFF;
  
  _fingerPrint->write(packet, 16);
  _fingerPrint->flush();
}

uint16_t BM2168_LED::calculateChecksum(uint8_t packet[], uint8_t start, uint8_t end) {
  uint16_t sum = 0;
  for(uint8_t i = start; i <= end; i++) {
    sum += packet[i];
  }
  return sum;
}