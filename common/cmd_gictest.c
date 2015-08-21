/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2015 by Tom Ting <tom_ting@realtek.com>
 *
 * Time initialization.
 */

#include <common.h>
#include <command.h>
#include <asm/arch/interrupt.h>

static int do_gictest(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	unsigned sgi=0, mask=0;

	gic_test_init();
	rtk_local_irq_enable();

	switch (argc) {
		case 1:
			cpu_sgi_test(sgi, mask);
			break;
		case 2:
			sgi = simple_strtoul(argv[1], NULL, 10);
			cpu_sgi_test(sgi, mask);
		case 3:
		default:
			printf("Not support yet\n");
			return 1;
			break;
	}
	return 0;
}

U_BOOT_CMD(
	gictest, 3, 1, do_gictest,
	"Turn on GIC and send SGI to CPU",
	"SGI# [CPUMASK]"
);
