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
#pragma once

#include <compiler.h>
#include <arch/or1k.h>

#ifndef ASSEMBLY
static inline void arch_enable_ints(void)
{
	mtspr(OR1K_SPR_SYS_SR_ADDR,
	      mfspr(OR1K_SPR_SYS_SR_ADDR) |
	      OR1K_SPR_SYS_SR_IEE_MASK |
	      OR1K_SPR_SYS_SR_TEE_MASK);
}

static inline void arch_disable_ints(void)
{
	mtspr(OR1K_SPR_SYS_SR_ADDR,
	      mfspr(OR1K_SPR_SYS_SR_ADDR) &
	      ~(OR1K_SPR_SYS_SR_IEE_MASK |
		OR1K_SPR_SYS_SR_TEE_MASK));
}

static inline int atomic_add(volatile int *ptr, int val)
{
	return __atomic_fetch_add(ptr, val, __ATOMIC_RELAXED);
}

static inline int atomic_or(volatile int *ptr, int val)
{
	return __atomic_fetch_or(ptr, val, __ATOMIC_RELAXED);
}

static inline int atomic_and(volatile int *ptr, int val)
{
	return __atomic_fetch_and(ptr, val, __ATOMIC_RELAXED);
}

static inline int atomic_swap(volatile int *ptr, int val)
{
	return __atomic_exchange_n(ptr, val, __ATOMIC_RELAXED);
}

#if 0
static inline int atomic_cmpxchg(volatile int *ptr, int oldval, int newval)
{
	int old;
	int test;

	do {
		__asm__ volatile(
		    "ldrex	%[old], [%[ptr]]\n"
		    "mov	%[test], #0\n"
		    "teq	%[old], %[oldval]\n"
#if ARM_ISA_ARMV7M
		    "bne	0f\n"
		    "strex	%[test], %[newval], [%[ptr]]\n"
		    "0:\n"
#else
		    "strexeq %[test], %[newval], [%[ptr]]\n"
#endif
		    : [old]"=&r" (old), [test]"=&r" (test)
		    : [ptr]"r" (ptr), [oldval]"Ir" (oldval), [newval]"r" (newval)
		    : "cc");

	} while (test != 0);

	return old;
}
#endif

/* use a global pointer to store the current_thread */
extern struct thread *_current_thread;

static inline struct thread *get_current_thread(void)
{
	return _current_thread;
}

static inline void set_current_thread(struct thread *t)
{
	_current_thread = t;
}

static inline uint32_t arch_cycle_count(void) { return 0; }
#endif // !ASSEMBLY
