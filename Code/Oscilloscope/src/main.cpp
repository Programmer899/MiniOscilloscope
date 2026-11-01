#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define PROBE_PIN 2

#define WIDTH 128
#define HEIGHT 64

static uint8_t measures [WIDTH];
static uint8_t framebuffer [WIDTH * HEIGHT / 8];

const uint16_t DISPLAY_INTERVAL_MS = 40;
const uint16_t SAMPLE_INTERVALL_US = 500;
uint32_t nextSampleUs = 0;
uint32_t lastDisplayMs = 0;
uint8_t sampleIndex = 0;

static uint8_t adcToY(uint16_t adc) {
  if(adc > 4095) adc = 4095;
  return HEIGHT - 1 - ((uint32_t)adc * (HEIGHT - 1) / 4095);
}

void setup() {
  pinMode(PROBE_PIN, INPUT);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  delay(200);
}

void loop() {
  uint32_t nowUs = micros();

  // Read analog samples from signal wave
  if ((int32_t)(nowUs - nextSampleUs) >= 0) {
    nextSampleUs += SAMPLE_INTERVALL_US;

    measures[sampleIndex] = adcToY(analogRead(PROBE_PIN));
    sampleIndex++;
  }

  if (sampleIndex >= WIDTH && millis() - lastDisplayMs >= DISPLAY_INTERVAL_MS) {
    display.clearDisplay();

    for (uint8_t x = 0; x < WIDTH - 1; x++) display.drawLine(x, measures[x], x + 1, measures[x + 1], SSD1306_WHITE);
    
    display.display();
    lastDisplayMs = millis();
    sampleIndex = 0;
  }
}