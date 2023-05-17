############################# SPI #############################

#--------------------------- PMOD3 ---------------------------#

# GLOBAL SETTINGS
set_property PACKAGE_PIN AE12 [get_ports spi_0_clk]
set_property IOSTANDARD LVCMOS33 [get_ports spi_0_clk]

set_property PACKAGE_PIN AF11 [get_ports miso_0]
set_property IOSTANDARD LVCMOS33 [get_ports miso_0]

set_property PACKAGE_PIN AF12 [get_ports mosi_0]
set_property IOSTANDARD LVCMOS33 [get_ports mosi_0]


# CHIP SELECT (slave select)
set_property PACKAGE_PIN AG11 [get_ports {ss_0[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ss_0[0]}]

set_property PACKAGE_PIN AG10 [get_ports {ss_0[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ss_0[1]}]

set_property PACKAGE_PIN AH12 [get_ports {ss_0[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ss_0[2]}]

set_property PACKAGE_PIN AH10 [get_ports {ss_0[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {ss_0[3]}]

#---------------------------------------------------------------#

#################################################################