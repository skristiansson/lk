/*
 * Copyright (c) 2015 Travis Geiselbrecht
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
#include <arch/or1k.h>
#include <arch/ops.h>
#include <arch/mmu.h>

#define LOCAL_TRACE 1

void arch_early_init(void)
{
	TRACE;
}

void arch_init(void)
{
	TRACE;
}

void arch_idle(void)
{
}

/* Read a physical address from the DTLB translate register.
 * Assumes that the address is already in the TLB
 * (i.e. that it has recently been accessed). */
status_t or1k_tophys(addr_t va, addr_t *pa)
{
	uint32_t dmmucfgr = mfspr(OR1K_SPR_SYS_DMMUCFGR_ADDR);
	uint32_t num_tlb_ways = OR1K_SPR_SYS_DMMUCFGR_NTW_GET(dmmucfgr) + 1;
	uint32_t num_tlb_sets = 1 << OR1K_SPR_SYS_DMMUCFGR_NTS_GET(dmmucfgr);
	uint32_t offset = (va >> PAGE_SIZE_SHIFT) & (num_tlb_sets-1);

	LTRACEF("va = 0x%x, offset = 0x%x\n", va, offset);
	for (uint i = 0; i < num_tlb_ways; i++) {
		uint32_t mr = mfspr_off(0, OR1K_SPR_DMMU_DTLBW_MR_ADDR(i, offset));
		LTRACEF("mr = 0x%x, va = 0x%x\n", mr, va);
		if (mr >> PAGE_SIZE_SHIFT == va >> PAGE_SIZE_SHIFT) {
			*pa = mfspr_off(0, OR1K_SPR_DMMU_DTLBW_TR_ADDR(i, offset)) & ~(PAGE_SIZE-1);
			return NO_ERROR;
		}
	}

	return ERR_NOT_FOUND;
}

void arch_chain_load(void *entry, ulong arg0, ulong arg1, ulong arg2, ulong arg3)
{
	PANIC_UNIMPLEMENTED;
}
