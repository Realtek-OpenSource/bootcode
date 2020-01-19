#include "asic_regs.h"


/*
 *  RTL865xC Initial Entry Point
 */
.ident   "Realtek Semiconductor Corporation"
.version "2005/8/26"

/*
 *  External Symbol For Reference
 */
.extern csp_main
.extern exception_handler

/*
 *  Start Section
 */
.section .start, "ax"
.p2align 2
.global _start
.ent _start
_start:

/*   .set  reorder   */

#if 0

   #if RTL8672_SIM
      /* set MCR */
      li    $8, MC_DCR_VAL
      li    $9, MC_DCR
      sw    $8, 0($9)

      /* set DTR */
      li    $8, MC_DTR_VAL
      li    $9, MC_DTR
      sw    $8, 0($9)
   #else

      /* set MCR */
      li    $8, MC_MCR_VAL
      li    $9, MC_MCR
      sw    $8, 0($9)

      /* set MTCR0 */
      li    $8, MC_MTCR0_VAL
      li    $9, MC_MTCR0
      sw    $8, 0($9)

      /* set MTCR1 */
      li    $8, MC_MTCR1_VAL
      li    $9, MC_MTCR1
      sw    $8, 0($9)
   #endif

   /* Jump to 0x80000000 */
   lui $8, 0x8000
   ori $8, 0x0000
   jr  $8
#endif

   /* set SDRAM parameters */
#if 1
   .word 0x24004790
   .word 0x24000050
#elif 0
   addiu $0, $0, 0x4790
   addiu $0, $0, 0x0050
#elif 0
   lui   $8, 0x9D00
   ori   $8, $8, 0x0010
   jr    $8
halts:
   jal   halts
   nop
   nop
   nop
   nop
   nop
   nop
#elif 0
   sleep
   nop
halts:
   jal   halts
   nop
   nop
   nop
   nop
   nop
#elif 0
   lui   $8, 0xBB00
   lui   $7, 0x1234
   lw    $9, 0x104($8)
   sw    $7, 0x104($8)
   nop
   nop
halts:
   jal   halts
   nop
   nop
#elif 0
   lui   $27, 0xb801
   ori   $27, $27, 0x9004
   lui   $26, 0xffff
   ori   $26, $26, 0xffff
   sw    $26, 0($27)
   lw    $26, 0($27)
#endif


#if !RAMBOOT_TEST && !RTL8389

   #if RTL8672_SIM
      /* set MCR */
      li    $8, MC_DCR_VAL
      li    $9, MC_DCR
      sw    $8, 0($9)

      /* set DTR */
      li    $8, MC_DTR_VAL
      li    $9, MC_DTR
      sw    $8, 0($9)
   #else

      #if RTL865xC
      /* set MCR */
      li    $8, MC_MCR_VAL
      li    $9, MC_MCR
      sw    $8, 0($9)
      #else
      /* set DCR */
      li    $8, MC_DCR_VAL
      li    $9, MC_DCR
      sw    $8, 0($9)

      /* set DTR */
      li    $8, MC_DTR_VAL
      li    $9, MC_DTR
      sw    $8, 0($9)
      #endif

      #if 0 //DPLL_TUNE
//      li    $8, 0xC0379CF8 // HS0: 350, HS1: 200
      li    $8, 0xC03F9CF8 // HS0: 400, HS1: 200
      li    $9, 0xB8003204
      sw    $8, 0($9)

      li    $8, 0x800003F8 // MEM: 160
      li    $9, 0xB8003208
      sw    $8, 0($9)

//      li    $8, 0x7        // SDRAM CLK Delay
//      li    $9, 0xB8003304
//      sw    $8, 0($9)
      #endif
   #endif

#elif RTL8389

   #if !RAMBOOT_TEST
   /* set MCR */
   li    $8, MC_MCR_VAL
   li    $9, MC_MCR
   sw    $8, 0($9)
   #endif

   /* set DCR */
   li    $8, MC_DCR_VAL
   li    $9, MC_DCR
   sw    $8, 0($9)

   /* set DTR */
   li    $8, MC_DTR_VAL
   li    $9, MC_DTR
   sw    $8, 0($9)

   /* set SFCR */
   li    $8, SFCR_VAL
   li    $9, SFCR
//   li    $9, SPI_VAR
   sw    $8, 0($9)

   /* set MPMR */
   li    $8, MPMR_VAL
   li    $9, MPMR
   sw    $8, 0($9)

   li    $9, 0xB8005100   // Disable Bus Timout Monitor
   sw    $0, 0($9)

#endif /* RTL8389 */


#if RTL865xC && !RTL8389

#if RAMBOOT_TEST

#if 1
   /* Set BEV = 0 */
   mfc0  $8, $12
   nop
   and   $8, 0xFFBFFFFF
   mtc0  $8, $12
#endif

#else

#if 1 & !RTL8672_SIM
   /* set PFCR */
   li    $8, MC_PFCR_VAL
   li    $9, MC_PFCR
   sw    $8, 0($9)
#endif

#endif

#endif

   /* Jump to Another Segment */

#if 0
   .word 0x1000006C
//   b     exception_handler
#else
   j     _start2
#endif


#if 0
   /* set STATUS reg. */
   mfc0  $9, $12
   nop
   srl   $9, $9, 2
   sll   $9, $9, 2
   mtc0  $9, $12
   nop

   /* set CAUSE reg. */
   mtc0  $0, $13
   nop

   /* set ESTATUS reg. */
   mtlxc0   $0, $0
   nop

   /* set CCTL reg. => Invalidate ICache/DCache */
   mtc0  $0, $20
   li    $8, 0x3
   mtc0  $8, $20
   nop

   /* Initialize COP3 */
   mfc0  $8, $12
   nop
   or    $8, 0x80000000
   mtc0  $8, $12

   /* Initialize IMEM */
   li    $8, IMEM_BASE
   li    $9, IMEM_TOP
   mtc3  $8, $0
   nop
   mtc3  $9, $1

   /* Initialize DMEM */
   li    $8, DMEM_BASE
   li    $9, DMEM_TOP
   mtc3  $8, $4
   nop
   mtc3  $9, $5
   nop

   /* set CCTL reg. => Invalidate ICache/DCache */
   mtc0  $0, $20
   nop

   /* set stack address */
   la    $sp, _stack_end

   jal   csp_main

stop:
   j     stop
   nop

#endif /* #if 0 */

.end _start



/*
 * Start2 Section
 */
.section .start2, "ax"
.p2align 2
.global _start2
.ent _start2
_start2:

   #if !RTL8672_SIM
   /* set STATUS reg. */
   mfc0  $9, $12
   nop
   srl   $9, $9, 2
   sll   $9, $9, 2
   mtc0  $9, $12
   nop

   /* set CAUSE reg. */
   mtc0  $0, $13
   nop

   /* set ESTATUS reg. */
   mtlxc0   $0, $0
   nop

   /* set CCTL reg. => Invalidate ICache/DCache */
   mtc0  $0, $20
   li    $8, 0x3
   mtc0  $8, $20
   nop

   /* Initialize COP3 */
   mfc0  $8, $12
   nop
   or    $8, 0x80000000
   mtc0  $8, $12

   /* Initialize IMEM */
   li    $8, IMEM_BASE
   li    $9, IMEM_TOP
   mtc3  $8, $0
   nop
   mtc3  $9, $1

   /* Initialize DMEM */
   li    $8, DMEM_BASE
   li    $9, DMEM_TOP
   mtc3  $8, $4
   nop
   mtc3  $9, $5
   nop

   /* set CCTL reg. => Invalidate ICache/DCache */
   mtc0  $0, $20
   nop
   #endif


#if 0
addr1:
   la    $8, addr1
#endif

/*
#if !RAMBOOT_TEST
   li    $8, MC_MCR_VAL
   li    $9, MC_MCR
   sw    $8, 0($9)

   li    $8, MC_MTCR0_VAL
   li    $9, MC_MTCR0
   sw    $8, 0($9)

   li    $8, MC_MTCR1_VAL
   li    $9, MC_MTCR1
   sw    $8, 0($9)
#endif
*/


#if 0
   /* Set Magic Number */
   li    $8, 0x77887788
   li    $9, 0x80100000
   sw    $8, 0($9)
#endif

/*
#if RTL865xC

#if RAMBOOT_TEST

#else

#if 1
   li    $8, MC_PFCR_VAL
   li    $9, MC_PFCR
   sw    $8, 0($9)
#endif

#endif

#endif
*/

   /* set stack address */
   la    $sp, _stack_end

#if 0
   la    $9, _vectors
   .set  reorder
#endif

#if 1 //!RTL8672_SIM
   jal   csp_main
#else /* This Route Skips Flash to DRAM copy */
   lui   $5, 0x8000
//   ori   $5, 0x4200
   ori   $5, 0x0000
   jalr  $5
#endif

stop:
   j     stop

.end _start2

