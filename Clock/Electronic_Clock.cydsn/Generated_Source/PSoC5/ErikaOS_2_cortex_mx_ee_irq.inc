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

 /** 
	
	@file ee_irq.h
	@brief Prestub and postub macros and related stuffs
	@author Gianluca Franchino
	@author Giuseppe Serano
	@date 2011
*/ 
    
/*
 * PSoC Port and API Generation
 * Carlos Fernando Meier Martinez
 * Hochschule Darmstadt, Germany. 2017.
 */

#ifndef __INCLUDE_CORTEX_MX_IRQ_H__
#define __INCLUDE_CORTEX_MX_IRQ_H__


#ifdef	__CORTEX_MX__
#define EE_ISR_UNMASKED 0x00000000
#define EE_ISR_PRI_1    0x00000010U
#define EE_ISR_PRI_2    0x0000000FU
#define EE_ISR_PRI_3    0x0000000EU
#define EE_ISR_PRI_4    0x0000000DU
#define EE_ISR_PRI_5    0x0000000CU
#define EE_ISR_PRI_6    0x0000000BU
#define EE_ISR_PRI_7    0x0000000AU
#define EE_ISR_PRI_8    0x00000009U
#define EE_ISR_PRI_9    0x00000008U
#define EE_ISR_PRI_10   0x00000007U
#define EE_ISR_PRI_11   0x00000006U
#define EE_ISR_PRI_12   0x00000005U
#define EE_ISR_PRI_13   0x00000004U
#define EE_ISR_PRI_14   0x00000003U
#define EE_ISR_PRI_15   0x00000002U
#define EE_ISR_PRI_16   0x00000001U
#endif	/* __CORTEX_M4__ */

#define EE_std_change_context(x) ((void)0)

/* Use angled parenthesis to include the main "ee_internal.inc" */
#include "ErikaOS_2_cortex_mx_ee_cpu.inc"
#include "ErikaOS_2_cortex_mx_ee_irq_cng_cont.inc"
#include "ErikaOS_2_cortex_mx_ee_context.inc"
#include "ErikaOS_2_common_ee_irqstub.inc"

#ifdef __ALLOW_NESTED_IRQ__

extern struct EE_TOS EE_cortex_mx_IRQ_tos;

#define EE_std_enableIRQ_nested() EE_cortex_mx_enableIRQ()
#define EE_std_disableIRQ_nested() EE_cortex_mx_disableIRQ()

#else	/* else __ALLOW_NESTED_IRQ__*/

#define EE_std_enableIRQ_nested() ((void)0)
#define EE_std_disableIRQ_nested() ((void)0)

#endif /* end __ALLOW_NESTED_IRQ__*/

#if defined(__MULTI__) && defined(__IRQ_STACK_NEEDED__)

extern void EE_cortex_mx_change_IRQ_stack(void);
extern void EE_cortex_mx_change_IRQ_stack_back(void);

/*save the stack pointer*/ /*Load new stack pointer*/
#define EE_cortex_mx_change_stack()\
do {\
	if(EE_IRQ_nesting_level==1) {\
	EE_cortex_mx_change_IRQ_stack();\
	}\
} while(0)

#define EE_cortex_mx_stack_back()\
	EE_cortex_mx_change_IRQ_stack_back()

#else	/* else __MULTI__ && __IRQ_STACK_NEEDED__*/

#define EE_cortex_mx_change_stack() ((void)0)
#define EE_cortex_mx_stack_back() ((void)0)

#endif	/* end __MULTI__ && __IRQ_STACK_NEEDED__*/
 
#define EE_ISR2_prestub(void)\
/* Defined as Macro */\
do {\
	EE_cortex_mx_disableIRQ();\
	ipl = EE_cortex_mx_get_int_prio();\
	EE_cortex_mx_set_int_prio(EE_cortex_mx_get_isr_prio());\
	EE_increment_IRQ_nesting_level();\
	EE_cortex_mx_change_stack();\
	/* Enable IRQ if nesting  is allowed */\
	EE_std_enableIRQ_nested();\
}\
while(0)

extern EE_UREG	EE_cortex_mx_change_context_active;

#define EE_ISR2_poststub(void)\
/* Defined as Macro */\
do{\
/* Disabled IRQ if nesting is allowed.\
 * Note: if nesting is not allowed, the IRQs are already disabled\
 */\
    EE_std_disableIRQ_nested();\
    EE_cortex_mx_set_int_prio(ipl);\
    EE_std_end_IRQ_post_stub();\
    EE_decrement_IRQ_nesting_level();\
/*\
* If the ISR at the lowest level is ended, restore the stack pointer\
* and active the change context procedure if needed ( call the scheduler).\
*/\
    if (!EE_is_inside_ISR_call()) {\
	EE_cortex_mx_stack_back();\
	EE_cortex_mx_IRQ_active_change_context();\
    }\
    EE_cortex_mx_enableIRQ();\
}\
while(0)



#define ISR1(f)			\
void ISR1_ ## f(void);		\
__IRQ void f(void) \
{				\
	ISR1_ ## f();		\
}				\
void ISR1_ ## f(void)


#define ISR2(f)			\
void ISR2_ ## f(void);		\
__IRQ void f(void) \
{				\
	EE_UREG ipl = 0;	\
	EE_ISR2_prestub();	\
	ISR2_ ## f();		\
	EE_ISR2_poststub();	\
}				\
void ISR2_ ## f(void)

/* Standard Macro to declare an ISR (2) */
#define ISR(f) ISR2(f)

#endif /* __INCLUDE_CORTEX_MX_IRQ_H__ */
