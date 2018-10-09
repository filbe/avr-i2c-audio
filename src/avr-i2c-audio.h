#include <avr-sound.h>
#include <twi.h>
#include <math.h>
#include <stdlib.h>
#include <util/delay.h>
#include <stdint.h>
#include "arpeggio.h"
#include "chords.h"

volatile uint32_t fade_time[AVRSOUND_MAX_CHANNELS];
volatile uint32_t fade_cursor[AVRSOUND_MAX_CHANNELS];
volatile uint8_t channel_fading[AVRSOUND_MAX_CHANNELS];
volatile uint16_t fade_start_volume[AVRSOUND_MAX_CHANNELS];