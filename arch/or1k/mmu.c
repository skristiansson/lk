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
#include <trace.h>
#include <err.h>
#include <arch/mmu.h>
#include <arch/or1k/mmu.h>

#define LOCAL_TRACE 1

uint32_t or1k_kernel_translation_table[4096] __ALIGNED(8192) __SECTION(".bss.prebss.translation_table");

status_t arch_mmu_query(vaddr_t vaddr, paddr_t *paddr, uint *flags)
{
	LTRACEF("vaddr 0x%lx\n", vaddr);
	uint index = vaddr / SECTION_SIZE;

	uint32_t pte = or1k_kernel_translation_table[index];
	LTRACEF("pte 0x%lx\n", pte);
	if (paddr)
		*paddr = pte & ~OR1K_MMU_FLAGS_MASK | vaddr & (SECTION_SIZE-1);
	LTRACEF("paddr 0x%lx\n", *paddr);

	/* SJK FIXME */
	if (flags)
		*flags = ARCH_MMU_FLAG_PERM_USER;

	return NO_ERROR;
}

int arch_mmu_unmap(vaddr_t vaddr, uint count)
{
	TRACE;
}

int arch_mmu_map(vaddr_t vaddr, paddr_t paddr, uint count, uint flags)
{
	TRACE;
}
