#include <avr/interrupt.h>
#include "traffic_light.h"

// Global variables
volatile TrafficState currentState = STATE_NS_RED_EW_GREEN;
volatile uint16_t timer_count = 0;
volatile uint8_t ns_pedestrian_waiting = 0;
volatile uint8_t ew_pedestrian_waiting = 0;

void init_ports(void) {
    SET_TRAFFIC_LIGHT_PINS_OUTPUT();
    SET_PEDESTRIAN_LIGHT_PINS_OUTPUT();
    SET_BUTTON_PINS_INPUT();
    DISABLE_BUTTON_PULLUPS();
}

void set_initial_state(void) {
    MY_PORT_B |= (NS_RED | EW_GREEN);
    MY_PORT_D |= (NS_PED_RED | EW_PED_GREEN);
}

/**
* Initialize Timer1 to generate interrupts at 100Hz frequency.
*
* This function configures Timer1 with the following settings:
* 1. CTC (Clear Timer on Compare match) mode via TCCR1B WGM12 bit
*    - Timer counts up and resets when reaching OCR1A value
*
* 2. Clock prescaler of 256 via TCCR1B CS12 bit
*    - CPU clock (16MHz) / 256 = 62.5kHz timer frequency
*
* 3. Compare match value of 625 in OCR1A register
*    - 62.5kHz / 625 = 100Hz interrupt frequency
*    - Each timer tick represents 1/100th of a second
*
* 4. Enable Timer Compare A Match Interrupt via TIMSK1 OCIE1A bit
*    - Triggers ISR(TIMER1_COMPA_vect) when timer reaches OCR1A
*/
void init_timer(void) {
    TCCR1B |= (1 << WGM12);    // CTC mode
    TCCR1B |= (1 << CS12);     // 256 prescaler
    OCR1A = 625;               // For 16MHz: 16M/256/625 = 100Hz interrupt rate
    TIMSK1 |= (1 << OCIE1A);   // Enable timer compare interrupt
}

void clear_all_lights(void) {
    MY_PORT_B &= ~(NS_RED | NS_YELLOW | NS_GREEN |
                   EW_RED | EW_YELLOW | EW_GREEN);
    MY_PORT_D &= ~(NS_PED_RED | NS_PED_GREEN | NS_PED_WHITE |
                   EW_PED_RED | EW_PED_GREEN | EW_PED_WHITE);
}

void update_lights(void) {
    clear_all_lights();

    uint8_t should_flash = (timer_count % 50) < 25;  // Will be 1 for 0.5s, then 0 for 0.5s

    switch (currentState) {
        case STATE_NS_RED_EW_GREEN:
            MY_PORT_B |= (NS_RED | EW_GREEN);
            MY_PORT_D |= (NS_PED_RED | EW_PED_GREEN);
            if (ns_pedestrian_waiting) {
              MY_PORT_D |= NS_PED_WHITE;  // Solid while waiting
            }
              if (ew_pedestrian_waiting && should_flash) {
              MY_PORT_D |= EW_PED_WHITE;  // Flash during green phase
            }
            break;

        case STATE_NS_RED_EW_YELLOW:
            MY_PORT_B |= (NS_RED | EW_YELLOW);
            MY_PORT_D |= (NS_PED_RED | EW_PED_RED);
            if (ns_pedestrian_waiting) {
              MY_PORT_D |= NS_PED_WHITE;
            }
            break;

        case STATE_NS_RED_EW_RED:
            MY_PORT_B |= (NS_RED | EW_RED);
            MY_PORT_D |= (NS_PED_RED | EW_PED_RED);
            if (ew_pedestrian_waiting) {
              MY_PORT_D |= EW_PED_WHITE;
            }
            if (ns_pedestrian_waiting) {
              MY_PORT_D |= NS_PED_WHITE;
            }
            break;

        case STATE_NS_REDYELLOW_EW_RED:
            MY_PORT_B |= (NS_RED | NS_YELLOW | EW_RED);
            MY_PORT_D |= (NS_PED_RED | EW_PED_RED);
            if (ns_pedestrian_waiting) {
              MY_PORT_D |= NS_PED_WHITE;
            }
            break;

        case STATE_NS_GREEN_EW_RED:
            MY_PORT_B |= (NS_GREEN | EW_RED);
            MY_PORT_D |= (NS_PED_GREEN | EW_PED_RED);
            if (ns_pedestrian_waiting && should_flash) {
              MY_PORT_D |= NS_PED_WHITE;  // Flash during green phase
            }
            if (ew_pedestrian_waiting) {
              MY_PORT_D |= EW_PED_WHITE;  // Solid while waiting
            }
            break;

        case STATE_NS_YELLOW_EW_RED:
            MY_PORT_B |= (NS_YELLOW | EW_RED);
            MY_PORT_D |= (NS_PED_RED | EW_PED_RED);
            if (ew_pedestrian_waiting) {
              MY_PORT_D |= EW_PED_WHITE;
            }
            break;

        case STATE_NS_RED_EW_RED_2:
            MY_PORT_B |= (NS_RED | EW_RED);
            MY_PORT_D |= (NS_PED_RED | EW_PED_RED);
            if (ew_pedestrian_waiting) {
              MY_PORT_D |= EW_PED_WHITE;
            }
            if (ns_pedestrian_waiting) {
              MY_PORT_D |= NS_PED_WHITE;
            }
            break;

        case STATE_NS_RED_EW_REDYELLOW:
            MY_PORT_B |= (NS_RED | EW_RED | EW_YELLOW);
            MY_PORT_D |= (NS_PED_RED | EW_PED_RED);
            if (ew_pedestrian_waiting) {
            MY_PORT_D |= EW_PED_WHITE;
            }
            if (ns_pedestrian_waiting) {
              MY_PORT_D |= NS_PED_WHITE;
            }
            break;
    }
}

void set_next_state(TrafficState state) {
    currentState = state;
}

void reset_timer(void) {
    timer_count = 0;
}

ISR(TIMER1_COMPA_vect) {
    timer_count++;

// Check button state - note these are pull-down.
  if (PINC & NS_BUTTON) {
    ns_pedestrian_waiting = 1;
  }
  if (PINC & EW_BUTTON) {
    ew_pedestrian_waiting = 1;
  }

    switch (currentState) {
        case STATE_NS_RED_EW_GREEN:
            if (timer_count >= (ns_pedestrian_waiting ? GREEN_TIME_SHORT : GREEN_TIME)) {
                set_next_state(STATE_NS_RED_EW_YELLOW);
                reset_timer();
                ew_pedestrian_waiting = 0;
            }
            break;

        case STATE_NS_RED_EW_YELLOW:
            if (timer_count >= YELLOW_TIME) {
                set_next_state(STATE_NS_RED_EW_RED);
                reset_timer();
            }
            break;

        case STATE_NS_RED_EW_RED:
            if (timer_count >= BOTH_RED_TIME) {
                set_next_state(STATE_NS_REDYELLOW_EW_RED);
                reset_timer();
            }
            break;

        case STATE_NS_REDYELLOW_EW_RED:
            if (timer_count >= REDYELLOW_TIME) {
                set_next_state(STATE_NS_GREEN_EW_RED);
                reset_timer();
            }
            break;

        case STATE_NS_GREEN_EW_RED:
            if (timer_count >= (ew_pedestrian_waiting ? GREEN_TIME_SHORT : GREEN_TIME)) {
                set_next_state(STATE_NS_YELLOW_EW_RED);
                reset_timer();
                ns_pedestrian_waiting = 0;
            }
            break;

        case STATE_NS_YELLOW_EW_RED:
            if (timer_count >= YELLOW_TIME) {
                set_next_state(STATE_NS_RED_EW_RED_2);
                reset_timer();
            }
            break;

        case STATE_NS_RED_EW_RED_2:
            if (timer_count >= BOTH_RED_TIME) {
                set_next_state(STATE_NS_RED_EW_REDYELLOW);
                reset_timer();
            }
            break;

        case STATE_NS_RED_EW_REDYELLOW:
            if (timer_count >= REDYELLOW_TIME) {
                set_next_state(STATE_NS_RED_EW_GREEN);
                reset_timer();
            }
            break;
    }

    update_lights();
}

int main(void) {
    init_ports();
    set_initial_state();
    init_timer();
    sei();

    while (1) {
        // Everything is handled in interrupts
    }
}