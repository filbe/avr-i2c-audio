#include "avr-i2c-audio.h"
#include "chords.h"

void chord_generate(uint8_t *notes_out, uint8_t number_of_notes, uint8_t basenote, uint16_t chordtype) {
	uint8_t notecount=0;
	uint8_t dim_flatsharp;
	for (uint8_t octave=0; octave <= (number_of_notes/5); octave++) {
		for (uint8_t n=0;n<5 && notecount<number_of_notes ;n++) {
			if (notecount == 0) {
				notes_out[0] = basenote;
				notecount++;
				continue;
			}
			dim_flatsharp = (chordtype >> (3*(notecount-1))) & 0x7;
			uint8_t minus = dim_flatsharp >> 2;
			dim_flatsharp = dim_flatsharp & 3;
			if (minus)
				dim_flatsharp = -dim_flatsharp-1;

			notes_out[notecount] = basenote + ((notecount*3)) + dim_flatsharp * (notecount > 0);
			notecount++;
		}
	}
}