.PHONY: all build build-release build-debug clean fmt

all: build

build: build-release

build-release:
	conan install . --build=missing -s build_type=Release
	cmake --preset conan-release
	cmake --build --preset conan-release

build-debug:
	conan install . --build=missing -s build_type=Debug
	cmake --preset conan-debug
	cmake --build --preset conan-debug

fmt:
	find src -name '*.cpp' -o -name '*.hh' -o -name '*.h' | xargs clang-format -i

clean:
	rm -rf build CMakeUserPresets.json
