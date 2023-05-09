/*
 * ranger.c
 *
 * Author: Joshua Campbell
 *
 */

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
#include "launchpad.h"
#include "ranger.h"

typedef struct {
    uint32_t raw_data;
    uint32_t rising_edge;
    uint32_t falling_edge;
    bool edge_captured;
    bool new_data_ready;
} RangerState;

static RangerState ranger = {0, 0, 0, false, false};

static void RangerISR();

void RangerInit()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PD1_WT2CCP1);

    TimerConfigure(WTIMER2_BASE,
                   (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_CAP_TIME_UP));

    TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_BOTH_EDGES);

    TimerIntRegister(WTIMER2_BASE, TIMER_B, RangerISR);

    TimerIntEnable(WTIMER2_BASE, TIMER_CAPB_EVENT);

    TimerEnable(WTIMER2_BASE, TIMER_B);
}

void RangerTriggerReading()
{
    ranger.new_data_ready = false;
    ranger.edge_captured = false;

    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);

    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    WaitUs(2);

    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    WaitUs(5);

    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);

    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);

    GPIOPinConfigure(GPIO_PD1_WT2CCP1);

    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
}

bool RangerDataReady()
{
    return ranger.new_data_ready;
}

uint32_t RangerReadRaw()
{
    ranger.new_data_ready = false;
    ranger.edge_captured = false;
    ranger.raw_data = ranger.falling_edge - ranger.rising_edge;
    return ranger.raw_data;
}

static void RangerISR()
{
    int time = TimerValueGet(WTIMER2_BASE, TIMER_B);

    if (!ranger.edge_captured) {
        ranger.rising_edge = time;
        ranger.edge_captured = true;
    } else {
        ranger.falling_edge = time;
        ranger.new_data_ready = true;
    }

    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
}
