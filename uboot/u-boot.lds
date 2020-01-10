OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
SECTIONS
{
 . = 0x00000000;
 . = ALIGN(4);
 .text :
 {
  __image_copy_start = .;
  arch/arm/cpu/armv7/start.o (.text)
  *(.text)
 }
 . = ALIGN(4);
 .golden_section : {
  __golden_sec_start = .;
  __golden_magic_1 = .;
  . += 0x4;
  __golden_magic_2 = .;
  . = ALIGN(0x10);
  *(.golden_data)
  *(.golden_func)
  __golden_sec_end = .;
 }
 . = ALIGN(4);
 .rodata : { *(SORT_BY_ALIGNMENT(SORT_BY_NAME(.rodata*))) }
 . = ALIGN(4);
 .data : {
  *(.data)
  . = ALIGN(8);
  _f_exc_redirect_img = .;
  image/src/app/libbootload.o (.exc_redirect)
  . = ALIGN(8);
  _e_exc_redirect_img = .;
  _f_exc_dispatch_img = .;
  image/src/app/libbootload.o (.exc_dispatch)
  . = ALIGN(8);
  _e_exc_dispatch_img = .;
  _f_a_entry_img = .;
  image/src/app/libbootload.o (.a_entry)
  . = ALIGN(8);
  _e_a_entry_img = .;
  _f_v_entry_img = .;
  image/src/app/libbootload.o (.v_entry)
  . = ALIGN(8);
  _e_v_entry_img = .;
  _f_isrvideo_img = .;
  image/src/app/libbootload.o (.isrvideoimg)
  . = ALIGN(8);
  _e_isrvideo_img = .;
  _f_rosbootvector_img = .;
  image/src/app/libbootload.o (.rosbootvectorimg)
  . = ALIGN(8);
  _e_rosbootvector_img = .;
 }
 . = ALIGN(4);
 . = .;
 __u_boot_cmd_start = .;
 .u_boot_cmd : { *(.u_boot_cmd) }
 __u_boot_cmd_end = .;
 . = ALIGN(4);
 __image_copy_end = .;
 .rel.dyn : {
  __rel_dyn_start = .;
  *(.rel*)
  __rel_dyn_end = .;
 }
 .dynsym : {
  __dynsym_start = .;
  *(.dynsym)
 }
 _end = .;
 . = ALIGN(4096);
 .mmutable : {
  *(.mmutable)
 }
 .bss __rel_dyn_start (OVERLAY) : {
  __bss_start = .;
  *(.bss)
   . = ALIGN(4);
  __bss_end__ = .;
 }
 . += 0x10000;
 __stack_start = .;
 /DISCARD/ : { *(.dynstr*) }
 /DISCARD/ : { *(.dynamic*) }
 /DISCARD/ : { *(.plt*) }
 /DISCARD/ : { *(.interp*) }
 /DISCARD/ : { *(.gnu*) }
}
