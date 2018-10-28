# Default configuration for nios2-softmmu

CONFIG_NIOS2=y
CONFIG_SERIAL=y
CONFIG_PTIMER=y
CONFIG_NAND=y
CONFIG_SD=y
CONFIG_SSI=y
CONFIG_SSI_M25P80=y
CONFIG_CADENCE=y
CONFIG_ALTERA_TIMER=y
CONFIG_ALTERA=y
CONFIG_ALTERA_JUART=y
CONFIG_FDT_GENERIC=$(CONFIG_FDT)
CONFIG_PFLASH_CFI01=y
CONFIG_SI57X=y
CONFIG_LABX=y

# Xilinx drivers included due to hw/core/fdt_generic_devices not really being generic

CONFIG_XILINX=y
CONFIG_XILINX_AXI=y
CONFIG_XILINX_SPI=y
CONFIG_XILINX_ETHLITE=y
CONFIG_XILINX_IOMODULE_UART=y
CONFIG_XILINX_IOMODULE_PIT=y
CONFIG_XILINX_IOMODULE_GPI=y
CONFIG_XILINX_IOMODULE_GPO=y
CONFIG_XILINX_IOMODULE_INTC=y
CONFIG_XLNX_ZYNQMP=y
CONFIG_XILINX_SPIPS=y
CONFIG_SDHCI=y
CONFIG_PCI=y
CONFIG_I2C=y
