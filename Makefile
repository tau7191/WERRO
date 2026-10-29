BUILD_DIR = build

all: build

build:
	@cmake --preset default
	@cmake --build --preset default --parallel

run: build
	@cd $(BUILD_DIR) && ./werro

clean:
	@rm -rf $(BUILD_DIR)

.PHONY: all build run clean