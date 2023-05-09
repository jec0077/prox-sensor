/*
 * ranger.h
 *
 * Author: Joshua Campbell
 *
 */

#ifndef RANGER_H_
#define RANGER_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/adc.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>

void RangerInit();

void RangerTriggerReading();

bool RangerDataReady();

uint32_t RangerReadRaw();

#endif /* RANGER_H_ */
