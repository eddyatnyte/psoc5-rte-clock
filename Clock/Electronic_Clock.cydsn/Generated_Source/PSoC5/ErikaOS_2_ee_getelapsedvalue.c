/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2011  Evidence Srl
 *
 * This file is part of ERIKA Enterprise.
 *
 * ERIKA Enterprise is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation, 
 * (with a special exception described below).
 *
 * Linking this code statically or dynamically with other modules is
 * making a combined work based on this code.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this code with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this code, you may extend
 * this exception to your version of the code, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 *
 * ERIKA Enterprise is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with ERIKA Enterprise; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 * ###*E*### */

/*
 * Author: 2011 Errico Guidieri
 */

/*
 * PSoC Port and API Generation
 * Carlos Fernando Meier Martinez
 * Hochschule Darmstadt, Germany. 2017.
 */

#include "ErikaOS_2_ee_internal.inc"

/***************************************************************************
 *   AS 4.0 OS SWS 8.4.18 GetElapsedValue
 ***************************************************************************/

#if EE_MAX_COUNTER
#ifndef __PRIVATE_GETELAPSEDVALUE__
//typedef struct {
//  TickType        maxallowedvalue;
//  TickType        ticksperbase;
//  TickType        mincycle;
//#ifdef EE_AS_OSAPPLICATIONS__
//  /** The ID of the application to which this counter belong to. */
//  ApplicationType ApplID;
//#endif /* EE_AS_OSAPPLICATIONS__ */
//} EE_oo_counter_ROM_type;
//    
//extern EE_oo_counter_ROM_type       EE_counter_ROM[EE_COUNTER_ROM_SIZE];
//
//typedef EE_TYPECOUNTEROBJECT  CounterObjectType;
//typedef struct {
//  TickType          value;  /**< current value of the counter */
//  CounterObjectType first;  /**< first object queued on the counter */
//} EE_oo_counter_RAM_type;
//
//extern EE_oo_counter_RAM_type       EE_counter_RAM[EE_MAX_COUNTER];

StatusType EE_oo_GetElapsedValue(CounterType CounterID, TickRefType Value,
    TickRefType ElapsedValue)
{
  register StatusType ev;
  /* Primitive Lock Procedure */
  EE_OS_DECLARE_AND_ENTER_CRITICAL_SECTION();

  EE_ORTI_set_service_in(EE_SERVICETRACE_GETELAPSEDVALUE);

  EE_as_monitoring_the_stack();

#ifdef EE_SERVICE_PROTECTION__
  /*  [OS093]: If interrupts are disabled/suspended by a Task/OsIsr and the
      Task/OsIsr calls any OS service (excluding the interrupt services)
      then the Operating System shall ignore the service AND shall return
      E_OS_DISABLEDINT if the service returns a StatusType value. */
  /*  [OS088]: If an OS-Application makes a service call from the wrong context
      AND is currently not inside a Category 1 ISR the Operating System module
      shall not perform the requested action (the service call shall have no
      effect), and return E_OS_CALLEVEL (see [12], section 13.1) or the
      invalid value of  the service. (BSW11009, BSW11013) */
  /* GetElapsedValue is callable by Task and ISR2 */
  if ( EE_as_execution_context > ISR2_Context ) {
    ev = E_OS_CALLEVEL;
  } else if ( EE_oo_check_disableint_error() ) {
    ev = E_OS_DISABLEDINT;
  } else
#endif /* EE_SERVICE_PROTECTION__ */

  if ( (Value == NULL) || (ElapsedValue == NULL) ) {
    ev = E_OS_PARAM_POINTER;
  } else
#if (defined(EE_AS_OSAPPLICATIONS__)) && (defined(EE_SERVICE_PROTECTION__)) \
  && (defined(__EE_MEMORY_PROTECTION__))
  /* [SWS_Os_00051]: If an invalid address (address is not writable by this
      OS-Application) is passed as an out-parameter to an Operating System
      service, the Operating System module shall return the status code
      E_OS_ILLEGAL_ADDRESS. (SRS_Os_11009, SRS_Os_11013) */
  if ( (!OSMEMORY_IS_WRITEABLE(EE_hal_get_app_mem_access(EE_as_active_app,
          Value, sizeof(*Value)))) || 
      (!OSMEMORY_IS_WRITEABLE( EE_hal_get_app_mem_access(EE_as_active_app,
          ElapsedValue, sizeof(*ElapsedValue)))) )
  {
    ev = E_OS_ILLEGAL_ADDRESS;  
  } else
#endif /* EE_AS_OSAPPLICATIONS__ && __EE_MEMORY_PROTECTION__ &&
  EE_SERVICE_PROTECTION__ */

#ifdef EE_AS_RPC__
  if ( EE_AS_ID_REMOTE(CounterID) )
  {
    EE_os_param as_value, as_elapsed_value;
    EE_os_param unmarked_alarm_id;
    unmarked_alarm_id.value_param = EE_AS_UNMARK_REMOTE_ID(CounterID);
    as_value.tick_ref             = Value;
    as_elapsed_value.tick_ref     = ElapsedValue;
    /* forward the request to another CPU in synchronous way */
    ev = EE_as_rpc(OSServiceId_GetElapsedValue, unmarked_alarm_id, as_value,
      as_elapsed_value);
  } else {
#endif /* EE_AS_RPC__ */

/* If counters are not defined cut everything */
#if (defined(EE_MAX_COUNTER)) && (EE_MAX_COUNTER > 0U)
    /* [OS381]: If the input parameter <CounterID> in a call of
        GetElapsedValue() is not Valid GetElapsedValue() shall return
        E_OS_ID. */
    if ( CounterID >= EE_MAX_COUNTER ) {
      ev = E_OS_ID;
    } else
#if (defined(EE_AS_OSAPPLICATIONS__)) && (defined(EE_SERVICE_PROTECTION__))
    if ( EE_COUNTER_ACCESS_ERR(CounterID, EE_as_active_app) ) {
      ev = E_OS_ACCESS;
    } else
#endif /* EE_AS_OSAPPLICATIONS__ || E_SERVICE_PROTECTION__ */

    if ( *Value > EE_counter_ROM[CounterID].maxallowedvalue ) {
      ev = E_OS_VALUE;
    } else {
      /* [OS382]: If the input parameters in a call of GetElapsedValue() are
          valid, GetElapsedValue() shall return the number of elapsed ticks
          since the given <Value> value via <ElapsedValue> and shall return
          E_OK.
        [OS533]: Caveats of GetCounterValue():If the timer already passed the
        <Value> value a second (or multiple) time, the result returned is wrong.
        The reason is that the service can not detect such a relative overflow.
      */
      *ElapsedValue = (EE_counter_RAM[CounterID].value >= *Value) ?
        /* Timer did not pass the <value> yet */
        (EE_counter_RAM[CounterID].value - *Value) :
        /* Timer already passed the <value> */
        ((EE_counter_ROM[CounterID].maxallowedvalue -
        (*Value - EE_counter_RAM[CounterID].value)) + 1U);

        ev = E_OK;
    }
#else   /* EE_MAX_COUNTER > 0 */
    {
      ev = E_OS_ID;
    }
#endif  /* EE_MAX_COUNTER > 0 */
#ifdef EE_AS_RPC__
  }
#endif /* EE_AS_RPC__ */

  if ( ev != E_OK ) {
    EE_ORTI_set_lasterror(ev);
    EE_oo_notify_error_GetElapsedValue(CounterID, Value, ElapsedValue,
      ev);
  }

  EE_ORTI_set_service_out(EE_SERVICETRACE_GETELAPSEDVALUE);
  EE_OS_EXIT_CRITICAL_SECTION();

  return ev;
}

#endif /* __PRIVATE_GETELAPSEDVALUE__ */
#endif
