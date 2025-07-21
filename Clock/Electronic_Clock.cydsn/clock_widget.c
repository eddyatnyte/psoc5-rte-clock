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

#include "clock_widget.h"

static ClockWidgetInstance* currentWidgetContext = NULL;

// =================================== Widget ===================================

ClockWidgetInstance CLOCK_hours = {
    .state = CLOCK_W_ISFOCUS,
    .valuePtr = &g_hours,
    .maxValue = 24,
    .type = WIDGET_HOUR
};

ClockWidgetInstance CLOCK_minutes = {
    .state = CLOCK_W_ISFOCUS,
    .valuePtr = &g_minutes,
    .maxValue = 60,
    .type = WIDGET_MINUTE
};

/** ==== CLOCK_W_ISFOCUS ===== */
const STATE_stateInnerTransitionTable_t STATE_CLOCK_W_ISFOCUS_Transitions[] = {
/*    Event                     ToState                Guard    Action                   */
    { EV_KEYRIGHT,              CLOCK_W_ISFOCUS,       0,       CLOCK__incrementWidget },
    { EV_KEYRIGHTLONGPRESS,     CLOCK_W_ISAUTOINCREMENT, 0,     0 },
    { EV_KEYLEFT,               CLOCK_W_ISFOCUS,        0,      CLOCK__setDisplayFonts }
};

/** ==== CLOCK_W_ISAUTOINCREMENT ===== */
const STATE_stateInnerTransitionTable_t STATE_CLOCK_W_ISAUTOINCREMENT_Transitions[] = {
/*    Event                     ToState                Guard    Action                   */
    { EV_KEYRIGHT,              CLOCK_W_ISFOCUS,        0,      CLOCK__incrementWidget },
    { EV_KEYLEFT,               CLOCK_W_ISFOCUS,        0,      CLOCK__setDisplayFonts }, // go back to focus if the container state chnages
    { EV_250MS,                 CLOCK_W_ISAUTOINCREMENT,0,      CLOCK__incrementWidget }
};

// =============== Outer Table ===============

const STATE_stateOuterTransitionTable_t CLOCK_Widget_Transitions[] = {
    { CLOCK_W_ISFOCUS,      STATE_CLOCK_W_ISFOCUS_Transitions, sizeof(STATE_CLOCK_W_ISFOCUS_Transitions) / sizeof(STATE_stateInnerTransitionTable_t) },
    { CLOCK_W_ISAUTOINCREMENT,   STATE_CLOCK_W_ISAUTOINCREMENT_Transitions, sizeof(STATE_CLOCK_W_ISAUTOINCREMENT_Transitions) / sizeof(STATE_stateInnerTransitionTable_t) }
};

const uint16_t CLOCK_Widget_Transitions_Size = sizeof(CLOCK_Widget_Transitions)/sizeof(STATE_stateOuterTransitionTable_t);

void WIDGET__processEvent(ClockWidgetInstance* widget, EVENT_enum_t event) {
    currentWidgetContext = widget;  // Kontext setzen

    for (uint8_t i = 0; i < CLOCK_Widget_Transitions_Size; ++i) {
        if (CLOCK_Widget_Transitions[i].currentState == widget->state) {
            const STATE_stateInnerTransitionTable_t* table = CLOCK_Widget_Transitions[i].transition;
            uint16_t size = CLOCK_Widget_Transitions[i].tableSize;

            for (uint16_t j = 0; j < size; ++j) {
                if (table[j].event == event && (!table[j].guard || table[j].guard())) {
                    if (table[j].action) {
                        table[j].action();  // Zugriff über globalen Kontext möglich
                    }
                    widget->state = table[j].nextState;
                    return;
                }
            }
        }
    }

    // reset the current context
    currentWidgetContext = NULL; 
}


void CLOCK__incrementWidget() {
    if (currentWidgetContext == NULL || currentWidgetContext->valuePtr == NULL)
        return;

    // Increase the value of the widget
    (*currentWidgetContext->valuePtr)++;
    
    //
    if (*currentWidgetContext->valuePtr >= currentWidgetContext->maxValue) {
        *currentWidgetContext->valuePtr = 0;
    }
}

void CLOCK__setDisplayFonts(){
     // Tracks if the edit mode is going to be exit
    static boolean_t leavingEditMode = FALSE; 
    
    // Retrieve current display configuration data
    SC_DISPLAYDATA_data_t data = RTE_SC_DISPLAYDATA_get(&SO_DISPLAYDATA_signal);  

    // If the currently focused widget is the hour field,
    if(currentWidgetContext->type == WIDGET_HOUR){
        // only highlight the hour field.
        data.invHours = TRUE;         
        data.invMinutes = FALSE;   
    }
    // If the focused widget is the minute field and the editing mode hasn't been left yet,
    else if((currentWidgetContext->type == WIDGET_MINUTE) && !leavingEditMode){
         // only highlight the minute field ...
        data.invHours = FALSE;      
        data.invMinutes = TRUE;     
        
        // Mark that the next EV_KEYLEFT will lead to leaving the edit mode
        leavingEditMode = TRUE;       
    }
    // All other cases (e.g., no widget focused or after leaving minute editing mode)
    else{
        // No speciifc highlighting
        data.invHours = FALSE;       
        data.invMinutes = FALSE;    
        
        // Reset the flag in order to re-enter the minute mode correctly
        leavingEditMode = FALSE;     
    }

    // Update the display signal with the modified highlight flags
    RTE_SC_DISPLAYDATA_set(&SO_DISPLAYDATA_signal, data);  
}

/* [] END OF FILE */
