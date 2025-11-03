#ifndef BM2168_LED_H
#define BM2168_LED_H

#include <SoftwareSerial.h>

// Colores predefinidos
#define COLOR_OFF     0x00
#define COLOR_BLUE    0x01
#define COLOR_GREEN   0x02  
#define COLOR_RED     0x04
#define COLOR_YELLOW  0x06  // RED + GREEN
#define COLOR_CYAN    0x03  // GREEN + BLUE
#define COLOR_MAGENTA 0x05  // RED + BLUE
#define COLOR_WHITE   0x07  // RED + GREEN + BLUE

// Modos de operación
#define MODE_BREATHING 0x01
#define MODE_FLASH     0x02
#define MODE_ON        0x03
#define MODE_OFF       0x04
#define MODE_FADE_IN   0x05
#define MODE_FADE_OUT  0x06

class BM2168_LED {
  public:
    // Constructor
    BM2168_LED(uint8_t rxPin, uint8_t txPin);
    
    // Inicialización
    void begin(unsigned long baudRate = 57600);
    
    // Control básico
    void setLED(uint8_t mode, uint8_t startColor, uint8_t endColor, uint8_t cycles);
    void setColor(uint8_t color);
    void off();
    
    // Efectos predefinidos
    void breathing(uint8_t color, uint8_t cycles = 0);
    void flash(uint8_t color, uint8_t cycles = 5);
    void fadeIn(uint8_t color);
    void fadeOut(uint8_t color);
    
    // Efectos avanzados
    void rainbow(uint16_t duration = 1000);
    void cycleColors(uint8_t colors[], uint8_t count, uint16_t duration);
    
    // NUEVO: Parpadeo rápido controlado
    void fastFlash(uint8_t color, uint16_t onTime, uint16_t offTime, uint8_t flashes);
    void startFastFlash(uint8_t color, uint16_t onTime, uint16_t offTime);
    void stopFastFlash();
    void update(); // Necesaria para efectos no bloqueantes
    
    // NUEVO: Sintaxis Adafruit-compatible
    void ledControl(uint8_t mode, uint16_t speed, uint8_t color, uint8_t cycles);
    
    // Constantes para compatibilidad Adafruit
    static const uint8_t LED_FLASHING = 0x02;
    static const uint8_t LED_BREATHING = 0x01;
    static const uint8_t LED_ON = 0x03;
    static const uint8_t LED_OFF = 0x04;
    static const uint8_t LED_RED = 0x04;
    static const uint8_t LED_BLUE = 0x01;
    static const uint8_t LED_GREEN = 0x02;
    static const uint8_t LED_WHITE = 0x07;
    
    // Utilidad
    bool isConnected();
    
  private:
    SoftwareSerial* _fingerPrint;
    uint8_t _rxPin;
    uint8_t _txPin;
    unsigned long _baudRate;
    
    // Variables para control de parpadeo rápido
    bool _fastFlashActive = false;
    uint8_t _flashColor;
    uint16_t _onTime, _offTime;
    unsigned long _lastFlashTime;
    bool _flashState = false;
    
    void sendCommand(uint8_t mode, uint8_t startColor, uint8_t endColor, uint8_t cycles);
    uint16_t calculateChecksum(uint8_t packet[], uint8_t start, uint8_t end);
    void handleSpeedControl(uint8_t mode, uint16_t speed, uint8_t color, uint8_t cycles);
};

// Alias para compatibilidad con estilo Adafruit
#define FINGERPRINT_LED_FLASHING BM2168_LED::LED_FLASHING
#define FINGERPRINT_LED_BREATHING BM2168_LED::LED_BREATHING
#define FINGERPRINT_LED_ON BM2168_LED::LED_ON
#define FINGERPRINT_LED_OFF BM2168_LED::LED_OFF
#define FINGERPRINT_LED_RED BM2168_LED::LED_RED
#define FINGERPRINT_LED_BLUE BM2168_LED::LED_BLUE
#define FINGERPRINT_LED_GREEN BM2168_LED::LED_GREEN
#define FINGERPRINT_LED_WHITE BM2168_LED::LED_WHITE

#endif