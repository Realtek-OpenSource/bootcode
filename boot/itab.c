#include "board.h"

/******************************************************************
 * The table in this file is used during the basic initialization *
 * process. It configures a number of devices. Please don't touch *
 * this unless you know what you're doing.             			  *
 ******************************************************************/

struct vrent
{
    unsigned long      	reg;
    unsigned long      	value;
};

struct vrent basicInitTable[] =
{
	/* memory controller */
	{ 0xB9000000, MCR0_VAL },
	{ 0xB9000004, MCR1_VAL },  //reg not used in 8671
	{ 0xB9000008, MCR2_VAL },
	/* for system Interface */
	{ 0xB9C04000, SICR_VAL },
	/* terminator */
	{ 0x00000000, 0x00000000 }			
};

