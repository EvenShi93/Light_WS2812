cmake_minimum_required(VERSION 3.0.0)

project(${CMAKE_PROJECT_NAME})

string(TIMESTAMP DATE "%Y_%m_%d")

execute_process(
    COMMAND git describe --always --tags
    OUTPUT_VARIABLE git_tag
    OUTPUT_STRIP_TRAILING_WHITESPACE
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
		)
	
message(STATUS "FIRMWARE VERSION: ${CMAKE_PROJECT_NAME}_V${git_tag}_${DATE}")

set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
set(CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "")

include(Toolchain-arm-none-eabi.cmake)

add_definitions(
	-std=c99
	-O1
	-DSTM32F070
	-DUSE_STDPERIPH_DRIVER
	-DARM_MATH_CM0
	-ffunction-sections
	-Wl,-gc-sections
	-g
)

set(FLAGS "-mthumb -mcpu=cortex-m0")
set(CMAKE_C_FLAGS	${FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map,${CMAKE_PROJECT_NAME}.map ${FLAGS} -T ${CMAKE_SOURCE_DIR}/StartUp/stm32_flash.ld")

include_directories(
	.
	arm
	StartUp
	StdLib/inc
	App
)

add_executable(
	${CMAKE_PROJECT_NAME}.elf

	StartUp/system_stm32f0xx.c
	StartUp/startup_stm32f0xx.s

	StdLib/src/stm32f0xx_dma.c
	StdLib/src/stm32f0xx_flash.c
	StdLib/src/stm32f0xx_gpio.c
	StdLib/src/stm32f0xx_misc.c
	StdLib/src/stm32f0xx_rcc.c
	StdLib/src/stm32f0xx_tim.c

	App/Driver_Button.c
	App/Driver_Flash.c
	App/Driver_Led.c
	App/Driver_Pwmc.c
	App/Driver_SysTime.c
	App/Main.c
	App/Task.c
)

target_link_libraries(${CMAKE_PROJECT_NAME}.elf
	m
	c
	gcc
	rdimon
	nosys
)

set_property(
	SOURCE StartUp/startup_stm32f0xx.s
	PROPERTY LANGUAGE C
)

add_custom_target(${CMAKE_PROJECT_NAME} ALL
	COMMAND ${OBJCOPY} -O binary 	${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}_V${git_tag}_${DATE}.bin
	COMMAND ${OBJCOPY} -O ihex    ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}_V${git_tag}_${DATE}.hex
	COMMAND ${OBJCOPY} -O binary 	${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
	COMMAND ${OBJCOPY} -O ihex    ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
	DEPENDS ${CMAKE_PROJECT_NAME}.elf
)