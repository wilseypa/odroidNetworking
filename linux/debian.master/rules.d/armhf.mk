human_arch	= ARM (hard float)
build_arch	= arm
header_arch	= arm
defconfig	= defconfig
flavours	= generic
build_image	= zImage
kernel_file	= arch/$(build_arch)/boot/zImage
install_file	= vmlinuz
no_dumpfile	= true

loader		= grub
do_tools	= true
do_tools_perf	= true

# Flavour specific configuration.
dtb_files_generic	= imx6q-sabrelite.dtb omap3-beagle-xm.dtb omap4-panda.dtb omap4-panda-es.dtb highbank.dtb
