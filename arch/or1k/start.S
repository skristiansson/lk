/*
 * Copyright (c) 2015 Stefan Kristiansson
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <asm.h>
#include <arch/ops.h>

#define RED_ZONE		128
#define EXCEPTION_FRAME		(124 + RED_ZONE)

#define SAVE_REGS \
	l.addi	r1, r1, -EXCEPTION_FRAME	;\
	l.sw	0(r1), r2			;\
	l.sw	4(r1), r3			;\
	l.sw	8(r1), r4			;\
	l.sw	12(r1), r5			;\
	l.sw	16(r1), r6			;\
	l.sw	20(r1), r7			;\
	l.sw	24(r1), r8			;\
	l.sw	28(r1), r9			;\
	l.sw	32(r1), r10			;\
	l.sw	36(r1), r11			;\
	l.sw	40(r1), r12			;\
	l.sw	44(r1), r13			;\
	l.sw	48(r1), r14			;\
	l.sw	52(r1), r15			;\
	l.sw	56(r1), r16			;\
	l.sw	60(r1), r17			;\
	l.sw	64(r1), r18			;\
	l.sw	68(r1), r19			;\
	l.sw	72(r1), r20			;\
	l.sw	76(r1), r21			;\
	l.sw	80(r1), r22			;\
	l.sw	84(r1), r23			;\
	l.sw	88(r1), r24			;\
	l.sw	92(r1), r25			;\
	l.sw	96(r1), r26			;\
	l.sw	100(r1), r27			;\
	l.sw	104(r1), r28			;\
	l.sw	108(r1), r29			;\
	l.sw	112(r1), r30			;\
	l.sw	116(r1), r31			;\
	l.mfspr	r3, r0, OR1K_SPR_SYS_EPCR_BASE	;\
	l.sw	120(r1), r3			;\
	l.mfspr	r3, r0, OR1K_SPR_SYS_ESR_BASE	;\
	l.sw	124(r1), r3

.section ".vectors", "ax"
.org 0x100
.global _reset
_reset:
	l.jal	start
	 l.nop

.org 0x200
	l.jal	unhandled_exception
	 l.nop
.org 0x300
	l.jal	unhandled_exception
	 l.nop
.org 0x400
	l.jal	unhandled_exception
	 l.nop
.org 0x500
	SAVE_REGS
	l.jal	or1k_tick
	 l.nop
	l.j	return_from_exception
	 l.nop
.org 0x600
	l.jal	unhandled_exception
	 l.nop
.org 0x700
	l.jal	unhandled_exception
	 l.nop
.org 0x800
	SAVE_REGS
	l.jal	or1k_irq
	 l.nop
	l.j	return_from_exception
	 l.nop
.org 0x900
	l.jal	unhandled_exception
	 l.nop
.org 0xa00
	l.jal	unhandled_exception
	 l.nop
.org 0xb00
	l.jal	unhandled_exception
	 l.nop
.org 0xc00
	l.jal	unhandled_exception
	 l.nop
.org 0xd00
	l.jal	unhandled_exception
	 l.nop
.org 0xe00
	l.jal	unhandled_exception
	 l.nop

.section ".text.boot"
FUNCTION(start)
	# set stack pointer to point at top of default stack
	l.movhi	r1, hi(default_stack_top)
	l.ori	r1, r1, lo(default_stack_top)

	# invalidate and enable caches
	l.jal	arch_invalidate_cache_all
	 l.nop
	l.jal	arch_enable_cache
	 l.ori	r3, r0, UCACHE

	# clear bss
	l.movhi	r3, hi(__bss_start)
	l.ori	r3, r3, lo(__bss_start)
	l.movhi	r4, hi(__bss_end)
	l.ori	r4, r4, lo(__bss_end)
1:	l.sw	0(r3), r0
	l.sfltu	r3, r4
	l.bf	1b
	 l.addi	r3, r3, 4

	# arguments to main
	l.ori	r3, r0, 1
	l.ori	r4, r0, 2
	l.ori	r5, r0, 3
	l.jal	lk_main
	 l.ori	r6, r0, 4

	# shouldn't happen, but loop if it does
	l.j	0
	 l.nop

FUNCTION(return_from_exception)
	l.lwz	r3, 120(r1)
	l.mtspr	r0, r3, OR1K_SPR_SYS_EPCR_BASE
	l.lwz	r3, 124(r1)
	l.mtspr	r0, r3, OR1K_SPR_SYS_ESR_BASE
	l.lwz	r2, 0(r1)
	l.lwz	r3, 4(r1)
	l.lwz	r4, 8(r1)
	l.lwz	r5, 12(r1)
	l.lwz	r6, 16(r1)
	l.lwz	r7, 20(r1)
	l.lwz	r8, 24(r1)
	l.lwz	r9, 28(r1)
	l.lwz	r10, 32(r1)
	l.lwz	r11, 36(r1)
	l.lwz	r12, 40(r1)
	l.lwz	r13, 44(r1)
	l.lwz	r14, 48(r1)
	l.lwz	r15, 52(r1)
	l.lwz	r16, 56(r1)
	l.lwz	r17, 60(r1)
	l.lwz	r18, 64(r1)
	l.lwz	r19, 68(r1)
	l.lwz	r20, 72(r1)
	l.lwz	r21, 76(r1)
	l.lwz	r22, 80(r1)
	l.lwz	r23, 84(r1)
	l.lwz	r24, 88(r1)
	l.lwz	r25, 92(r1)
	l.lwz	r26, 96(r1)
	l.lwz	r27, 100(r1)
	l.lwz	r28, 104(r1)
	l.lwz	r29, 108(r1)
	l.lwz	r30, 112(r1)
	l.lwz	r31, 116(r1)
	l.addi	r1, r1, EXCEPTION_FRAME
	l.rfe

FUNCTION(unhandled_exception)
	# report epcr
	l.mfspr	r3, r0, OR1K_SPR_SYS_EPCR_BASE
	l.nop	0x2
	# report vector
	l.ori	r3, r9, 0
	l.nop	0x2
	l.nop	0x1
	l.j	0
	 l.nop

.section ".bss"
.align 8
LOCAL_DATA(default_stack)
	.skip 8192
LOCAL_DATA(default_stack_top)
