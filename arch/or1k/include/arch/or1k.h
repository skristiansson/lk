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

#include <arch/or1k-sprs.h>

#define mtspr(_spr, _val)				\
	__asm__ __volatile__(				\
		"l.mtspr r0, %1, %0"			\
		:					\
		: "K" (_spr), "r" (_val)		\
	)

#define mtspr_off(_spr, _off, _val)			\
	__asm__ __volatile__ (				\
		"l.mtspr %0,%1,%2"			\
		:					\
		: "r" (_off), "r" (_val), "K" (_spr)	\
	)

#define mfspr(_spr)					\
({							\
	uint32_t _val;					\
	__asm__ __volatile__(				\
		"l.mfspr %0, r0, %1"			\
		: "=r"(_val)				\
		: "K" (_spr)				\
		);					\
	_val;						\
})

#define mfspr_off(_spr, _off)				\
({							\
	uint32_t _val;					\
	__asm__ __volatile__ (				\
		"l.mfspr %0,%1,%2"			\
		: "=r" (_val)				\
		: "r" (_off), "K" (_spr)		\
		);					\
	_val;						\
})
