/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
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
