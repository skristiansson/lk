/*
 * Copyright (c) 2015 Stefan Kristiansson
 * Based on arch/arm/arm/mmu.c
 * Copyright (c) 2008-2014 Travis Geiselbrecht
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
#include <string.h>
#include <arch/mmu.h>
#include <arch/or1k/mmu.h>
#include <kernel/vm.h>

#define LOCAL_TRACE 1

uint32_t or1k_kernel_translation_table[256] __ALIGNED(8192) __SECTION(".bss.prebss.translation_table");

status_t arch_mmu_query(vaddr_t vaddr, paddr_t *paddr, uint *flags)
{
	uint index = vaddr / SECTION_SIZE;
	uint32_t pte = or1k_kernel_translation_table[index];

	if (!(pte & OR1K_MMU_PG_PRESENT))
		return ERR_NOT_FOUND;

	/* not a l1 entry */
	if (!(pte & OR1K_MMU_PG_L)) {
		uint32_t *l2_table = paddr_to_kvaddr(pte & ~OR1K_MMU_PG_FLAGS_MASK);
		index = (vaddr % SECTION_SIZE) / PAGE_SIZE;
		pte = l2_table[index];
		LTRACEF("pte = 0x%0x\n", pte);
	}

	if (paddr)
		*paddr = (pte & ~OR1K_MMU_PG_FLAGS_MASK) | (vaddr & (SECTION_SIZE-1));

	if (flags) {
		*flags = 0;
		if (pte & OR1K_MMU_PG_U)
			*flags |= ARCH_MMU_FLAG_PERM_USER;
		if (!(pte & OR1K_MMU_PG_X))
			*flags |= ARCH_MMU_FLAG_PERM_NO_EXECUTE;
		if (!(pte & OR1K_MMU_PG_W))
			*flags |= ARCH_MMU_FLAG_PERM_RO;
		if (pte & OR1K_MMU_PG_CI)
			*flags |= ARCH_MMU_FLAG_UNCACHED;
	}

	return NO_ERROR;
}

int arch_mmu_unmap(vaddr_t vaddr, uint count)
{
	PANIC_UNIMPLEMENTED;
}

int arch_mmu_map(vaddr_t vaddr, paddr_t paddr, uint count, uint flags)
{
	uint l1_index;
	uint32_t pte;
	uint32_t arch_flags = 0;

	LTRACEF("vaddr = 0x%x, paddr = 0x%x, count = %d, flags = 0x%x\n", vaddr, paddr, count, flags);

	if (!IS_PAGE_ALIGNED(vaddr) || !IS_PAGE_ALIGNED(paddr))
		return ERR_INVALID_ARGS;

	if (flags & ARCH_MMU_FLAG_PERM_USER)
		arch_flags |= OR1K_MMU_PG_U;
	if (!(flags & ARCH_MMU_FLAG_PERM_NO_EXECUTE))
		arch_flags |= OR1K_MMU_PG_X;
	if (flags & ARCH_MMU_FLAG_CACHE_MASK)
		arch_flags |= OR1K_MMU_PG_CI;
	if (!(flags & ARCH_MMU_FLAG_PERM_RO))
		arch_flags |= OR1K_MMU_PG_W;

	uint mapped = 0;
	while (count) {
		l1_index = vaddr / SECTION_SIZE;
		if (IS_ALIGNED(vaddr, SECTION_SIZE) && IS_ALIGNED(paddr, SECTION_SIZE) && count >= SECTION_SIZE / PAGE_SIZE) {
			or1k_kernel_translation_table[l1_index] = (paddr & ~(SECTION_SIZE-1)) | arch_flags | OR1K_MMU_PG_PRESENT | OR1K_MMU_PG_L;
			count -= SECTION_SIZE / PAGE_SIZE;
			mapped += SECTION_SIZE / PAGE_SIZE;
			vaddr += SECTION_SIZE;
			paddr += SECTION_SIZE;
		} else {
			pte = or1k_kernel_translation_table[l1_index];
			LTRACEF("pte 0x%x\n", pte);

			/* FIXME: l1 already mapped as a section */
			if (pte & OR1K_MMU_PG_PRESENT && pte & OR1K_MMU_PG_L)
				PANIC_UNIMPLEMENTED;

			if (pte & OR1K_MMU_PG_PRESENT)
				PANIC_UNIMPLEMENTED; /*SJK FIXME*/

			uint32_t *l2_table = pmm_alloc_kpage();
			if (!l2_table) {
				TRACEF("failed to allocate pagetable\n");
				return mapped;
			}

			memset(l2_table, 0, PAGE_SIZE);
			paddr_t l2_pa = 0;
			if (or1k_tophys((vaddr_t)l2_table, &l2_pa) == ERR_NOT_FOUND)
				panic("failed to get phys addr of l2_table\n");

			LTRACEF("allocated pagetable at %p, pa 0x%lx\n", l2_table, l2_pa);

			or1k_kernel_translation_table[l1_index] = l2_pa | arch_flags | OR1K_MMU_PG_PRESENT;

			uint l2_index = (vaddr % SECTION_SIZE) / PAGE_SIZE;

			LTRACEF("l2_index = 0x%x, vaddr = 0x%x, paddr = 0x%x\n", l2_index, vaddr, paddr);
			l2_table[l2_index] = paddr | arch_flags | OR1K_MMU_PG_PRESENT | OR1K_MMU_PG_L;

			count--;
			mapped++;
			vaddr += PAGE_SIZE;
			paddr += PAGE_SIZE;
		}
	}

	return mapped;
}
