#include <stdint.h>

#define	DIM7	0 // base of all chords
#define SUS4	0b0010000011001010
#define SUS2	0b0100000011001100
#define MAJOR	0b0001000011001001
#define MINOR	0b0000000011001000

void chord_generate(uint8_t *notes_out, uint8_t number_of_notes, uint8_t basenote, uint16_t chordtype);