include .config

ifeq ($(CONFIG_MIPS),y)
ifeq ($(CONFIG_MIPS_RUNS_LOADER),y)
SUBDIRS = util zlib tftpnaive tcp loader secboot boot
else
SUBDIRS = boot
endif
else
SUBDIRS = util zlib tftpnaive tcp loader boot
endif

ifeq ($(CONFIG_SPI_NAND_FLASH),y)
SUBDIRS += plr
endif

MAKE = make
SCRIPTSDIR = config/scripts
HOSTCC = unset GCC_EXEC_PREFIX; cc
ROOTDIR = $(shell pwd)
MIPSDIR = $(ROOTDIR)_MIPS 

export MAKE HOSTCC SCRIPTSDIR ROOTDIR

ifeq ($(CONFIG_BOOT_MIPS),y)
Default: gen_mips_boot subdirs
else
ifeq ($(CONFIG_MIPS),y)
Default: subdirs copy_mips_bin
else
Default: subdirs
#	@if egrep "^CONFIG_SPI_NAND_FLASH=y" .config > /dev/null; then \
	SUBDIRS += plr \
	fi
endif
endif

subdirs : $(SUBDIRS)
$(SUBDIRS)::
	$(MAKE) -C $@
	
cleanall:
	$(MAKE) clean -C util
	$(MAKE) clean -C boot
	$(MAKE) clean -C loader
	$(MAKE) clean -C tftpnaive
	$(MAKE) clean -C zlib
	$(MAKE) clean -C tcp
	$(MAKE) clean -C config
ifeq ($(CONFIG_SPI_NAND_FLASH),y)
	$(MAKE) clean -C plr
endif
ifeq ($(CONFIG_BOOT_MIPS),y)
	@if [ -f $(ROOTDIR)/mips.bin ]; then \
		rm $(ROOTDIR)/mips.bin; \
	fi
endif

#generate/clean  MIPS bootloader src
ifeq ($(CONFIG_RTL8685SB),y)
ifeq ($(CONFIG_BOOT_MIPS),y)
.PHONY: gen_mips_boot clean_mips_boot
gen_mips_boot:
	@if [ -d $(MIPSDIR) ]; then\
		echo "MIPS bootloader src already exists, generating is skipped."; \
	else \
		echo "generating MIPS bootloader src..."; \
		cp -rf $(ROOTDIR) $(MIPSDIR); \
	fi

	@echo "start to build MIPS bootloader"
	make cleanall -C $(MIPSDIR)

	@if [ -f $(MIPSDIR)/autoconf.h ]; then \
		rm $(MIPSDIR)/autoconf.h; \
	fi

	make preconfig_RTL8685SB_FPGA_MIPS -C $(MIPSDIR)
	make menuconfig -C $(MIPSDIR)
	make -C $(MIPSDIR)

clean_mips_boot:
	@if [ -d $(MIPSDIR) ]; then\
		echo "cleaning MIPS bootloader src..."; \
		rm -rf $(MIPSDIR); \
	else \
		echo "MIPS bootloader src does not exist."; \
	fi
endif #CONFIG_BOOT_MIPS

ifeq ($(CONFIG_MIPS),y)
.PHONY: copy_mips_bin
copy_mips_bin:
	cp $(ROOTDIR)/boot/mips.bin $(subst _MIPS,,$(ROOTDIR))/mips.bin
endif
endif #CONFIG_RTL8685SB

.PHONY: config.tk config.in

config.in:
	config/mkconfig > config.in

preconfig_%:
	@if [ -f $(ROOTDIR)/vendors/config_$(@:preconfig_%=%) ]; then \
		echo "Configuration $(@:preconfig_%=%) loaded."; \
		cp $(ROOTDIR)/vendors/config_$(@:preconfig_%=%) .config; \
		rm *_loaded ; \
		touch $@_loaded ; \
	else \
		echo "No configuration found"; \
	fi
	
.PHONY: menuconfig config
menuconfig: 
	$(MAKE) -C $(SCRIPTSDIR)/lxdialog all
	@HELP_FILE=config/Configure.help \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Menuconfig config.in
	@if [ ! -f .config ]; then \
		echo; \
		echo "You have not saved your config, please re-run make config"; \
		echo; \
		exit 1; \
	fi
#	@config/setconfig defaults
#	@if egrep "^CONFIG_DEFAULTS_KERNEL=y" .config > /dev/null; then \
		$(MAKE) linux_menuconfig; \
	fi
#	@if egrep "^CONFIG_DEFAULTS_MODULES=y" .config > /dev/null; then \
		$(MAKE) modules_menuconfig; \
	fi
#	@if egrep "^CONFIG_DEFAULTS_VENDOR=y" .config > /dev/null; then \
		$(MAKE) config_menuconfig; \
	fi
#	@config/setconfig final
	
config: 	
	@HELP_FILE=config/Configure.help PATH=$(shell pwd):$(PATH) \
		$(CONFIG_SHELL) $(SCRIPTSDIR)/Configure config.in
	#@config/setconfig defaults	
	@config/setconfig final
	
