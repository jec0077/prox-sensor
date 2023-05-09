/*
 * ECE 266 B12 Lab Project, Spring 2023
 *
 * main.c
 *
 * Authors: Joshua Campbell & Aaron Tillery
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "ranger.h"
#include "buzzer.h"
#include "seg7.h"


typedef struct {
    uint32_t delay;
    bool activated;
    bool buzzer_on;
} SysState;

static SysState sys =
{
     100,
     false,
     false
};

Seg7Display seg7 = { { 10, 10, 10, 10 }, false /* colon on */};


void BuzzerPlay(uint32_t time)                  // the scheduled time
{
    uint32_t delay = 500;                       // default checking interval

    if (sys.activated)
    {
        // Turn buzzer on and off
        if (!sys.buzzer_on)
        {
            BuzzerOff();
            sys.buzzer_on = false;
            delay = 2985;
        }
        else
        {
            BuzzerOn();
            sys.buzzer_on = true;
            delay = 15 + sys.delay;
        }
    }
    else
    {
        // Make sure the buzzer is turned off
        if (sys.buzzer_on)
        {
            BuzzerOff();
            sys.buzzer_on = false;
        }
    }

    WaitUs(10000);
    BuzzerOff();

    // schedule callback to buzzer play
    ScheduleCallback(BuzzerPlay, time + delay);
}


void EvaluateProx(double dist) {
    if (dist <= 60.0) {
        uprintf("\t<<<DANGER>>>\r\n");

        sys.buzzer_on = true;
        sys.delay = 100;
        LedTurnOnOff(true /* red */, false /* blue */, false /* green */);

    } else if (dist > 60.0 && dist <= 80.0) {
        uprintf("\t<<<WARNING>>>\r\n");

        sys.buzzer_on = true;
        sys.delay = 1000;
        LedTurnOnOff(true /* red */, false /* blue */, true /* green */);

    } else if (dist > 80.0 && dist <= 100.0) {
        uprintf("\t<<<WATCH>>>\r\n");

        sys.buzzer_on = false;
        sys.delay = 0;
        LedTurnOnOff(false /* red */, false /* blue */, true /* green */);

    } else {
        uprintf("\t<<<NOTHING>>>\r\n");

        sys.buzzer_on = false;
        sys.delay = 0;
        LedTurnOnOff(false /* red */, false /* blue */, false /* green */);
    }
}


void RangerSampling(uint32_t time)
{
    RangerTriggerReading();

    ScheduleCallback(RangerSampling, time + 1000);
}


void CheckNewRangerReading()
{
    if (sys.activated) {
        if (!RangerDataReady())
            return;

        uint32_t raw_data = RangerReadRaw();

        double distance = (17.0 / 5000000.0) * (double)raw_data * 1000.0;

        seg7.digit[3] = (uint8_t)((uint32_t)distance / 1000);
        seg7.digit[2] = (uint8_t)(((uint32_t)distance % 1000) / 100);
        seg7.digit[1] = (uint8_t)(((uint32_t)distance % 100) / 10);
        seg7.digit[0] = (uint8_t)((uint32_t)distance % 10);

        Seg7Update(&seg7);

        uprintf("Distance Reading: %0.2f mm", distance);
        EvaluateProx(distance);

    }
}


void CheckPushButtons()
{
    // Process the pushbutton input
    switch (PbRead())
    {
    case 1:
        if (!sys.activated) {
            sys.activated = true;

            Seg7Update(&seg7);
            uprintf("Sys ON ");
        } else {
            sys.activated = false;
            sys.buzzer_on = false;
            sys.delay = 0;

            seg7.digit[3] = 10;
            seg7.digit[2] = 10;
            seg7.digit[1] = 10;
            seg7.digit[0] = 10;

            seg7.colon_on = false;

            Seg7Update(&seg7);
            LedTurnOnOff(false /* red */, false /* blue */, false /* green */);
            uprintf("Sys OFF ");
        }
        uprintf("SW1 pushed\r\n");
        break;

    case 2:

        uprintf("SW2 pushed\r\n");
        break;
    }
}



void main(void)
{
    LpInit();
    RangerInit();
    Seg7Init();
    BuzzerInit();

    uprintf("%s\n\r", "B12 Lab Project: Car Proximity Sensor");

    // Schedule first callback events
    ScheduleCallback(BuzzerPlay, 1000);
    ScheduleCallback(RangerSampling, 1000);

    // Loop forever
    while (true)
    {
        // Wait for interrupt
        __asm("   wfi");

        // Check new ranger reading
        CheckNewRangerReading();

        // Check push button events
        CheckPushButtons();

        // Call the callback scheduler
        ScheduleExecute();
    }
}
