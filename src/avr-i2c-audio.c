#include <avr-sound.h>
#include <avr-i2c-audio.h>
#include <twi.h>

#define DEVICE_ID 1

float midi[100];

void receive_i2c() {
	uint8_t channel = twi_rxBuffer[0];
	uint8_t command = twi_rxBuffer[1];
	uint8_t note = twi_rxBuffer[2];

	if (command == 1)
		avrsound_set_hz(channel, midi[note]);
}

int main(void) {

	for (float i=0;i<100;i+=1.0) {
		midi[(uint8_t)(i)] = pow(2.0, (i-69.0)*0.083333)*440.0;
	}

	DDRD=255;
	DDRC = 0;
	DDRB = 255;
	twi_init();	
	twi_setAddress(DEVICE_ID);
	twi_attachSlaveRxEvent(receive_i2c);

	avrsound_init();
	avrsound_sample_init(256, 440.0);

	for (uint16_t b=0;b<256;b++) {
		// Buffer range is -128....127
		//avrsound_setbuffer(b, sin(b/256.0*M_PI)*128.0); // SINE WAVE
		avrsound_setbuffer(b, b-128); // SAWTOOTH
		//avrsound_setbuffer(b, b/128*128); // SQUARE WAVE
	}
	sei();


	while(1) {

	}
	return 0;
}