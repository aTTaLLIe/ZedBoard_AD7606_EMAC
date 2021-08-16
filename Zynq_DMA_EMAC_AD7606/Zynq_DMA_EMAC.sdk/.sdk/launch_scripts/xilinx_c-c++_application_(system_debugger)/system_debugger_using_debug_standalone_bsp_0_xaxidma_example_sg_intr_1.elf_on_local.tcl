connect -url tcp:127.0.0.1:3121
source D:/Zynq_other/Zynq_Projects/Zynq_DMA_EMAC_AD7606/Zynq_DMA_EMAC.sdk/design_1_wrapper_hw_platform_0/ps7_init.tcl
targets -set -filter {jtag_cable_name =~ "Digilent Zed 210248AC9031" && level==0} -index 1
fpga -file D:/Zynq_other/Zynq_Projects/Zynq_DMA_EMAC_AD7606/Zynq_DMA_EMAC.runs/impl_1/design_1_wrapper.bit
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zed 210248AC9031"} -index 0
loadhw -hw D:/Zynq_other/Zynq_Projects/Zynq_DMA_EMAC_AD7606/Zynq_DMA_EMAC.sdk/design_1_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zed 210248AC9031"} -index 0
stop
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zed 210248AC9031"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zed 210248AC9031"} -index 0
dow D:/Zynq_other/Zynq_Projects/Zynq_DMA_EMAC_AD7606/Zynq_DMA_EMAC.sdk/standalone_bsp_0_xaxidma_example_sg_intr_1/Debug/standalone_bsp_0_xaxidma_example_sg_intr_1.elf
configparams force-mem-access 0
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zed 210248AC9031"} -index 0
con
