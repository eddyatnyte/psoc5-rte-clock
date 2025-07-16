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

#include "clock_container.h"

// =================================== Container ===================================

/** ==== CLOCK_C_ISDISPLAYING ===== */
const STATE_stateInnerTransitionTable_t STATE_CLOCK_C_ISDISPLAYING_Transitions[] = {
/*    Event                     ToState                Guard    Action                   */
    { EV_KEYLEFT,               CLOCK_C_ISEDITINGHOURS, 0,      CLOCK__dispatch_KL_Hours },
    { EV_1MIN,                  CLOCK_C_ISDISPLAYING, 0,        CLOCK__increment1Minute }
};

/** ==== CLOCK_C_ISEDITINGHOURS ===== */
const STATE_stateInnerTransitionTable_t STATE_CLOCK_C_ISEDITINGHOURS_Transitions[] = {
/*    Event                     ToState                Guard    Action                   */
    { EV_KEYLEFT,               CLOCK_C_ISEDITINGMINUTES, 0,    CLOCK__dispatch_KL_HoursMin } ,
    { EV_KEYRIGHT,              CLOCK_C_ISEDITINGHOURS,   0,    CLOCK__dispatch_KR_Hours },
    { EV_KEYRIGHTLONGPRESS,     CLOCK_C_ISEDITINGHOURS,   0,    CLOCK__dispatch_KRLP_Hours },
    { EV_250MS,                 CLOCK_C_ISEDITINGHOURS,   0,    CLOCK__dispatch_250MS_Hours }
};

/** ==== CLOCK_C_ISEDITINGMINUTES ===== */
const STATE_stateInnerTransitionTable_t STATE_CLOCK_C_ISEDITINGMINUTES_Transitions[] = {
/*    Event                     ToState                Guard    Action                   */
    { EV_KEYLEFT,               CLOCK_C_ISDISPLAYING,    0,     CLOCK__dispatch_KL_Min },
    { EV_KEYRIGHT,              CLOCK_C_ISEDITINGMINUTES, 0,    CLOCK__dispatch_KR_Min },
    { EV_KEYRIGHTLONGPRESS,     CLOCK_C_ISEDITINGMINUTES, 0,    CLOCK__dispatch_KRLP_Min },
    { EV_250MS,                 CLOCK_C_ISEDITINGMINUTES, 0,    CLOCK__dispatch_250MS_Min }
};

// =============== Outer Table ===============

const STATE_stateOuterTransitionTable_t CLOCK_Container_Transitions[] = {
    { CLOCK_C_ISDISPLAYING,      STATE_CLOCK_C_ISDISPLAYING_Transitions, sizeof(STATE_CLOCK_C_ISDISPLAYING_Transitions) / sizeof(STATE_stateInnerTransitionTable_t) },
    { CLOCK_C_ISEDITINGHOURS,     STATE_CLOCK_C_ISEDITINGHOURS_Transitions, sizeof(STATE_CLOCK_C_ISEDITINGHOURS_Transitions) / sizeof(STATE_stateInnerTransitionTable_t) },
    { CLOCK_C_ISEDITINGMINUTES,   STATE_CLOCK_C_ISEDITINGMINUTES_Transitions, sizeof(STATE_CLOCK_C_ISEDITINGMINUTES_Transitions) / sizeof(STATE_stateInnerTransitionTable_t) }
};

const uint16_t CLOCK_Container_Transitions_Size = sizeof(CLOCK_Container_Transitions)/sizeof(STATE_stateOuterTransitionTable_t);

// Function in order to process the events
void CONTAINER_processEvent(ClockContainerState* currentState, EVENT_enum_t event) {
    for (uint8_t i = 0; i < CLOCK_Container_Transitions_Size; ++i) {
        if (CLOCK_Container_Transitions[i].currentState == *currentState) {
            const STATE_stateInnerTransitionTable_t* table = CLOCK_Container_Transitions[i].transition;
            uint16_t size = CLOCK_Container_Transitions[i].tableSize;

            for (uint16_t j = 0; j < size; ++j) {
                if (table[j].event == event) {
                    if (table[j].guard == NULL || table[j].guard()) {
                        if (table[j].action) {
                            table[j].action();
                        }
                        *currentState = table[j].nextState;
                        return;
                    }
                }
            }
        }
    }
}


void CLOCK__increment1Minute(void) {
    g_minutes++;
    
    // If 60 minutes have been reached,
    if(g_minutes >= 60) {
        // increase the current hour
        g_hours++;
         
        // and reset the counter for the minutes.
        g_minutes = 0;
        
        // if 24 hours have been reached,
        if(g_hours >= 24){
            // reset the hours
            g_hours = 0;
        }
    }
        
}

void CLOCK__dispatch_KL_Hours(void) {
    WIDGET__processEvent(&CLOCK_hours, EV_KEYLEFT);
}

void CLOCK__dispatch_KL_HoursMin(void) {
    WIDGET__processEvent(&CLOCK_hours, EV_KEYLEFT);
    WIDGET__processEvent(&CLOCK_minutes, EV_KEYLEFT);
}

void CLOCK__dispatch_KR_Hours(void) {
    WIDGET__processEvent(&CLOCK_hours, EV_KEYRIGHT);
}

void CLOCK__dispatch_KRLP_Hours(void) {
    WIDGET__processEvent(&CLOCK_hours, EV_KEYRIGHTLONGPRESS);
}

void CLOCK__dispatch_250MS_Hours(void) {
    WIDGET__processEvent(&CLOCK_hours, EV_250MS);
}

void CLOCK__dispatch_KL_Min(void) {
    WIDGET__processEvent(&CLOCK_minutes, EV_KEYLEFT);
}

void CLOCK__dispatch_KR_Min(void) {
    WIDGET__processEvent(&CLOCK_minutes, EV_KEYRIGHT);
}

void CLOCK__dispatch_KRLP_Min(void) {
    WIDGET__processEvent(&CLOCK_minutes, EV_KEYRIGHTLONGPRESS);
}

void CLOCK__dispatch_250MS_Min(void) {
    WIDGET__processEvent(&CLOCK_minutes, EV_250MS);
}

/* [] END OF FILE */
