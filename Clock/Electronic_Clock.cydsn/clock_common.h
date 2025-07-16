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

#ifndef CLOCK_COMMON_H
#define CLOCK_COMMON_H

#include "sc_event_type.h"
      
// Transition entry for one state transition
typedef struct {
    EVENT_enum_t event;                    // Event
    int nextState;         // ToState
    boolean_t (*guard)(void);              // Guard
    void (*action)(void);                  // Action 
} STATE_stateInnerTransitionTable_t;


typedef struct {
    int currentState;
    const STATE_stateInnerTransitionTable_t* transition;
    const uint16_t tableSize;
} STATE_stateOuterTransitionTable_t;
    
#endif

/* [] END OF FILE */
