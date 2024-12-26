set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_SRCS "wificonfig.c" "wificonfig_uart.c")

set(COMPONENT_REQUIRES console spi_flash)

register_component()
