set(COMPONENT_ADD_INCLUDEDIRS .)

set(COMPONENT_SRCS "nvs.c")

set(COMPONENT_REQUIRES console spi_flash)

register_component()
