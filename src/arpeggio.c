#include <avr/io.h>
#include <avr/interrupt.h>
#include "arpeggio.h"
#include "avr-i2c-audio.h"

#define MAXIMUM_ARPEGGIO_LEN	16

#define MODE_NORMAL		0
#define MODE_ARPEGGIO	1

volatile uint8_t play_mode = MODE_NORMAL;
volatile uint8_t arp_notes[MAXIMUM_ARPEGGIO_LEN];
volatile uint8_t arp_notes_count = 0;
volatile uint8_t arp_delay_between_notes = 0;
volatile uint32_t arpeggio_time = 0;
volatile uint8_t arpeggio_cursor = 0;
volatile arpeggio_on = 0;


void arpeggio_play_note(uint8_t note);

void init_arp_timer()
{
	TCCR0A |= (1 << WGM01); // timer0 ctc mode
	OCR0A = 250;
	TIMSK0 |= (1 << OCIE0A);
	sei();
	TCCR0B |= (1 << CS01) | (1 << CS00); // timer0 prescaler -> 64 and start timer
}
void init_arpeggio(uint8_t notes_count, uint8_t *note, uint8_t delay_between_notes)
{
	for (uint8_t i = 0; i < notes_count; i++) {
		arp_notes[i] = note[i];
	}
	arp_notes_count = notes_count;
	arp_delay_between_notes = delay_between_notes;

	if (!arpeggio_on) {
		init_arp_timer();
		arpeggio_on = 1;
	}


	arpeggio_play_note(arp_notes[arpeggio_cursor]);
}

void arpeggio_stop()
{
	if (arpeggio_on) {
		TCCR0B &= ~((1 << CS01) | (1 << CS00));
		arpeggio_on = 0;
	}
}

uint8_t fade_new_volume(uint32_t start_vol, uint32_t cursor, uint32_t length)
{
	return start_vol - (start_vol * cursor / length);
}

void fade_start(uint8_t channel, uint16_t time)
{
	fade_start_volume[channel] = 0;
	channel_fading[channel] = 0;
	fade_time[channel] = 0;
	fade_cursor[channel] = 0;

	fade_start_volume[channel] = avrsound_get_volume(channel);
	channel_fading[channel] = 1;
	fade_time[channel] = time;
}

void fade_reset(uint8_t channel) {
	
	fade_start_volume[channel] = 0;
	channel_fading[channel] = 0;
	fade_time[channel] = 0;
	fade_cursor[channel] = 0;
}
void fade_stop(uint8_t channel)
{
	return;
	fade_reset(channel);
	avrsound_set_volume(channel, fade_start_volume[channel]);
	avrsound_set_hz(channel, 0);
}

ISR (TIMER0_COMPA_vect)
{
	switch (play_mode) {
	case MODE_NORMAL:

		for (uint8_t i = 0; i < AVRSOUND_MAX_CHANNELS; i++) {
			if (channel_fading[i]) {
				if (fade_cursor[i] % 8 == 0) {
					avrsound_set_volume(i, fade_new_volume(fade_start_volume[i], fade_cursor[i], fade_time[i]));
				}
				fade_cursor[i]++;
				if (fade_cursor[i] > fade_time[i]) {
					fade_stop(i);
					avrsound_set_volume(i, 1);
				}
			}
		}

		break;
	case MODE_ARPEGGIO:
		if (arpeggio_time > arp_delay_between_notes) {
			arpeggio_time -= arp_delay_between_notes;
			arpeggio_cursor = (arpeggio_cursor + 1) %  arp_notes_count;
			arpeggio_play_note(arp_notes[arpeggio_cursor]);
		}
		arpeggio_time++;
		break;

	}
}