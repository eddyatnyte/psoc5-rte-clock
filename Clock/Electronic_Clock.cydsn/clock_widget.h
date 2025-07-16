/* ========================================
 *
 * Copyright (c) 2025 Hochschule Darmstadt
 * All Rights Reserved
 *
 * Developed by Edaet Boloban
 * M.Sc. Electrical Engineering (Major: Embedded and Microelectronics)
 *
 * This project is open source and published for educational purposes.
 *
 * ========================================
*/

#ifndef CLOCK_WIDGET_H
#define CLOCK_WIDGET_H

// Include necessary headers for widget container and clock functionality
#include "clock_container.h"

#include "sp_common.h"
#include "swc_clock.h"

// Enum to distinguish between hour and minute widgets
typedef enum {
    WIDGET_MINUTE,    // Represents the minute widget
    WIDGET_HOUR       // Represents the hour widget
} WidgetType;

// Enum to define the finite state machine (FSM) states of a clock widget
typedef enum {
    CLOCK_W_ISDISPLAYING,       // Widget is currently displaying time (optional state)
    CLOCK_W_ISFOCUS,            // Widget is in focus, allowing manual adjustment
    CLOCK_W_ISAUTOINCREMENT     // Widget is automatically incrementing (e.g., by a timer)
} ClockWidgetState;

// Structure representing an instance of a clock widget (hour or minute)
typedef struct {
    ClockWidgetState state;     // Current state of the widget (FSM)
    uint8_t* valuePtr;          // Pointer to the value being displayed/managed (e.g., hour/minute)
    uint8_t maxValue;           // Maximum allowed value (e.g., 59 for minutes, 23 for hours)
    WidgetType type;            // Type of widget (hour or minute)
} ClockWidgetInstance;

// External declarations of hour and minute widget instances
extern ClockWidgetInstance CLOCK_hours;
extern ClockWidgetInstance CLOCK_minutes;

// Function to update the time based on widget state or external input
void CLOCK__updateTime();

// Function to update the visual display of the clock widgets
void CLOCK__updateDisplay();

// Function to set the fonts used in displaying the clock (visual style settings)
void CLOCK__setDisplayFonts();

// General function to increment the currently focused widget (hour or minute)
void CLOCK__incrementWidget();

// Direct function to increment the hour widget value
void CLOCK__incrementHours();

// Direct function to increment the minute widget value
void CLOCK__incrementMinutes();

// Event processing function for clock widgets (handles user or system events)
void WIDGET__processEvent(ClockWidgetInstance* instance, EVENT_enum_t event);

#endif 
/* [] END OF FILE */
