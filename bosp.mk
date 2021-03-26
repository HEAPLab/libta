ifdef CONFIG_EXTERNAL_LIBTA

# Targets provided by this project
.PHONY: libta clean_libta

# Add this to the "external" target
external: libta
clean_external: clean_libta

MODULE_EXTERNAL_LIBTA = external/optional/libta

ifdef CONFIG_EXTERNAL_LIBTA_IMPL_BSCTA
LIBTA_IMPL=bscta
endif

ifdef CONFIG_EXTERNAL_LIBTA_IMPL_CHRONOVISE
LIBTA_IMPL=chronovise
endif

ifndef LIBTA_IMPL
    $(error LIBTA Implementation not set)
endif

libta:
	@echo
	@echo "==== Building Timing Analysis Library - libta ($(BUILD_TYPE)) ===="
	@[ -d $(MODULE_EXTERNAL_LIBTA)/build/$(BUILD_TYPE) ] || \
		mkdir -p $(MODULE_EXTERNAL_LIBTA)/build/$(BUILD_TYPE) || \
		exit 1
	@cd $(MODULE_EXTERNAL_LIBTA)/build/$(BUILD_TYPE) && \
		CC=$(CC) CFLAGS=$(TARGET_FLAGS) \
		CXX=$(CXX) CXXFLAGS=$(TARGET_FLAGS) \
		cmake $(CMAKE_COMMON_OPTIONS) -DIMPLEMENTATION=$(LIBTA_IMPL) ../.. || \
		exit 1
	@cd $(MODULE_EXTERNAL_LIBTA)/build/$(BUILD_TYPE) && \
		make -j$(CPUS) install || \
		exit 1

clean_libta:
	@echo
	@echo "==== Clean-up Timing Analysis Library - libta ($(BUILD_TYPE)) ===="
	@rm -rf $(BUILD_DIR)/lib/libta* $(BUILD_DIR)/include/libta
	@rm -rf $(MODULE_EXTERNAL_LIBTA)/build
	@echo

else # CONFIG_EXTERNAL_LIBTA

libta:
	$(warning external/libta module disabled by BOSP configuration)
	$(error BOSP compilation failed)

endif # CONFIG_EXTERNAL_LIBTA
