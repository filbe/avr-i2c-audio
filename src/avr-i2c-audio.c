#include <avr-i2c-audio.h>


float midi[120];
volatile uint8_t channel_fading[AVRSOUND_MAX_CHANNELS];

#define CHANNEL_FADE_SETTING	3

void play_triplet(uint8_t *notes)
{
	avrsound_set_hz(0, midi[notes[0]]);
	avrsound_set_hz(1, midi[notes[1]]);
	avrsound_set_hz(2, midi[notes[2]]);
}

void receive_i2c()
{
	uint8_t channel = twi_rxBuffer[0];
	uint8_t channel_settings = channel >> 4;
	channel = channel & 0x0f;
	uint8_t command = twi_rxBuffer[1];
	uint8_t note[8];
	uint8_t notedelay;
	uint8_t notescount;
	note[0] = twi_rxBuffer[2];
	uint8_t i = 0;
	switch (command) {
	case 0x00: // NOTE OFF CHANNEL N OR ALL CHANNELS
		if (channel == 0xff) {
			for (channel = 0; channel < AVRSOUND_MAX_CHANNELS; channel++) {
				avrsound_set_hz(channel, 0);
			}
		} else {

			if (channel_settings & (1 << CHANNEL_FADE_SETTING)) {
			} else {
				avrsound_set_hz(channel, 0);
			}
		}
		break;
	case 0x01: // NOTE ON CHANNEL N
		avrsound_set_hz(channel, midi[note[0]]);
		break;
	case 0x02: // HZ ON CHANNEL N
		avrsound_set_hz(channel, ((twi_rxBuffer[2] * 256) + twi_rxBuffer[3]) + twi_rxBuffer[4] / 256.0);
		break;
	case 0x10: // SET OSCILLATOR TO SQUARE WAVE
		for (uint16_t b = 0; b < 256; b++) {
			avrsound_setbuffer(b, (b < 128) * 255); // SQUARE WAVE
		}
		break;
	case 0x11: // SET OSCILLATOR TO SAW WAVE
		for (uint16_t b = 0; b < 256; b++) {
			avrsound_setbuffer(b, b - 128); // SAWTOOTH
		}
		break;
	case 0x12: // SET OSCILLATOR TO SINE WAVE
		for (uint16_t b = 0; b < 256; b++) {
			avrsound_setbuffer(b, sin(2 * b / 256.0 * M_PI) * 127 + 128); // SINE WAVE
		}
		break;
	case 0x13: // SET OSCILLATOR TO WHITE NOISE
		for (uint16_t b = 0; b < 256; b++) {
			avrsound_setbuffer(b, rand());
		}
		break;
	case 0x20: // FINE TUNE
		avrsound_finetune((twi_rxBuffer[2] << 8) + twi_rxBuffer[3]);
		break;

	case 0x31: // set buffer value
		avrsound_setbuffer((twi_rxBuffer[2] << 8) + twi_rxBuffer[3], twi_rxBuffer[4]);
		break;

	case 0x41: // set channel volume
		avrsound_set_volume(twi_rxBuffer[2], twi_rxBuffer[3]);
		break;

	case 0x51: // note arpeggio
		notescount = twi_rxBuffer[2];
		notedelay = twi_rxBuffer[3];
		for (i = 0; i < notescount; i++) {
			note[i] = twi_rxBuffer[4 + i];
		}
		init_arpeggio(notescount, note, notedelay);
		break;

	case 0x61: // triplet
		chord_generate(note, 3, twi_rxBuffer[4], (twi_rxBuffer[2] << 8) + twi_rxBuffer[3]);
		play_triplet(note);
		break;

	case 0xf0: // set samplerate
		avrsound_set_samplerate((twi_rxBuffer[2] << 8) + twi_rxBuffer[3]);
		break;
	}
}

void arpeggio_play_note(uint8_t note)
{
	avrsound_set_hz(0, midi[note]);
	//avrsound_set_hz(1, midi[note]);
	//avrsound_set_hz(2, midi[note]);
}

int main(void)
{

	for (uint8_t o = 0; o < AVRSOUND_MAX_CHANNELS; o++) {
		channel_fading[o] = 0;
		fade_start_volume[o] = 0;
		fade_time[o] = 0;
		fade_cursor[o] = 0;
	}
	for (float i = 0; i < 120; i += 1.0) {
		midi[(uint8_t)(i)] = pow(2.0, (i - 69.0) * 0.083333) * 440.0;
	}

	DDRD = 255;
	DDRC = 0;
	DDRB = 255;
	twi_init();
	twi_setAddress(DEVICE_ID);
	twi_attachSlaveRxEvent(receive_i2c);

	avrsound_init();
	avrsound_sample_init(256, 440.0);
	avrsound_set_samplerate(15000);

	for (uint16_t b = 0; b < 256; b++) {
		avrsound_setbuffer(b, sin(2.0 * 3.14159265 * (float)(b) / 256.0) * 126.5 + 127.5); // SINE WAVE
		//avrsound_setbuffer(b, b);
		//else avrsound_setbuffer(b, 256 - (b - 128));
		//avrsound_setbuffer(b, (b < 128) ? 255 : 0);

	}

	uint8_t notestoplay[] = {
		62,57,69,70,62,67,69,62,65,67,62,64,65,60,
		62,57,69,70,62,67,69,62,72,62,70,62,67,69,
		62,57,69,70,62,67,69,62,65,67,65,64,62,60,
		58,53,65,58,65,67,58,65,55,64,60,55,65,64,60,57
	};

	avrsound_set_volume(0, 120); //avrsound_set_volume(1, 30); avrsound_set_hz(1, 256);
	//avrsound_set_volume(2, 60);avrsound_set_hz(2,75);




	//avrsound_set_hz(0,600);

	adc_init();

	sei();



	while (1) {
		for (uint8_t i = 0; i < sizeof(notestoplay); i++) {
			avrsound_set_hz(0, midi[notestoplay[i]]);
			_delay_ms(25);
			//avrsound_set_hz(0, midi[0]);
			//_delay_ms(20);
		}

	}
	return 0;
}