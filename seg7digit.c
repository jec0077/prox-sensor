/*
 * seg7.c: Starter code for the digital interface for the 4-digit 7-segment display.
 *
 * Created by Zhao Zhang for ECE 266 labs, fall 2022.
 *
 * Note: It is revised from the 7-segment code from previous semesters.
 */

#include <stdint.h>
#include <stdbool.h>
#include "seg7.h"

/*
 * 7-segment coding table. See https://en.wikipedia.org/wiki/Seven-segment_display. The segments
 * are named as A, B, C, D, E, F, G. In this coding table, segments A-G are mapped to bits 0-7.
 * Bit 7 is not used in the coding. This display uses active high signal, in which '1' turns ON a
 * segment, and '0' turns OFF a segment.
 *
 * Note: The table is incomplete. Fill the codes for digits 4-9.
 */
static uint8_t seg7_coding_table[10] = {
        0b00111111,         // digit 0
        0b00000110,         // digit 1
        0b01011011,         // digit 2
        0b01001111,         // digit 3
        0b01100110,         // digit 4
        0b01101101,         // digit 5
        0b01111101,         // digit 6
        0b00000111,         // digit 7
        0b01111111,         // digit 8
        0b01101111          // digit 9
        // YOUR CODE
};

/*
 * Update the 7-segment display using the Seg7Display structure.
 */
void Seg7Update(Seg7Display *seg7)
{
    uint8_t seg7_code[4];
    uint8_t colon_code;

    // Get the raw encoding for the colon
    colon_code = seg7->colon_on ? 0b10000000 : 0b00000000;

    // Get the raw encoding for 7-segment digits
    seg7_code[0] = seg7_coding_table[seg7->digit[0]] + colon_code;
    seg7_code[1] = seg7_coding_table[seg7->digit[1]] + colon_code;
    seg7_code[2] = seg7_coding_table[seg7->digit[2]] + colon_code;
    seg7_code[3] = seg7_coding_table[seg7->digit[3]] + colon_code;

    // Update the 7-segment
    Seg7RawUpdate(seg7_code);
}
