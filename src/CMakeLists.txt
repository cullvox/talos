# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.


set(SOURCES
    "main.c"
    "bitmap.c"
    "display.c")

idf_component_register(
    SRCS ${SOURCES}
    PRIV_REQUIRES esp_driver_gpio esp_driver_spi esp_wifi esp_https_server)
