deps_config := \
	build/config.in

.config include/config.h: $(deps_config)

$(deps_config):
