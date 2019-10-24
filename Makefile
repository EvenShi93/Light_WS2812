
 PROJECT_NAME = WS2812

CMAKE_GENERATOR ?= "Unix Makefiles"
MAKE = make
MAKE_ARGS = -j8
STLINKCONFIG = interface/stlink-v2.cfg 
STLINKMCUCONFIG = target/stm32f0x.cfg
JLINKCONFIG = interface/jlink.cfg 
JLINKMCUCONFIG = target/stm32f0x.cfg
BUILD_TYPE = Debug
OPENOCD = openocd

define cmake-build
+@$(eval BUILD_DIR = build)

+@if [ ! -e $(BUILD_DIR)/CMakeCache.txt ]; then \
	mkdir -p $(BUILD_DIR) \
	&& (cd $(BUILD_DIR) \
	&& cmake ../code \
	-DCMAKE_PROJECT_NAME=$(PROJECT_NAME)\
	-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
	-G$(CMAKE_GENERATOR)) \
	|| (rm -rf $(BUILD_DIR)) \
fi

+@(echo "Build dir: $(BUILD_DIR)" \
	&& (cd $(BUILD_DIR) && $(MAKE) $(MAKE_ARGS)) \
  )
endef

all:
	@make clean
	$(call cmake-build, ${PROJECT_NAME}.elf)

upload: all stlink-flash
	@echo "Upload have done!"

stlink-flash:  
	$(OPENOCD) --search ../build -f $(STLINKCONFIG) -f $(STLINKMCUCONFIG) -c init -c "reset halt" -c "flash write_image erase ./build/$(PROJECT_NAME).bin 0x08000000" -c "reset run" -c shutdown

jlink-flash: 
	$(OPENOCD) --search ../build -f $(JLINKCONFIG) -c "transport select swd" -f $(JLINKMCUCONFIG) -c init -c "reset init" -c halt -c "flash write_image erase ./build/$(PROJECT_NAME).bin 0x08000000" -c "reset run" -c shutdown

clean:
	@rm -rf build/

