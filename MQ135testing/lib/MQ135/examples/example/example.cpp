#include "Particle.h"
#include "MQ135.h"

int number = 0;
int state = 0;
MQ135 gasSensor = MQ135(11);
int led = D7;
float rzero = gasSensor.getRZero();
int ppm = gasSensor.getPPM();

SYSTEM_MODE(SEMI_AUTOMATIC);


void setup() {
    Serial.begin(9600);
}

// LOOP
void loop() { 

    float rzero = gasSensor.getRZero();
    
    int co2_ppm = gasSensor.getPPM();
    int ppm = co2_ppm / 4;
    Serial.printf("%i %i", co2_ppm, ppm);
    delay(1000);
}