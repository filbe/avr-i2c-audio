#include <avr/io.h>
#include <avr/interrupt.h>
#include "arpeggio.h"

#define MAXIMUM_ARPEGGIO_LEN	8
volatile uint8_t arp_notes[MAXIMUM_ARPEGGIO_LEN];
volatile uint8_t arp_notes_count = 0;
volatile uint8_t arp_delay_between_notes = 0;
volatile uint8_t arpeggio_time = 0;
volatile uint8_t arpeggio_cursor = 0;
void arpeggio_play_note(uint8_t note);

void init_arpeggio(uint8_t notes_count, uint8_t *note, uint8_t delay_between_notes) {
	for (uint8_t i=0;i<notes_count;i++) {
		arp_notes[i] = note[i];
	}
	arp_notes_count = notes_count;
	arp_delay_between_notes = delay_between_notes;

	TCCR0A |= (1 << WGM01); // timer0 ctc mode
	OCR0A = 250;
	TIMSK0 |= (1 << OCIE0A);
	sei();
	TCCR0B |= (1 << CS01) | (1 << CS00); // timer0 prescaler -> 64 and start timer
	
	arpeggio_play_note(arp_notes[arpeggio_cursor]);
}

void arpeggio_stop() {
	TCCR0B &= ~((1 << CS01) | (1 << CS00));
}

ISR (TIMER0_COMPA_vect) {
	if (arpeggio_time > arp_delay_between_notes) {
		arpeggio_time -= arp_delay_between_notes;
		arpeggio_cursor = (arpeggio_cursor + 1) %  arp_notes_count;
		arpeggio_play_note(arp_notes[arpeggio_cursor]);
	}
	arpeggio_time++;
}