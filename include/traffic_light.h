#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include <avr/io.h>

// Direct port addressing
#define MY_PORT_B *(unsigned char*)0x25    // Port B Data Register
#define MY_PORT_B_DDR *(unsigned char*)0x24 // Port B Data Direction Register
#define MY_PORT_C *(unsigned char*)0x28    // Port C Data Register
#define MY_PORT_C_DDR *(unsigned char*)0x27 // Port C Data Direction Register
#define MY_PORT_D *(unsigned char*)0x2B    // Port D Data Register
#define MY_PORT_D_DDR *(unsigned char*)0x2A // Port D Data Direction Register

// States for traffic light system
typedef enum {
    STATE_NS_RED_EW_GREEN,      // NS: Red, EW: Green
    STATE_NS_RED_EW_YELLOW,     // NS: Red, EW: Yellow
    STATE_NS_RED_EW_RED,        // Both: Red (before NS green)
    STATE_NS_REDYELLOW_EW_RED,  // NS: Red+Yellow, EW: Red
    STATE_NS_GREEN_EW_RED,      // NS: Green, EW: Red
    STATE_NS_YELLOW_EW_RED,     // NS: Yellow, EW: Red
    STATE_NS_RED_EW_RED_2,      // Both: Red (before EW green)
    STATE_NS_RED_EW_REDYELLOW   // NS: Red, EW: Red+Yellow
} TrafficState;

// Pin definitions for traffic lights (PORT B)
#define NS_RED     (1<<5)    // PB5
#define NS_YELLOW  (1<<4)    // PB4
#define NS_GREEN   (1<<3)    // PB3
#define EW_RED     (1<<2)    // PB2
#define EW_YELLOW  (1<<1)    // PB1
#define EW_GREEN   (1<<0)    // PB0

// Pin definitions for pedestrian lights (PORT D)
#define NS_PED_RED    (1<<7)  // PD7
#define NS_PED_GREEN  (1<<6)  // PD6
#define NS_PED_WHITE  (1<<5)  // PD5
#define EW_PED_RED    (1<<4)  // PD4
#define EW_PED_GREEN  (1<<3)  // PD3
#define EW_PED_WHITE  (1<<2)  // PD2

// Pin definitions for pedestrian buttons (PORT D)
#define NS_BUTTON     (1<<0)  // PC0
#define EW_BUTTON     (1<<1)  // PC1

// Helper macros for setting pins for lights and buttons
#define SET_TRAFFIC_LIGHT_PINS_OUTPUT() (MY_PORT_B_DDR |= (NS_RED | NS_YELLOW | NS_GREEN | EW_RED | EW_YELLOW | EW_GREEN))
#define SET_PEDESTRIAN_LIGHT_PINS_OUTPUT() (MY_PORT_D_DDR |= (NS_PED_RED | NS_PED_GREEN | NS_PED_WHITE | EW_PED_RED | EW_PED_GREEN | EW_PED_WHITE))
#define SET_BUTTON_PINS_INPUT() (MY_PORT_C_DDR &= ~(NS_BUTTON | EW_BUTTON))
#define DISABLE_BUTTON_PULLUPS() (MY_PORT_C &= ~(NS_BUTTON | EW_BUTTON))

// Timing constants (in timer ticks)
#define GREEN_TIME        1500     // 15 seconds
#define GREEN_TIME_SHORT  800      // 8  seconds
#define YELLOW_TIME       200      // 2  seconds
#define BOTH_RED_TIME     300      // 3  seconds
#define REDYELLOW_TIME    200      // 2  seconds

// Function declarations
void init_ports(void);
void set_initial_state(void);
void init_timer(void);
void clear_all_lights(void);
void update_lights(void);
void set_next_state(TrafficState state);
void reset_timer(void);

#endif // TRAFFIC_LIGHT_H