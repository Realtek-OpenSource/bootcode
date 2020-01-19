#include "bus_traffic_generator.h"

#ifndef NULL
#define NULL 0
#endif

#define MAX_ARGS        20
#define MAX_ARG_LEN     20

/* Provided outside. */
#define BTGLX0_BASE   (0xB8144000)
#define BTGLX1_BASE   (0xB800A000)
#define BTGLX2_BASE   (0xB8018000)
#define BTGLXUW_BASE  (0xB8148000)
#define BTGLXUR_BASE  (0xB8149000)
#define BTGLXDW_BASE  (0xB814A000)
#define BTGLXDR_BASE  (0xB814B000)


btg_para_v3_t btg[] = { {
		.gbase = BTGLX0_BASE + 0x100,
		.lxid  = 0,
		.feat  = BTG_WRITE,
	}, {
		.gbase = BTGLX0_BASE + 0x200,
		.lxid  = 0,
		.feat  = BTG_READ,
	}, {
		.gbase = BTGLX1_BASE + 0x100,
		.lxid  = 1,
		.feat  = BTG_WRITE,
	}, {
		.gbase = BTGLX1_BASE + 0x200,
		.lxid  = 1,
		.feat  = BTG_READ,
	}, {
		.gbase = BTGLX2_BASE + 0x100,
		.lxid  = 2,
		.feat  = BTG_WRITE,
	}, {
		.gbase = BTGLX2_BASE + 0x200,
		.lxid  = 2,
		.feat  = BTG_READ,
	}, {
        .gbase = BTGLXUW_BASE + 0x100,
        .lxid  = 3,
        .feat  = BTG_WRITE,
    }, {
        .gbase = BTGLXUR_BASE + 0x200,
        .lxid  = 4,
        .feat  = BTG_READ,
    }, {
        .gbase = BTGLXDW_BASE + 0x100,
        .lxid  = 5,
        .feat  = BTG_WRITE,
    }, {
        .gbase = BTGLXDR_BASE + 0x200,
        .lxid  = 6,
        .feat  = BTG_READ,
    }, {
		/* Sentinal entry guards end of list. */
		.gbase = 0xffff,
	}
};

void btg_chk_reset(void);
void btg_chk_basic_func(void);
void btg_chk_timing_debug_reg(void);
void btg_chk_read_dgen_debug_reg(void);
void btg_chk_read_csum_debug_reg(void);
void btg_chk_concurrent_rw(void);
void btg_chk_experiment_single(void);

inline static uint32_t gdma_get_base(btg_para_v3_t *req) {
	return (req->gbase & 0xfffff000);
}

inline static uint32_t btg_get_base(btg_para_v3_t *req) {
	return req->gbase;
}

void _btg_setup(btg_para_v3_t *req) {
	uint32_t infinite = 0;
	uint32_t pktlen = req->pktlen;
	uint32_t base_addr = 0;

	if (pktlen > 8160) {
		printf("WW: packet length is capped to 8160B\n");
		pktlen = 8160;
		req->pktlen = pktlen;
	}

	/* if (pktlen & 0x1F) { */
	/* 	printf("WW: packet length is changed to 32B-alignment\n"); */
	/* 	pktlen &= (~(0x1F)); */
		req->pktlen = pktlen;
	/* } */

	if (req->iter == 0) {
		infinite = 1;
	}

	BTGREG(req, BTGCRo)  = ((req->rchkmode << 24) | (req->rec_latest << 22) |
	                        (req->precise  << 21) | (infinite << 20)        |
	                        (req->burstlen << 16) | pktlen);
	BTGREG(req, BTGIRo)  = req->iter;
	BTGREG(req, BTGRTRo) = req->resp_time;
	BTGREG(req, BTGPRo)  = req->perid_time;
	
	switch ((req->addr_base) & 0xF0000000) {
		case 0x80000000:
		case 0xa0000000:
			base_addr = req->addr_base & (~(0xF0000000)); /* get physical address. */
			break;
			
		case 0xb0000000:
		case 0x90000000:
		case 0x10000000:
			base_addr = (req->addr_base & (~(0xF0000000))) | 0x10000000; /* get physical address. */
			break;
			
		default:
			printf("WW: invalid base address may be set\n");
			base_addr = req->addr_base & (~(0xF0000000)); /* get physical address. */
	}
	BTGREG(req, BTGBARo) = base_addr;
	BTGREG(req, BTGAMRo) = req->addr_mask;

	if (req->addr_gap < 0) {
		req->addr_gap *= -1;
		BTGREG(req, BTGGRo) = 0x80000000 | req->addr_gap;
	} else  {
		BTGREG(req, BTGGRo) = req->addr_gap;
	}

	/* Input Vector setup */
	BTGIVEC(req, 0)  = 0xa5a55a5a;
	BTGIVEC(req, 1)  = 0xffff0000;
	BTGIVEC(req, 2)  = 0x0000ffff;
	BTGIVEC(req, 3)  = 0xff00ff00;
	BTGIVEC(req, 4)  = 0x00ff00ff;
	BTGIVEC(req, 5)  = 0x5a5aa5a5;
	BTGIVEC(req, 6)  = 0x01234567;
	BTGIVEC(req, 7)  = 0x89abcdef;
	BTGIVEC(req, 8)  = 0xaaaa5555;
	BTGIVEC(req, 9)  = 0x5555aaaa;
	BTGIVEC(req, 10) = 0xa5a55a5a;
	BTGIVEC(req, 11) = 0xffff0000;
	BTGIVEC(req, 12) = 0x0000ffff;
	BTGIVEC(req, 13) = 0xff00ff00;
	BTGIVEC(req, 14) = 0x00ff00ff;
	BTGIVEC(req, 15) = 0x5a5aa5a5;

	return;
}

#define ISCMD(x)    (strcmp(x, argv[1]) == 0)
#define ISSUBCMD(x) (strcmp(x, argv[2]) == 0)

#define ISARG1(x) (strcmp(x, argv[1]) == 0)
#define ISARG2(x) (strcmp(x, argv[2]) == 0)
#define ISARG3(x) (strcmp(x, argv[3]) == 0)

#define ISBTGAVAL(x) ((x->gbase & 0xb8000000) == 0xb8000000)
#define ISBTGEND(x)  (x->gbase == 0xffff)

#define SETIFVAR(x) do {            \
		if (strcmp(#x, argv[4]) == 0) { \
			btg_entry->x = str2UL(argv[5]); \
			return 0;                     \
		}                               \
	} while(0)

#define SETIFVARH(x) do {           \
		if (strcmp(#x, argv[4]) == 0) { \
			btg_entry->x = str2UL(argv[5]); \
			return 0;                     \
		}                               \
	} while(0)

//static void _btg_start(btg_para_v3_t *req) {
void _btg_start(btg_para_v3_t *req) {
	volatile uint32_t *region;

	printf("II: BTG-LX%d(%s) ",
	       req->lxid,
	       (req->feat == BTG_WRITE) ? "WR" : "RD");

	if (!ISBTGAVAL(req)) {
		printf("is NOT avaiable\n");
	} else {
		printf("@ %08x: %08x ~ %08x... ",
		       req->gbase, req->addr_base, (req->addr_base | req->addr_mask));

		if (req->feat == BTG_WRITE) {
			region = (uint32_t *)(req->addr_base | 0xa0000000);
			while ((uint32_t)region < (req->addr_base | req->addr_mask | 0xa0000000)) {
				*region++ = 0x77777777;
			}
		}

		BTGREG(req, BTGCRo) |= 0x80000000;
		printf("enabled\n");
	}

	return;
}

//static void _btg_stop(btg_para_v3_t *req) {
void _btg_stop(btg_para_v3_t *req) {
    printf("II: BTG-LX%d(%s) ",
	       req->lxid,
	       (req->feat == BTG_WRITE) ? "WR" : "RD");

	if (!ISBTGAVAL(req)) {
		printf("is NOT avaiable\n");
	} else {
		printf("@ %08x: %08x ~ %08x... ",
		       req->gbase, req->addr_base, (req->addr_base | req->addr_mask));
		BTGREG(req, BTGCRo) &= 0x7FFFFFFF;
		printf("stopped\n");
	}

	return;
}

static void _btg_show_reg(btg_para_v3_t *req) {
	volatile uint32_t *btgreg = (volatile uint32_t *)req->gbase;

	uint32_t i = 0;

	if (!ISBTGAVAL(req)) {
		printf("II: BTG-LX%d(%s) is NOT available\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD");
	} else {
		printf("II: BTG-LX%d(%s) @ %08x:\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD",
		       req->gbase);
		while (i < 28) {
			printf("II:   %p: %08x %08x %08x %08x\n",
			       btgreg,
			       *(btgreg+0), *(btgreg+1), *(btgreg+2), *(btgreg+3));
			btgreg += 4;
			i += 4;
		}
	}
	return;
}

static void _btg_para_decode(btg_para_v3_t *req) {
	uint32_t isr, isr_offset;

	//printf("entering %s\n\r", __func__);
	
	if (!req) printf("btg_para_v3_t *req is NULL\n\r");
	
	if (!ISBTGAVAL(req)) {
		printf("II: BTG-LX%d(%s) is NOT available\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD");
	} else {
		isr = GDMAREG(req, GDMAISRo);
		isr_offset = (req->feat == BTG_WRITE) ? 0 : 4;

		printf("II: BTG-LX%d(%s) @ %08x Configuration:\n",
		       req->lxid,
		       (req->feat == BTG_WRITE) ? "WR" : "RD",
		       req->gbase);
		printf("II:   rec_latest: %d\tprecise: %d\n", req->rec_latest, req->precise);
		printf("II:   rchkmode:  %d\tburstlen: %d B\n", req->rchkmode, 1 << (4+req->burstlen));
		printf("II:   pktlen: %d B\titer: %d\n", req->pktlen, req->iter);
		printf("II:   resp_time: %d C\tperid_time: %d C\n", req->resp_time, req->perid_time);
		printf("II:   addr_base: %08x\taddr_mask: %08x\n", req->addr_base, req->addr_mask);
		printf("II:   addr_gap: %08x\n", req->addr_gap);
		printf("II:   covered range: %08x ~ %08x\n",
		       req->addr_base, (req->addr_base | req->addr_mask));
		printf("II: Status:\n");
		printf("II:   Int. Status Reg.: %08x\n", isr);
		printf("II:   In progress? %c\n", (BTGREG(req, BTGCRo) & 0x80000000) ? 'Y' : 'N');
		printf("II:   Int. %d, Addr. fails response time: %08x\n",
		       (isr >> (20 + isr_offset)) & 0x1, BTGREG(req, BTGRFARo));
		printf("II:   Int. %d, Addr. fails period:        %08x\n",
		       (isr >> (21 + isr_offset)) & 0x1, BTGREG(req, BTGPFARo));
		if (req->feat == BTG_READ) {
			printf("II:   Int. %d, Addr. fails verification:  %08x\n",
			       (isr >> 26) & 0x1, BTGREG(req, BTGVFARo));
		}
		printf("II:   Max response time: %d(0x%08x) cycles\n",
		       BTGREG(req, BTGMRTRo), BTGREG(req, BTGMRTRo));
	}
	return;
}

void static inline _btg_reset(void) {
	btg_para_v3_t *btg_entry = &btg[0];

	printf("II: Resetting ");
	while (btg_entry->gbase != 0xffff) {
		printf("BTG-LX%d... ", btg_entry->lxid);
		if ((btg_entry->gbase & 0xfffff000) == 0) {
			btg_entry += 2;
			printf("N/A; ");
			continue;
		}

		/* Reset GDMA and BTG */
		GDMAREG(btg_entry, GDMACRo) = 0x00000000;
		GDMAREG(btg_entry, GDMACRo) = 0x10000000;
		GDMAREG(btg_entry, GDMACRo) = 0x90000000;

		/* Reset Interrupt bits */
		GDMAREG(btg_entry, GDMAIMRo) = 0x07300000;
		GDMAREG(btg_entry, GDMAISRo) = 0x07300000;
		GDMAREG(btg_entry, GDMAISRo) = 0x00000000;

		BTGREG(btg_entry, BTGCRo) = 0;
        if(btg_entry->lxid >=3){
    		printf("done; ");
            btg_entry++;
    		printf("BTG-LX%d... ", btg_entry->lxid);
        }else{
    		btg_entry++;
        }
		BTGREG(btg_entry, BTGCRo) = 0;
		printf("done; ");
		btg_entry++;
	}

	printf("\n");

	return;
}

#if 1
char btg_prompt_string[] = {
	"btg [lx0|lx1|lx2] [w|r] [start|stop|info|reg]\n\rbtg [lx0|lx1|lx2] [w|r] set [iarg] [val]\n\r"
};
#else
char btg_prompt_string[] = {
	"all [reset|start|stop|info|reg]\n"
	"btg all set [garg] [val]\n"
	"btg [lx0|lx1|lx2] [w|r] [start|stop|info|reg]\n"
	"btg [lx0|lx1|lx2] [w|r] set [iarg] [val]\n"
	"btg [lx3|lx5] [w] [start|stop|info|reg]\n"
        "btg [lx3|lx5] [w] set [iarg] [val]\n"
        "btg [lx4|lx6] [r] [start|stop|info|reg]\n"
        "btg [lx4|lx6] [r] set [iarg] [val]\n"
        "  supported iarg:\n"
	"    rec_latest: 0 for record the first error; 1 for the latest one\n"
	"       precise: 0 for imprecise; 1 for precise\n"
	"      rchkmode: 0 for do no check; 1 for using DG; 2 for checksum\n"
	"      burstlen: 0 for 16, 1 for 32, 2 for 64, and 3 for 128 bytes\n"
	"        pktlen: 1 ~ 8160 bytes\n"
	"          iter: 0 for infinite; others for 1 ~ 4G times\n"
	"     resp_time: 0 ~ 4G cycles\n"
	"    perid_time: 0 ~ 4G cycles\n"
	"     addr_base: physical address BTG starts from\n"
	"     addr_mask: combine with addr_base to form a region\n"
	"      addr_gap: distance for next WR/RD\n"
	"  suggested flow:\n"
	"    btg all reset => btg ... set ... => btg ... start\n"
};
#endif

void _btg_prompt(void) {
	printf(btg_prompt_string);
	return;
}

int _btg_indie_cmd_parse(int argc, char argv[MAX_ARGS][MAX_ARG_LEN+1], btg_para_v3_t *btg_entry) {
	int i;

#if 0	
	printf("[%s] ", __func__);
	for (i = 0; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n\r");
#endif

	if (!ISBTGAVAL(btg_entry)) {
		printf("EE: BTG-LX%d is not available\n", btg_entry->lxid);
		return -1;
	}

	if (argc < 4) {
		_btg_prompt();
		return -1;
	}

	if (ISARG3("set")) {
		if (argc != 6) {
			_btg_prompt();
			return -1;
		}

		SETIFVAR(rec_latest);
		SETIFVAR(precise);
		SETIFVAR(rchkmode);
		SETIFVAR(burstlen);
		SETIFVAR(pktlen);
		SETIFVAR(iter);
		SETIFVAR(resp_time);
		SETIFVAR(perid_time);
		SETIFVARH(addr_base);
		SETIFVARH(addr_mask);
		SETIFVAR(addr_gap);

		printf("EE: unknown parameter\n");
		return -1;
	} else if (ISARG3("info")) {
		_btg_para_decode(btg_entry);
	} else if (ISARG3("reg")) {
		_btg_show_reg(btg_entry);
	} else if (ISARG3("start")) {
		_btg_setup(btg_entry);
		_btg_start(btg_entry);
	} else if (ISARG3("stop")) {
		_btg_stop(btg_entry);
	} else {
		_btg_prompt();
		return -1;
	}

	return 0;
}

int _btg_all_cmd_parse(int argc, char argv[MAX_ARGS][MAX_ARG_LEN+1]) {
	btg_para_v3_t *btg_entry = &btg[0];
	int i;
	
	printf("[%s] ", __func__);
	for (i = 0; i < argc; i++) {
		printf("%s ", argv[i]);
	}
	printf("\n\r");
	
	while (!ISBTGEND(btg_entry)) {
		if (ISARG2("reset")) {
			_btg_reset();
			break;
		} else if (ISARG2("info")) {
			_btg_para_decode(btg_entry);
		} else if (ISARG2("start")) {
			if (!ISBTGAVAL(btg_entry)) {
				printf("II: BTG-LX%d(%s) is NOT available\n",
				       btg_entry->lxid,
				       (btg_entry->feat == BTG_WRITE) ? "WR" : "RD");
			} else {
				_btg_setup(btg_entry);
				_btg_start(btg_entry);
			}
		} else if (ISARG2("stop")) {
			_btg_stop(btg_entry);
		} else if (ISARG2("reg")) {
			_btg_show_reg(btg_entry);
		} else {
			_btg_prompt();
			return -1;
		}
		btg_entry++;
	}

	return 0;
}

int btg_sub_sys(int argc, char argv[MAX_ARGS][MAX_ARG_LEN+1]) {
	int32_t btg_sel, btg_func_sel;
	btg_para_v3_t *btg_entry;

	int i;
	//printf("entering %s\n\r", __func__);

#if 0	
	printf("argv is: ");
	for (i = 0; i < argc; i++) {
		if(argv[i])
			printf("%s ", argv[i]);
		else printf("NULL ");
	}
	printf("\n\r");
#endif
	
#if 0	
	if (argc < 3) {
		_btg_prompt();
		return -1;
	}
#endif	

	if (ISARG1("help")) {
		_btg_prompt();
		return 0;
	}
	
	if (ISARG1("lx0") ||
	    ISARG1("lx1") ||
	    ISARG1("lx2")) {
		if (ISARG2("r") || ISARG2("w")) {
			//printf("[BTG] argv[1] is %s, argv[2] is %s\n\r", argv[1], argv[2]);
			
			btg_sel = (argv[1][2] - 48);

			//printf("[BTG] btg_sel is %d\n\r", btg_sel);
			
			/* bit 0 of 'r' is 0; 'w' is 1. */
			btg_func_sel = 1 - (argv[2][0] & 0x1);

			btg_entry = &btg[btg_sel*2+btg_func_sel];
			return _btg_indie_cmd_parse(argc, argv, btg_entry);
		} else {
			printf("EE: unknown BTG function\n");
		}
	} else if (ISARG1("lx3") || ISARG1("lx5")) {
        if (ISARG2("w")) {
			printf("[BTG] argv[1] is %s, argv[2] is %s\n\r", argv[1], argv[2]);
            btg_sel = (argv[1][2] - 48);
			printf("[BTG] btg_sel is %d\n\r", btg_sel);
            btg_entry = &btg[3+btg_sel];
            return _btg_indie_cmd_parse(argc, argv, btg_entry);
        } else {
            printf("EE: unknown BTG function\n");
        }
    } else if (ISARG1("lx4") || ISARG1("lx6")) {
        if (ISARG2("r")) {
			printf("[BTG] argv[1] is %s, argv[2] is %s\n\r", argv[1], argv[2]);
            btg_sel = (argv[1][2] - 48);
			printf("[BTG] btg_sel is %d\n\r", btg_sel);
            btg_entry = &btg[3+btg_sel];
            return _btg_indie_cmd_parse(argc, argv, btg_entry);
        } else {
            printf("EE: unknown BTG function\n");
        }
    } else if (ISARG1("all")) {
		printf("[BTG] argv[1] is %s\n\r", argv[1]);
		return _btg_all_cmd_parse(argc, argv);
	}

	printf("EE: unknown BTG\n");
	return -1;
}

#ifdef CONFIG_CMD_CONCURENT_BTG_TEST
void btg_status(btg_para_v3_t *btg_entry)
{
    while (!ISBTGEND(btg_entry)) {
        _btg_para_decode(btg_entry);
        btg_entry++;
    }
}

void btg_controller_init(btg_para_v3_t *btg_entry)
{
    _btg_reset();

    while (!ISBTGEND(btg_entry)) {
        _btg_setup(btg_entry);
        btg_entry++;
    }
}
#endif
