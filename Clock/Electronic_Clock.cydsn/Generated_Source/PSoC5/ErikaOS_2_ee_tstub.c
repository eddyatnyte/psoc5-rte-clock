/* ###*B*###
 * ERIKA Enterprise - a tiny RTOS for small microcontrollers
 *
 * Copyright (C) 2002-2008  Evidence Srl
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
 * Author: 2002-2004 Paolo Gai
 * CVS: $Id: ee_tstub.c,v 1.1 2005/07/16 12:23:42 pj Exp $
 */

/*
 * PSoC Port and API Generation
 * Carlos Fernando Meier Martinez
 * Hochschule Darmstadt, Germany. 2017.
 */
#include "ErikaOS_2_ee_tracing.h"

#include "ErikaOS_2_ee_internal.inc"

/* TerminateStub:
   - This is the function that is called as the "real thread body" of 
     an OO task.
   - This function is not called if a task use only a terminate_task at 
     the end of the thread. In some architectures, this can lead 
     to some stack savings...
*/

#if (__OO_configUSE_TRACE_FACILITY == 1)
#include "trcKernelPort.h"
#endif

#ifndef __PRIVATE_THREADSTUB__
void EE_oo_thread_stub(void)
{
  register EE_TID temp;

#if (__OO_configUSE_TRACE_FACILITY == 1)
  traceMOVED_TASK_TO_READY_STATE(EE_stk_queryfirst());
  traceTASK_SWITCHED_IN(EE_stk_queryfirst(), EE_th_ready_prio[EE_stk_queryfirst()]);
#endif

  EE_oo_call_PreTaskHook();

  temp = EE_stk_queryfirst();

  EE_hal_terminate_savestk(temp);

  /* NOTE:
     It is not specified if at this point the system runs or not 
     with interrupts disabled.
  */
}
#endif /* __PRIVATE_THREADSTUB__ */
