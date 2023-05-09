/*
 * seg7digit.h
 *
 * Created by Zhao Zhang for ECE 266, fall 2022
 */

#include <stdint.h>
#include <stdbool.h>

#ifndef SEG7_H_
#define SEG7_H_

/*
 * The state of the 4-digit 7-segment display
 */
typedef struct {
    uint8_t digit[4];           // 4 digits, digit[0] for the right most
    bool colon_on;              // if the colon (dots) is on or off
} Seg7Display;

/*
 * Interface functions
 */

// Initialize the port connection to TiM1637 and the 7-segment display
void Seg7Init();

// Update all 4 digits of the 7-segment displays
void Seg7RawUpdate(uint8_t code[]);

// Update the 4-digit 7-segment display with digit numbers (not 7-segment display pattern)
void Seg7Update(Seg7Display *seg7);

#endif /* SEG7_H_ */
