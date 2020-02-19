idf_component_register(SRCS "main.c"
                            "protocal.c"
                            "bluetooth.c"
                            "uart.c"
                    INCLUDE_DIRS ".")
add_subdirectory(HMI)
add_subdirectory(camera)