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

#ifndef CLOCK_CONTAINER_H
#define CLOCK_CONTAINER_H

#include "sc_event_type.h"
#include "clock_widget.h"
#include "clock_common.h"

// FSM states of the container
typedef enum {
    CLOCK_C_ISDISPLAYING,     // Clock is currently displaying the time
    CLOCK_C_ISEDITINGMINUTES,  // User is editing the minutes
    CLOCK_C_ISEDITINGHOURS     // User is editing the hours
} ClockContainerState;

// Defines the transition actions
void CLOCK__increment1Minute();
void CLOCK__dispatch_KL_Hours();
void CLOCK__dispatch_KR_Hours();
void CLOCK__dispatch_KRLP_Hours();
void CLOCK__dispatch_250MS_Hours();
void CLOCK__dispatch_KL_Min();
void CLOCK__dispatch_KR_Min();
void CLOCK__dispatch_KRLP_Min();
void CLOCK__dispatch_250MS_Min();
void CLOCK__dispatch_KL_HoursMin();

void CONTAINER_processEvent(ClockContainerState* state, EVENT_enum_t event);

#endif
/* [] END OF FILE */
