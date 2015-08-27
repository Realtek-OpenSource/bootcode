#include <common.h>
#include <command.h>

#include <rtk/fw_info.h>
#include <rtk/boot.h>
#include <rtk/watchdog_api.h>


int rtk_plat_do_bootr(cmd_tbl_t *cmdtp, int flag, int argc, char * const argv[])
{
	int ret = RTK_PLAT_ERR_OK;

	/* reset boot flags */
	boot_from_flash = BOOT_FROM_FLASH_NORMAL_MODE;
	boot_from_usb = BOOT_FROM_USB_DISABLE;

	/* parse option */
	if (argc == 1) {
		boot_from_usb = BOOT_FROM_USB_DISABLE;
	} else if (argc == 2 && argv[1][0] == 'u') {
		if (argv[1][1] == 'z') {
			boot_from_usb = BOOT_FROM_USB_COMPRESSED;
		} else if (argv[1][1] == '\0') {
			boot_from_usb = BOOT_FROM_USB_UNCOMPRESSED;
		} else {
			return CMD_RET_USAGE;
		}
	}
	else if (argc == 2 && argv[1][0] == 'm') {
		boot_from_flash = BOOT_FROM_FLASH_MANUAL_MODE;
	} else {
		return CMD_RET_USAGE;
	}

	WATCHDOG_KICK();
	ret = rtk_plat_boot_handler();

#ifdef CONFIG_RESCUE_FROM_USB
	if (ret != RTK_PLAT_ERR_OK) {
		ret = boot_rescue_from_usb();
	}
#endif /* CONFIG_RESCUE_FROM_USB */
	if(!ret)
		return CMD_RET_SUCCESS;
	else
		return CMD_RET_FAILURE;
}

U_BOOT_CMD(
	bootr, 2, 0,	rtk_plat_do_bootr,
	"boot realtek platform",
	"[u/uz]\n"
	"\tu   - boot from usb\n"
	"\tuz  - boot from usb (use lzma image)\n"
	"\tm   - read fw from flash but boot manually (go all)\n"
);

