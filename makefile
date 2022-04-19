# 2021-∞ (c) blurryroots innovation qanat OÜ. All rights reserved.
# See license.md for details.

all: prepare build build-examples
all-release: prepare-release build
examples: build-examples

prepare:
	cmake -B staging -DCMAKE_BUILD_TYPE=Debug -DUSE_GEDANKEN=1

prepare-release:
	cmake -B staging -DUSE_GEDANKEN=1

analysis:
	@echo "Running static code check ..."
	@cppcheck --std=c11 \
		--enable=warning --enable=style \
		--includes-file=etc/analyze-include-dirs.list \
		--file-list=etc/analyze-source-files.list

build:
	cmake --build staging

build-examples:
	cmake --build staging --target kaji-example-counter
	cmake --build staging --target kaji-example-gedanken

build-docs:
	@echo "Building docxygen documentation to docs/html ..."
	@doxygen docs/Doxyfile > docs/Doxyfile.log 2> docs/Doxyfile.err.log

clean:
	rm -rf staging

run:
	staging/./kaji-example-counter $(MODE)

run-gedanken:
	staging/./kaji-example-gedanken

symbols:
	nm -an staging/kaji-example-gedanken | c++filt

grind-gedanken:
	valgrind --leak-check=full staging/kaji-example-gedanken